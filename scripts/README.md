# scripts/

Tool bảo trì offset bộ nhớ cho H3roViewer.

## Vấn đề

H3roViewer đọc dữ liệu hero bằng cách bám vào một số địa chỉ hard-code trong
`src/memoryscanner.cpp`. Chúng chia làm hai nhóm:

| Nhóm | Trỏ vào | Có bị đổi khi HotA update? |
|---|---|---|
| `HOTA_DLL_TO_HERO_SECTION_POINTER_FALLBACK` | `hota.dll` | **Có — gần như mỗi bản** |
| `HOTA_EXE_TO_INIT_OFFSET`, `SOD_EXE_TO_INIT_OFFSET`, `EXE_TO_STATUS_OFFSET`, `EXE_TO_BATTLE_RESULT_OFFSET` | `h3hota.exe` / `Heroes3.exe` | Không — HotA không build lại exe (file vẫn là bản 2023) |

Mỗi bản HotA đều build lại `hota.dll`, `.data` nở ra, nên RVA của con trỏ tới
mảng hero dịch chỗ.

**Từ bản này trở đi app tự dò lấy giá trị đó lúc chạy** (`src/offsetscanner.cpp`),
nên HotA update không còn bắt buộc phải build lại. Hằng số trong
`memoryscanner.cpp` chỉ còn là giá trị dự phòng — xem mục
"[Tự dò lúc chạy](#tự-dò-lúc-chạy)" bên dưới.

**Quan trọng:** offset gắn với **build của `hota.dll`**, không gắn với version
string của HotA. HD Mod ship bản `hota.dll` riêng (xem `_HD3_Data/HotA.dll_*fix`)
và update theo nhịp của nó — R5, R8, R11 chỉ trong 10 ngày — nên cùng một
"1.8.1" vẫn có thể cần offset khác. Bảng dưới key theo **PE timestamp**.
Lấy timestamp bằng chính script (dòng `PE timestamp` trong output).

| HotA | hota.dll PE timestamp | Offset |
|---|---|---|
| 1.7.0 | | `0x489CB4` |
| 1.7.1 | | `0x490D70` |
| 1.7.2 | | `0x4A2E80` |
| 1.7.3 | | `0x4A6F08` |
| 1.8.0 | | `0x63515C` |
| 1.8.1 | `0x6A8D9427` | `0x6463CC` |
| 1.8.1 | `0x6A99ADE7` | `0x6463D4` |

Script `find_hota_offsets.py` dò lại giá trị này tự động thay vì phải RE tay.

## Tự dò lúc chạy

`src/offsetscanner.cpp` (`HeroPointerLocator`) chạy đúng thuật toán của script
này, nhưng đọc `hota.dll` **đã map trong process game** thay vì file trên đĩa.
`MemoryScanner::updateGameAddresses()` gọi nó mỗi giây.

Khác biệt quan trọng so với script offline: sau khi chọn ứng viên, nó **follow
con trỏ và kiểm tra dữ liệu thật** — đọc 16 hero đầu mảng rồi đối chiếu
`heroID == index`, tên hero in được, `color` trong `0..7` hoặc `0xFF`. Chỉ khi
đạt mới coi là "confirmed". Nhờ vậy heuristic "10 sites / 1 stores" của script
được xác nhận bằng dữ liệu chứ không phải chỉ đoán.

Thứ tự ưu tiên trong `resolve()`:

1. Giá trị đã cache trong `QSettings`, key theo **PE timestamp + SizeOfImage**
   của `hota.dll` — đổi DLL (kể cả HD Mod swap bản khác) là cache tự miss.
2. Ứng viên tốt nhất mà signature scan tìm được.
3. `HOTA_DLL_TO_HERO_SECTION_POINTER_FALLBACK` trong `memoryscanner.cpp`.

Mảng hero chưa tồn tại khi còn ở main menu, nên bước xác nhận được thử lại mỗi
giây cho tới khi vào map. Trong lúc chưa confirm, app vẫn dùng giá trị scan được
(bước 2), tức đã đúng cho bản `hota.dll` đang chạy.

Trong `Settings` có ô **Game memory** hiển thị trạng thái và nút
**Detect hero data offset again** — nút này xoá cache rồi dò lại từ đầu. Bình
thường không cần bấm; chỉ dùng khi nghi việc tự dò chốt nhầm giá trị.

Log ghi bằng `qInfo`, xem được khi bật debug messages trong cửa sổ About.

## Chạy như thế nào

Cần Python 3.6+ (chỉ dùng stdlib). **Không cần mở game** — script chỉ đọc file
`HotA.dll` / `h3hota.exe` trên đĩa.

```bash
# tự tìm thư mục game trong các đường dẫn thường gặp
python scripts/find_hota_offsets.py

# chỉ định thư mục game (khuyến nghị)
python scripts/find_hota_offsets.py --game-dir "D:/Game_Heroes_3/HoMM_3_Hota"

# dò xong ghi luôn vào src/memoryscanner.cpp
python scripts/find_hota_offsets.py --game-dir "D:/Game_Heroes_3/HoMM_3_Hota" --patch
```

`--patch` thay số trong dòng `constexpr uint32_t HOTA_DLL_TO_HERO_SECTION_POINTER_OFFSET`
và thêm `<version>: 0x<value>` vào dòng comment lịch sử ngay phía trên. Nó đọc số
version từ `HotA_Setup.ini` trong thư mục game, giữ nguyên CRLF, và chạy lại
nhiều lần không sao (idempotent).

Exit code: `0` = ổn, `1` = có warning cần đọc, `2` = không dò được (xem mục
"Script báo No candidate found" bên dưới).

## Output mẫu (HotA 1.8.1)

```
=== hota.dll: HOTA_DLL_TO_HERO_SECTION_POINTER_OFFSET ===
  .data spans RVA 0x25F000..0x4689908 (initialized up to 0x262000)
  imm32 0x492 (sizeof BaseHeroStruct) appears 40 times in .text

  candidates (RVA -> sites / runtime stores):
    0x006463CC   10 sites    1 stores  <-- best

=== src/gamestructs.h cross-checks (hota.dll .text) ===
  heroSpecialty (+0x1A)             21 accesses   ok
  color (+0x22)                    133 accesses   ok
  hotaHeroStructPointer (+0x47A)     5 accesses   ok

=== exe offsets (should not move; HotA does not rebuild the exe) ===
  HOTA_EXE_TO_INIT_OFFSET        RVA 0x00097F4  .text  h3hota.exe  operand -> 0x0069CCFC, 162 refs in hota.dll
  ...
==========================================================================
  HOTA_DLL_TO_HERO_SECTION_POINTER_OFFSET = 0x6463CC
==========================================================================
```

Kết quả tin được khi: **chỉ có 1 candidate**, số `sites` cách biệt hẳn candidate
thứ hai (script tự cảnh báo nếu không), có ít nhất 1 `stores`, và cả 3 dòng
cross-check đều `ok`.

## Script dò bằng cách nào

`sizeof(BaseHeroStruct) == 1170 == 0x492` chính là stride game dùng để index mảng
hero, nên mọi chỗ truy cập hero đều có dạng:

```asm
imul  esi, esi, 0x492          ; 69 F0 92 04 00 00
add   esi, [0x106463CC]        ; 03 35 CC 63 64 10   <-- con trỏ base cần tìm
```

Thuật toán:

1. Parse PE header của `hota.dll`, lấy `ImageBase` và khoảng RVA của `.text` / `.data`.
2. Quét `.text` tìm imm32 `92 04 00 00`.
3. Trong cửa sổ ±8/+24 byte quanh mỗi hit, bắt các lệnh truy cập global bằng địa
   chỉ tuyệt đối: `A1`/`A3` (mov eax), `03 05`/`8B 05`/`89 05`, `FF 35`.
4. Giữ các operand rơi vào `.data` và chia hết cho 4 (con trỏ luôn dword-aligned).
5. Chọn giá trị xuất hiện nhiều nhất; kiểm tra có lệnh **ghi** (`mov [global], reg`)
   ở đâu đó trong `.text` để chắc đó là *biến con trỏ* được gán lúc runtime, đúng
   như `MemoryScanner::getPointerValue()` mong đợi, chứ không phải bản thân mảng.
6. Kiểm tra chéo bằng các field trong `BaseHeroStruct` mà chỉ hero mới có:
   `color` ở `+0x22`, `heroSpecialty` ở `+0x1A`, `hotaHeroStructPointer` ở `+0x47A`.

## Lần sau lỗi thì sửa ở file nào

Tra theo triệu chứng:

### 1. App attach được, hiện màu player, nhưng hero trống / sai bét

Đây là ca thường gặp nhất sau khi update HotA (~95%), và giờ app phải tự xử lý
được. Kiểm tra theo thứ tự:

- Mở `Settings` → `Game memory`. Nếu ghi **auto detected** thì offset không phải
  nguyên nhân, xem tiếp ca 4.
- Nếu ghi **not confirmed**: vào một map rồi xem lại. Vẫn không confirm nghĩa là
  các invariant xác nhận trong `HeroPointerLocator::confirmCandidate()` không còn
  đúng — sang ca 2 và 3.
- Bấm **Detect hero data offset again** nếu nghi cache giữ giá trị sai.
- **Chỉ khi tự dò không được:** `src/memoryscanner.cpp` →
  `HOTA_DLL_TO_HERO_SECTION_POINTER_FALLBACK`, chạy
  `python scripts/find_hota_offsets.py --game-dir <...> --patch`, rồi build lại.

### 2. Script báo `No candidate found`

Nghĩa là `sizeof(BaseHeroStruct)` đã khác 1170 — HotA thêm field vào struct hero.
Đây là ca **duy nhất mà tự dò lúc chạy không cứu được**: `HeroPointerLocator` lấy
stride từ chính `sizeof(BaseHeroStruct)`, nên sửa struct là nó tự đúng lại, nhưng
sửa struct thì bắt buộc build lại. Phải làm cả hai:

- **Sửa:** `src/gamestructs.h` → `BaseHeroStruct` (điều chỉnh `padding*` cho khớp size mới)
- **Sửa:** `scripts/find_hota_offsets.py` → `HERO_STRUCT_SIZE`
- **Cách tìm size mới:** quét `.text` của `hota.dll` tìm các imm32 xuất hiện nhiều
  lần ngay trước một lệnh `add reg, [<global trong .data>]`. Stride mới sẽ là giá
  trị lớn hơn 1170 một chút và xuất hiện ở hàng chục chỗ.

### 3. Cross-check báo `MISSING` cho `color` / `heroSpecialty` / `hotaHeroStructPointer`

Offset field trong struct hero đã dịch.

- **Sửa:** `src/gamestructs.h` → `BaseHeroStruct` (và `HotAHeroStruct` nếu `+0x47A` sai)
- **Sửa:** `scripts/find_hota_offsets.py` → `HERO_FIELD_COLOR`, `HERO_FIELD_SPECIALTY`,
  `HERO_FIELD_EXT_POINTER`

### 4. Tên hero đúng nhưng số liệu sai (mana, movement, artifact, buff...)

Không phải chuyện offset — là layout struct bị lệch.

- **Sửa:** `src/gamestructs.h` → `BaseHeroStruct` / `HotAHeroStruct` / `StatusWindowStruct` / `PlayerStruct`
- **Lưu ý:** comment `// <byte offset>` ở cột phải trong `gamestructs.h` có vài chỗ
  ghi sai (`castedVisionLevel` và `helmet` đều ghi `301`; `arti5Type` ghi `451`
  nhưng thực tế 449). Đừng tin comment, hãy tự cộng lại từ đầu struct.

### 5. Secondary skill order sai, hoặc HotA thêm skill mới

- **Sửa:** `src/memoryscanner.cpp` → `NUMBER_OF_SKILLS_IN_HOTA` (1.8.0 đã tăng 29 → 30 khi thêm Rune)
- **Sửa:** `src/gamestructs.h` → `HeroSkillOrderStruct` (thêm field `<skill>OrderSkill`) và
  `BaseHeroStruct` (thêm field `<skill>Skill`)
- **Sửa:** `src/data_providers/skillprovider.cpp`, `src/imagesMap.h`,
  `resources/resources.qrc` + ảnh trong `resources/images/skills/`

### 6. App không tìm thấy game / không attach được

- **Sửa:** `src/memoryscanner.cpp` → `HOTA_PROCESS_NAME`, `HOTA_HD_PROCESS_NAME`,
  `HOMM_PROCESS_NAME`, `HOMM_HD_PROCESS_NAME`, `HOTA_DLL_NAME`
- **Lưu ý:** cả `GetHommPid()` và `GetModuleBaseAddress()` dùng `wcscmp` — **phân
  biệt hoa thường**. File trên đĩa tên `HotA.dll` nhưng so sánh với `L"hota.dll"`
  vẫn khớp, vì cả hai exe import DLL bằng chuỗi lowercase nên loader ghi
  `BaseDllName` là `hota.dll`. Nếu HotA đổi cách load DLL thì chỗ này vỡ —
  đổi sang `_wcsicmp` là an toàn hơn.

### 7. Script cảnh báo về `EXE_TO_STATUS_OFFSET` / `EXE_TO_BATTLE_RESULT_OFFSET` / `*_EXE_TO_INIT_OFFSET`

Chỉ xảy ra nếu HotA build lại `h3hota.exe` (chưa từng xảy ra kể từ 2023).

- **Sửa:** `src/memoryscanner.cpp` → 4 hằng số nhóm exe
- **Cách:** phải RE lại thủ công, script không dò được nhóm này. Lưu ý
  `HOTA_EXE_TO_INIT_OFFSET`/`SOD_EXE_TO_INIT_OFFSET` **không phải địa chỉ dữ liệu**
  mà là RVA trong `.text` trỏ tới toán hạng imm32 của một lệnh; đọc dword ở đó ra
  địa chỉ tuyệt đối của global con trỏ player (`0x69CCFC` cho HotA, `0x69CCB0`
  cho SoD). Cách này để không phải hard-code trực tiếp `0x29CCFC`.

### 8. Hero / creature / artifact mới của HotA không có ảnh

Không liên quan bộ nhớ.

- **Sửa:** `src/imagesMap.h`, `resources/resources.qrc`, và thêm ảnh vào
  `resources/images/{heroes,monsters,skills,specialty,items,scrolls,buffs}/`

## Build lại sau khi sửa offset

Toolchain đã dùng: Qt 6.9.0 mingw_64 + MinGW 13.1.0 (`D:/Qt`). Build out-of-source
rồi deploy vào `build/` ở gốc repo (`build/` đã được `.gitignore`):

```bash
export PATH="/d/Qt/6.9.0/mingw_64/bin:/d/Qt/Tools/mingw1310_64/bin:$PATH"

BLD=/tmp/h3rv-build && rm -rf $BLD && mkdir -p $BLD && cd $BLD
qmake /d/Projects/heroes3_tools/h3roviewer/h3roviewer.pro CONFIG+=release
mingw32-make -j8 release

OUT=/d/Projects/heroes3_tools/h3roviewer/build
rm -rf $OUT && mkdir -p $OUT
cp release/h3roviewer.exe release/README.md release/LICENSE.md $OUT/
windeployqt.exe --release --compiler-runtime $OUT/h3roviewer.exe
```

Lưu ý:

- `.pro` bật `-Werror`, nên mọi warning đều làm build fail.
- `CONFIG += c++23` cần GCC 13+.
- `windeployqt --compiler-runtime` là bắt buộc để đóng gói `libgcc_s_seh-1.dll`,
  `libstdc++-6.dll`, `libwinpthread-1.dll` — thiếu là máy khác không chạy được.
- Target `copyReadme`/`copyLicense` trong `.pro` tự copy `README.md` và
  `LICENSE.md` vào `$$OUT_PWD/release/`.

## Việc còn có thể làm: cho app tự dò lúc chạy

Thuật toán trong script hoàn toàn port được sang C++ để H3roViewer không cần
build lại mỗi bản HotA:

- Chỗ đặt: `MemoryScanner::updateGameAddresses()` trong `src/memoryscanner.cpp`,
  làm fallback khi đọc bằng offset hard-code trả về giá trị không hợp lệ.
- Vẫn dùng `ReadProcessMemory` như hiện tại: đọc PE header tại `dllBaseAddress`
  (`e_lfanew` ở `+0x3C`) để lấy bảng section, đọc `.text` ra buffer, quét pattern
  y như bước 2–5 ở trên. Địa chỉ tuyệt đối trong ảnh đã map sẵn là hậu-relocation
  nên trừ đi `dllBaseAddress` là ra RVA.
- Nên validate kết quả trước khi tin: đọc thử hero đầu tiên và kiểm tra
  `color <= 8` cùng `heroName` toàn ký tự ASCII in được.

## Dead code có thể xoá

`HOTA_TO_MAP_NAME_OFFSET` và `HOTA_TO_MP_MAP_NAME_OFFSET` trong
`src/memoryscanner.cpp` không được dùng ở đâu cả. Riêng `0x1c67b8` còn nằm trong
`.text` và trỏ vào `90 90 90 90` (NOP padding) — nó không phải địa chỉ dữ liệu.
