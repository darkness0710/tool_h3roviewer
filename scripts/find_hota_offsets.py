#!/usr/bin/env python3
"""
Re-detect the hardcoded game memory offsets used by H3roViewer.

Every HotA release rebuilds hota.dll, which shifts the RVA of the hero array
base pointer and breaks HOTA_DLL_TO_HERO_SECTION_POINTER_OFFSET in
src/memoryscanner.cpp. This script finds the new value by signature scanning
instead of by manual reverse engineering, and sanity-checks the offsets that
point into h3hota.exe as well.

    python scripts/find_hota_offsets.py                     # auto-locate the game
    python scripts/find_hota_offsets.py --game-dir "D:/HotA"
    python scripts/find_hota_offsets.py --patch             # also update the source

Stdlib only, no third party modules. See scripts/README.md for the details.
"""

import argparse
import os
import re
import struct
import sys
from collections import Counter

# --------------------------------------------------------------------------
# Mirrors of the constants in src/memoryscanner.cpp and src/gamestructs.h.
# Keep these in sync with the C++ side.
# --------------------------------------------------------------------------

SOURCE_FILE = os.path.join("src", "memoryscanner.cpp")
CONSTANT_NAME = "HOTA_DLL_TO_HERO_SECTION_POINTER_OFFSET"

# sizeof(BaseHeroStruct) - the stride used to index the hero array.
HERO_STRUCT_SIZE = 1170                      # 0x492

# Field offsets inside BaseHeroStruct, used to confirm the candidate really is
# the hero array and not some other array with the same element size.
HERO_FIELD_SPECIALTY = 0x1A                  # heroSpecialty, byte 26
HERO_FIELD_COLOR = 0x22                      # color, byte 34
HERO_FIELD_EXT_POINTER = 0x47A               # hotaHeroStructPointer, byte 1146

# Offsets into h3hota.exe / Heroes3.exe. These live in a binary that HotA has
# not rebuilt since 2023, so they are expected to stay valid - the script only
# verifies that they still look alive.
# ("constant", rva, kind, which binary to read it from)
EXE_OFFSETS = [
    ("HOTA_EXE_TO_INIT_OFFSET", 0x97F4, "code", "hota"),
    ("SOD_EXE_TO_INIT_OFFSET", 0x3BB4, "code", "sod"),
    ("EXE_TO_STATUS_OFFSET", 0x2992B8, "data", "hota"),
    ("EXE_TO_BATTLE_RESULT_OFFSET", 0x294DAC, "data", "hota"),
]

GAME_DIR_CANDIDATES = [
    r"C:/Program Files (x86)/HotA",
    r"D:/Games/HotA",
]


# --------------------------------------------------------------------------
# Minimal PE reader
# --------------------------------------------------------------------------

class PEFile(object):
    """Just enough PE32 parsing to map RVAs and read section bytes."""

    def __init__(self, path):
        self.path = path
        with open(path, "rb") as handle:
            self.raw = handle.read()

        if self.raw[:2] != b"MZ":
            raise ValueError("%s is not a PE image" % path)

        pe = struct.unpack_from("<I", self.raw, 0x3C)[0]
        if self.raw[pe:pe + 4] != b"PE\0\0":
            raise ValueError("%s has no PE signature" % path)

        coff = pe + 4
        section_count = struct.unpack_from("<H", self.raw, coff + 2)[0]
        self.timestamp = struct.unpack_from("<I", self.raw, coff + 4)[0]
        optional_size = struct.unpack_from("<H", self.raw, coff + 16)[0]

        optional = coff + 20
        if struct.unpack_from("<H", self.raw, optional)[0] != 0x10B:
            raise ValueError("%s is not PE32 (32 bit)" % path)
        self.image_base = struct.unpack_from("<I", self.raw, optional + 28)[0]
        self.image_size = struct.unpack_from("<I", self.raw, optional + 56)[0]

        self.sections = []
        for index in range(section_count):
            entry = optional + optional_size + index * 40
            name = self.raw[entry:entry + 8].rstrip(b"\0").decode("latin1")
            virtual_size, rva, raw_size, raw_ptr = struct.unpack_from("<IIII", self.raw, entry + 8)
            self.sections.append({
                "name": name,
                "rva": rva,
                "virtual_size": virtual_size,
                "raw_ptr": raw_ptr,
                "raw_size": raw_size,
            })

    def section(self, name):
        for section in self.sections:
            if section["name"] == name:
                return section
        return None

    def section_of(self, rva):
        for section in self.sections:
            span = max(section["virtual_size"], section["raw_size"])
            if section["rva"] <= rva < section["rva"] + span:
                return section
        return None

    def section_bytes(self, name):
        section = self.section(name)
        if section is None:
            return b""
        return self.raw[section["raw_ptr"]:section["raw_ptr"] + section["raw_size"]]

    def read_u32(self, rva):
        """Read a dword that is present in the file (returns None for BSS)."""
        section = self.section_of(rva)
        if section is None:
            return None
        delta = rva - section["rva"]
        if delta + 4 > section["raw_size"]:
            return None
        offset = section["raw_ptr"] + delta
        if offset + 4 > len(self.raw):
            return None
        return struct.unpack_from("<I", self.raw, offset)[0]

    def count_refs(self, absolute_va):
        """How many times an absolute address appears as a literal dword."""
        return len(re.findall(re.escape(struct.pack("<I", absolute_va)), self.raw))


# --------------------------------------------------------------------------
# The signature scan
# --------------------------------------------------------------------------

# Instruction forms that load or store a global through an absolute address.
# (opcode, extra byte or None, offset of the imm32 within the instruction)
ABSOLUTE_FORMS = [
    (0xA1, None, 1),        # mov eax, [imm32]
    (0xA3, None, 1),        # mov [imm32], eax
    (0x03, 0x05, 2),        # add r32, [imm32]      (modrm mod=00 rm=101)
    (0x8B, 0x05, 2),        # mov r32, [imm32]
    (0x89, 0x05, 2),        # mov [imm32], r32
    (0xFF, 0x35, 2),        # push [imm32]
]
# 0x03/0x8B/0x89 encode the destination register in modrm bits 3-5, so accept
# any modrm whose mod/rm part selects "disp32 only".
ABSOLUTE_MODRM_MASK = 0xC7
ABSOLUTE_MODRM_VALUE = 0x05

WINDOW_BEFORE = 8
WINDOW_AFTER = 24


def find_absolute_operands(code, position, image_base, image_size):
    """Collect absolute addresses referenced in a window around `position`."""
    lo = max(0, position - WINDOW_BEFORE)
    hi = min(len(code), position + WINDOW_AFTER)
    window = code[lo:hi]
    found = []
    for index in range(len(window) - 5):
        opcode = window[index]
        for form_opcode, form_extra, imm_at in ABSOLUTE_FORMS:
            if opcode != form_opcode:
                continue
            if form_extra is not None:
                modrm = window[index + 1]
                if (modrm & ABSOLUTE_MODRM_MASK) != ABSOLUTE_MODRM_VALUE:
                    continue
            if index + imm_at + 4 > len(window):
                continue
            value = struct.unpack_from("<I", window, index + imm_at)[0]
            if image_base <= value < image_base + image_size:
                found.append((value - image_base, form_opcode, form_extra))
            break
    return found


def detect_hero_pointer(dll):
    """Locate the hero array base pointer inside hota.dll's .data section."""
    text = dll.section_bytes(".text")
    text_rva = dll.section(".text")["rva"]
    data = dll.section(".data")
    if data is None:
        raise ValueError("hota.dll has no .data section")
    data_lo = data["rva"]
    data_hi = data["rva"] + max(data["virtual_size"], data["raw_size"])
    data_initialized_hi = data["rva"] + data["raw_size"]

    stride = struct.pack("<I", HERO_STRUCT_SIZE)
    votes = Counter()
    stores = Counter()
    sites = {}

    position = text.find(stride)
    while position >= 0:
        for rva, opcode, extra in find_absolute_operands(text, position + 4,
                                                         dll.image_base, dll.image_size):
            # A pointer variable is always dword aligned; anything else is a
            # misaligned read that happened to land inside .data.
            if rva % 4 or not (data_lo <= rva < data_hi):
                continue
            votes[rva] += 1
            sites.setdefault(rva, []).append(text_rva + position)
            if opcode == 0xA3 or (opcode == 0x89 and extra == 0x05):
                stores[rva] += 1
        position = text.find(stride, position + 4)

    # A store to the global anywhere in .text proves it is a pointer variable
    # that gets assigned at runtime, which is what getPointerValue() expects.
    for rva in list(votes):
        if stores[rva]:
            continue
        absolute = struct.pack("<I", dll.image_base + rva)
        for match in re.finditer(re.escape(absolute), text):
            head = match.start()
            if head >= 2 and text[head - 2] == 0x89 and (text[head - 1] & ABSOLUTE_MODRM_MASK) == ABSOLUTE_MODRM_VALUE:
                stores[rva] += 1
            elif head >= 1 and text[head - 1] == 0xA3:
                stores[rva] += 1

    return {
        "votes": votes,
        "stores": stores,
        "sites": sites,
        "data_lo": data_lo,
        "data_hi": data_hi,
        "data_initialized_hi": data_initialized_hi,
        "stride_hits": len(re.findall(re.escape(stride), text)),
    }


def confirm_hero_fields(dll):
    """Look for the field accesses that identify the struct as a hero."""
    text = dll.section_bytes(".text")
    results = {}
    for label, field in [("heroSpecialty (+0x1A)", HERO_FIELD_SPECIALTY),
                         ("color (+0x22)", HERO_FIELD_COLOR)]:
        # movsx r32, byte [reg+disp8]  /  mov [reg+reg+disp8], r8
        pattern = re.compile(re.escape(bytes([0x0F, 0xBE])) + b"(.)" + re.escape(bytes([field])), re.S)
        results[label] = len(pattern.findall(text))
    disp32 = struct.pack("<I", HERO_FIELD_EXT_POINTER)
    count = 0
    for match in re.finditer(re.escape(disp32), text):
        head = match.start()
        if head >= 2 and text[head - 2] == 0x8B and (text[head - 1] >> 6) == 0b10:
            count += 1
    results["hotaHeroStructPointer (+0x47A)"] = count
    return results


# --------------------------------------------------------------------------
# Game discovery
# --------------------------------------------------------------------------

def resolve_game_dir(explicit):
    if explicit:
        if not os.path.isdir(explicit):
            raise SystemExit("Not a directory: %s" % explicit)
        return explicit
    for candidate in GAME_DIR_CANDIDATES:
        if os.path.isfile(find_file(candidate, "HotA.dll") or ""):
            return candidate
    raise SystemExit(
        "Could not find a HotA installation. Pass --game-dir <path to the folder\n"
        "containing HotA.dll and h3hota.exe>."
    )


def find_file(directory, name):
    """Case-insensitive lookup, because HotA.dll / hota.dll both occur."""
    if not os.path.isdir(directory):
        return None
    lowered = name.lower()
    for entry in os.listdir(directory):
        if entry.lower() == lowered:
            return os.path.join(directory, entry)
    return None


def read_installed_version(game_dir):
    ini = find_file(game_dir, "HotA_Setup.ini")
    if not ini:
        return None
    try:
        with open(ini, "r", encoding="utf-8", errors="replace") as handle:
            for line in handle:
                if line.lower().startswith("main version"):
                    return line.split("=", 1)[1].strip()
    except OSError:
        return None
    return None


# --------------------------------------------------------------------------
# Source patching
# --------------------------------------------------------------------------

CONSTANT_RE = re.compile(
    (r"(constexpr\s+uint32_t\s+" + CONSTANT_NAME + r"\s*=\s*0x)([0-9A-Fa-f]+)(\s*;)").encode("ascii")
)
# The trailing \r? matters: memoryscanner.cpp uses CRLF, so with re.M the "$"
# anchor sits before the \n and the CR has to be consumed explicitly.
HISTORY_RE = re.compile(rb"^(//[^\r\n]*\b1\.7\.0:[^\r\n]*?)[ \t]*\r?$", re.M)


def current_source_value(repo_root):
    path = os.path.join(repo_root, SOURCE_FILE)
    with open(path, "rb") as handle:
        blob = handle.read()
    match = CONSTANT_RE.search(blob)
    if not match:
        raise SystemExit("Could not find %s in %s" % (CONSTANT_NAME, SOURCE_FILE))
    return int(match.group(2), 16), path, blob


def patch_source(repo_root, new_value, version, timestamp):
    old_value, path, blob = current_source_value(repo_root)
    if old_value == new_value:
        print("  %s already reads 0x%X - nothing to patch." % (SOURCE_FILE, new_value))
        return False

    blob = CONSTANT_RE.sub(lambda m: m.group(1) + (b"%X" % new_value) + m.group(3), blob, count=1)

    # HotA rebuilds hota.dll without bumping the version string, so two builds
    # can both call themselves e.g. "1.8.1" while needing different offsets.
    # Key the history entry by version *and* PE timestamp to keep it unambiguous.
    label = ("%s/%08X" % (version or "unknown", timestamp)).encode("ascii", "replace")
    entry = b"  " + label + (b": 0x%X" % new_value)
    match = HISTORY_RE.search(blob)
    if match and entry.strip() not in match.group(1):
        blob = blob[:match.end(1)] + entry + blob[match.end(1):]

    with open(path, "wb") as handle:
        handle.write(blob)
    print("  %s: 0x%X -> 0x%X (patched)" % (SOURCE_FILE, old_value, new_value))
    return True


# --------------------------------------------------------------------------
# Reporting
# --------------------------------------------------------------------------

def report(game_dir, repo_root, do_patch):
    version = read_installed_version(game_dir)
    dll_path = find_file(game_dir, "HotA.dll")
    exe_path = find_file(game_dir, "h3hota.exe") or find_file(game_dir, "h3hota HD.exe")

    if not dll_path:
        raise SystemExit("HotA.dll not found in %s" % game_dir)
    if not exe_path:
        raise SystemExit("h3hota.exe not found in %s" % game_dir)

    print("Game directory : %s" % game_dir)
    print("HotA version   : %s" % (version or "unknown (HotA_Setup.ini missing)"))
    print()

    dll = PEFile(dll_path)
    exe = PEFile(exe_path)
    sod_path = find_file(game_dir, "Heroes3.exe")
    sod = PEFile(sod_path) if sod_path else None

    for label, binary in [("hota.dll", dll), ("hota exe", exe), ("sod exe ", sod)]:
        if binary is None:
            continue
        print("%s  %-14s %9d bytes  ImageBase 0x%08X  PE timestamp 0x%08X"
              % (label, os.path.basename(binary.path), len(binary.raw),
                 binary.image_base, binary.timestamp))
    print()

    # ---- hota.dll: the offset that actually moves -------------------------
    print("=== hota.dll: %s ===" % CONSTANT_NAME)
    result = detect_hero_pointer(dll)
    print("  .data spans RVA 0x%X..0x%X (initialized up to 0x%X)"
          % (result["data_lo"], result["data_hi"], result["data_initialized_hi"]))
    print("  imm32 0x%X (sizeof BaseHeroStruct) appears %d times in .text"
          % (HERO_STRUCT_SIZE, result["stride_hits"]))

    if not result["votes"]:
        print()
        print("  !! No candidate found. The hero struct size probably changed,")
        print("     which means src/gamestructs.h needs work too - see scripts/README.md.")
        return 2

    print()
    print("  candidates (RVA -> sites / runtime stores):")
    for rva, count in result["votes"].most_common(8):
        marker = "  <-- best" if rva == result["votes"].most_common(1)[0][0] else ""
        print("    0x%08X   %2d sites   %2d stores%s" % (rva, count, result["stores"][rva], marker))

    best, best_votes = result["votes"].most_common(1)[0]
    runner_up = result["votes"].most_common(2)[1][1] if len(result["votes"]) > 1 else 0
    print()
    print("  first referencing sites: %s"
          % ", ".join("RVA 0x%X" % site for site in sorted(set(result["sites"][best]))[:6]))

    warnings = []
    if len(result["votes"]) > 1 and best_votes <= runner_up * 2:
        warnings.append("more than one plausible candidate - verify manually before trusting this")
    if not result["stores"][best]:
        warnings.append("no runtime store to this global was found; it may be the array itself "
                        "rather than a pointer to it")
    if best < result["data_initialized_hi"]:
        warnings.append("candidate lies in the initialized part of .data; a runtime pointer is "
                        "normally in the BSS tail")

    # ---- struct layout cross-checks --------------------------------------
    print()
    print("=== src/gamestructs.h cross-checks (hota.dll .text) ===")
    for label, count in confirm_hero_fields(dll).items():
        status = "ok" if count else "MISSING"
        print("  %-32s %3d accesses   %s" % (label, count, status))
        if not count:
            warnings.append("no code accesses %s - BaseHeroStruct/HotAHeroStruct may have changed"
                            % label)

    # ---- exe offsets: expected to be stable ------------------------------
    print()
    print("=== exe offsets (should not move; HotA does not rebuild the exe) ===")
    for name, rva, kind, target in EXE_OFFSETS:
        binary = sod if target == "sod" else exe
        if binary is None:
            print("  %-30s RVA 0x%07X  skipped (Heroes3.exe not installed)" % (name, rva))
            continue
        section = binary.section_of(rva)
        section_name = section["name"] if section else "OUT OF IMAGE"
        absolute = binary.image_base + rva
        note = ""
        if kind == "code":
            operand = binary.read_u32(rva)
            if operand is None:
                note = "unreadable"
                warnings.append("%s does not point at readable bytes" % name)
            else:
                note = "operand -> 0x%08X" % operand
                if binary is exe:
                    note += ", %d refs in hota.dll" % dll.count_refs(operand)
                else:
                    note += ", %d refs in Heroes3.exe" % binary.count_refs(operand)
                if not (binary.image_base <= operand < binary.image_base + binary.image_size):
                    warnings.append("%s no longer holds an address inside %s"
                                    % (name, os.path.basename(binary.path)))
        else:
            dll_refs = dll.count_refs(absolute)
            note = "%d refs in hota.dll" % dll_refs
            if dll_refs == 0 and binary.count_refs(absolute) == 0:
                warnings.append("%s is referenced nowhere; the global may be gone" % name)
        print("  %-30s RVA 0x%07X  %-8s  %-8s %s"
              % (name, rva, section_name, os.path.basename(binary.path), note))

    # ---- verdict ---------------------------------------------------------
    print()
    print("=" * 74)
    print("  %s = 0x%X" % (CONSTANT_NAME, best))
    print("=" * 74)

    if warnings:
        print()
        print("Warnings:")
        for warning in warnings:
            print("  - %s" % warning)

    print()
    source_value, _, _ = current_source_value(repo_root)
    if source_value == best:
        print("%s is already up to date (0x%X)." % (SOURCE_FILE, best))
    else:
        print("%s currently says 0x%X and needs to be updated to 0x%X."
              % (SOURCE_FILE, source_value, best))
        if do_patch:
            patch_source(repo_root, best, version, dll.timestamp)
        else:
            print("Re-run with --patch to apply it, or edit the line by hand:")
            print("    constexpr uint32_t %s = 0x%X;" % (CONSTANT_NAME, best))

    return 1 if warnings else 0


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--game-dir", help="folder containing HotA.dll and h3hota.exe")
    parser.add_argument("--patch", action="store_true",
                        help="write the detected offset into " + SOURCE_FILE)
    parser.add_argument("--repo-root", default=os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                        help="H3roViewer checkout to inspect/patch (default: parent of scripts/)")
    args = parser.parse_args(argv)

    game_dir = resolve_game_dir(args.game_dir)
    return report(game_dir, args.repo_root, args.patch)


if __name__ == "__main__":
    sys.exit(main())
