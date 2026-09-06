#include "memoryscanner.h"

#include <tlhelp32.h>
#include <psapi.h>
#include <wchar.h>

#include <QObject>
#include <QDebug>

constexpr uint32_t NO_HERO_SELECTED = 0xFFFFFFFF;

constexpr wchar_t HOMM_PROCESS_NAME[] = L"Heroes3.exe";
constexpr wchar_t HOMM_HD_PROCESS_NAME[] = L"Heroes3 HD.exe";
constexpr wchar_t HOTA_PROCESS_NAME[] = L"h3hota.exe";
constexpr wchar_t HOTA_HD_PROCESS_NAME[] = L"h3hota HD.exe";

constexpr wchar_t HOTA_DLL_NAME[] = L"hota.dll";

constexpr uint32_t SOD_EXE_TO_INIT_OFFSET = 0x3BB4;
constexpr uint32_t SOD_INIT_TO_PLAYERS_SECTION_OFFSET = 0x00;
constexpr uint32_t SOD_PLAYERS_TO_HEROES_OFFSET = 0xB50;

constexpr uint32_t HOTA_EXE_TO_INIT_OFFSET = 0x97F4;
constexpr uint32_t HOTA_INIT_TO_PLAYER_POINTER_OFFSET = 0x00;

// Only a fallback. Every HotA release rebuilds hota.dll and moves this offset,
// so HeroPointerLocator detects it from the running game instead. This value is
// used until that succeeds, and forever if it cannot.
constexpr uint32_t HOTA_DLL_TO_HERO_SECTION_POINTER_FALLBACK = 0x6463CC;

constexpr uint32_t EXE_TO_BATTLE_RESULT_OFFSET = 0x294DAC;
constexpr uint32_t EXE_TO_STATUS_OFFSET = 0x2992B8;

constexpr uint8_t NUMBER_OF_SKILLS_IN_SOD = 28;
constexpr uint8_t NUMBER_OF_SKILLS_IN_HOTA = 30;

constexpr uint32_t HOTA_TO_MAP_NAME_OFFSET = 0x299430;
constexpr uint32_t HOTA_TO_MP_MAP_NAME_OFFSET = 0x1c67b8;

#define POINTER_CASTING(addr) reinterpret_cast<LPCVOID>(static_cast<intptr_t>(addr))

HeroStructs * MemoryScanner::hero = nullptr;
PlayerStruct * MemoryScanner::player = nullptr;
GameStateStruct * MemoryScanner::gameState = nullptr;

MemoryScanner::MemoryScanner():
    QObject(nullptr),
    exeName(nullptr)
{
    this->pollRateTimer.setInterval(16);
    connect(&(this->pollRateTimer), &QTimer::timeout, this, &MemoryScanner::updateState);

    this->processCheckTimer.setInterval(2000);
    connect(&(this->processCheckTimer), &QTimer::timeout, this, &MemoryScanner::attachToProcess);

    // After HotA 1.7, some addresses are only available when a match
    this->gameAddressUpdateTimer.setInterval(1000);
    //this->delayedStateUpdateTimer.setSingleShot(true);
    connect(&(this->gameAddressUpdateTimer), &QTimer::timeout, this, &MemoryScanner::updateGameAddresses);

    this->hero = &this->heroBuffer;
    this->player = &this->playerBuffer;
    this->gameState = &this->gameStateBuffer;
}


bool MemoryScanner::init()
{
    memset(&this->gameStateBuffer, 0, sizeof(GameStateStruct));
    memset(&this->playerBuffer, 0xFF, sizeof(PlayerStruct));
    memset(&this->heroBuffer, 0xFF, sizeof(HeroStructs));

    // Extra permissions is required for this process to be
    // allowed to attach to HoMM HotA process
    bool gotExtraPermission = askForExtraPermissions();
    if (gotExtraPermission == false)
    {
        qWarning() << "Could not get extra permissions.";
    }

    if(this->attachToProcess())
    {
        this->updateGameAddresses();
    }

    this->pollRateTimer.start();
    this->processCheckTimer.start();

    return gotExtraPermission;
}


uintptr_t MemoryScanner::getPointerValue(const uintptr_t address)
{
    uintptr_t pointerValue = 0;
    size_t bytesRead = 0;
    if(!ReadProcessMemory(this->gameStateBuffer.processHandler,
                          reinterpret_cast<LPCVOID>(address),
                          &pointerValue,
                          sizeof(uint32_t),
                          &bytesRead))
    {
        DWORD lastError = GetLastError();
        if(lastError != 299)
            qWarning() << "Could not read pointer at " << address <<  " . Error code: " << lastError;
        return 0;
    }
    return pointerValue;
}


bool MemoryScanner::isGameStillRunning()
{
    DWORD returnCode;
    BOOL success = GetExitCodeProcess(this->gameStateBuffer.processHandler, &returnCode);
    return success && returnCode == STILL_ACTIVE && this->gameStateBuffer.HommPID && this->gameStateBuffer.processHandler;
}

DWORD MemoryScanner::GetHommPid()
{
    if(isGameStillRunning())
    {
        return this->gameStateBuffer.HommPID;
    }

    qDebug() << "Searching for HoMM PID.";

    this->exeName = nullptr;

    DWORD pid = 0;
    PROCESSENTRY32 pe32;
    HANDLE hSnapshot = nullptr;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if(Process32First( hSnapshot, &pe32))
    {
        do
        {
            if(wcscmp( pe32.szExeFile, HOTA_PROCESS_NAME) == 0)
            {
                this->gameStateBuffer.isSoD = false;
                this->exeName = HOTA_PROCESS_NAME;
                pid = pe32.th32ProcessID;
                break;
            }
            else if(wcscmp( pe32.szExeFile, HOTA_HD_PROCESS_NAME) == 0)
            {
                this->gameStateBuffer.isSoD = false;
                this->exeName = HOTA_HD_PROCESS_NAME;
                pid = pe32.th32ProcessID;
                break;
            }
            else if(wcscmp( pe32.szExeFile, HOMM_PROCESS_NAME) == 0)
            {
                this->gameStateBuffer.isSoD = true;
                this->exeName = HOMM_PROCESS_NAME;
                pid = pe32.th32ProcessID;
                break;
            }
            else if(wcscmp( pe32.szExeFile, HOMM_HD_PROCESS_NAME) == 0)
            {
                this->gameStateBuffer.isSoD = true;
                this->exeName = HOMM_HD_PROCESS_NAME;
                pid = pe32.th32ProcessID;
                break;
            }

        } while(Process32Next(hSnapshot, &pe32));
    }

    if(hSnapshot != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hSnapshot);
    }

    return pid;
}


uintptr_t GetModuleBaseAddress(const DWORD dwProcID, const wchar_t * moduleName)
{
    if(dwProcID == 0 || moduleName == nullptr)
    {
        return 0;
    }
    qDebug() << "Getting module offset address.";
    uintptr_t ModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcID);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    MODULEENTRY32 ModuleEntry32;
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
    if (Module32First(hSnapshot, &ModuleEntry32))
    {
        do
        {
            if (wcscmp(ModuleEntry32.szModule, moduleName) == 0)
            {
                ModuleBaseAddress = reinterpret_cast<uintptr_t>(ModuleEntry32.modBaseAddr);
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32));
    }
    CloseHandle(hSnapshot);

    return ModuleBaseAddress;
}


bool MemoryScanner::askForExtraPermissions()
{
    qDebug() << "Asking for elevated permissions";
    bool bRtn = false;
    TOKEN_PRIVILEGES tp;
    HANDLE tokenhandle = nullptr;
    HANDLE ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId());
    if (!ProcessHandle)
    {
        DWORD lastError = GetLastError();
        qWarning() << "Could not open own process to set permissions. Error code: " << lastError;
        return false;
    }

    bRtn = OpenProcessToken(ProcessHandle, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &tokenhandle);
    CloseHandle(ProcessHandle);
    if (bRtn == 0)
    {
        DWORD lastError = GetLastError();
        qWarning() << "Could not open process tokens. Error code: " << lastError;
        return false;
    }

    bRtn = LookupPrivilegeValueW(nullptr, SE_DEBUG_NAME, &(tp.Privileges[0].Luid));
    if (bRtn)
    {
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        tp.PrivilegeCount = 1;
        bRtn = AdjustTokenPrivileges(tokenhandle, false, &tp, sizeof(tp), nullptr, nullptr);
        if(!bRtn)
        {
            DWORD lastError = GetLastError();
            qWarning() << "Could not adjust privilages for this process. Error code: " << lastError;
            return false;
        }
    }
    else
    {
        DWORD lastError = GetLastError();
        qWarning() << "Could not look up privilage for this process. Error code: " << lastError;
        return false;
    }
    return true;
}

void MemoryScanner::clearBuffers()
{
    memset(&this->playerBuffer, 0, sizeof(PlayerStruct));
    memset(&this->heroBuffer, 0, sizeof(HeroStructs));
    memset(&this->gameStateBuffer, 0, sizeof(GameStateStruct));

    // 3 = restore main window.
    this->gameState->status.interactionDisplayID = 3;
    emit playerUpdated();
    emit heroUpdated();

}

bool MemoryScanner::attachToProcess()
{
    if(isGameStillRunning())
    {
        qDebug() << "Process is still active.";
        return true;
    }

    if(this->gameStateBuffer.processHandler)
    {
        qInfo() << "Closing process handle.";
        CloseHandle(this->gameStateBuffer.processHandler);
        clearBuffers();
    }

    this->gameStateBuffer.HommPID = GetHommPid();
    if (!this->gameStateBuffer.HommPID)
    {
        // If no pid was found, clearing buffers and states
        qDebug() << "No HoMM pid...";
        clearBuffers();

        return false;
    }

    if(!this->gameStateBuffer.processHandler)
    {
        qDebug() << "Trying to attatch to HoMM...";
        this->gameStateBuffer.processHandler = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION,
                                                           false,
                                                           this->gameStateBuffer.HommPID);
        if (!this->gameStateBuffer.processHandler)
        {
            DWORD errorCode = GetLastError();
            qWarning() << "Failed to attach to process, error code: " << errorCode;
            return false;
        }
    }

    qInfo() << "Attached to H3 pid:" << this->gameStateBuffer.HommPID;
    qInfo() << "Process name:" << QString::fromWCharArray(this->exeName);

    this->gameAddressUpdateTimer.start();
    return true;
}

void MemoryScanner::updateGameAddresses()
{
    if(this->gameStateBuffer.processHandler == 0)
    {
        publishOffsetStatus(tr("The game is not running."));
        return;
    }
    this->gameStateBuffer.h3BaseAddress = GetModuleBaseAddress(this->gameStateBuffer.HommPID,
                                                               this->exeName);

    this->gameStateBuffer.dllBaseAddress = GetModuleBaseAddress(this->gameStateBuffer.HommPID,
                                                                HOTA_DLL_NAME);

    if(this->gameStateBuffer.h3BaseAddress == 0)
    {
        qWarning() << "Could not get base game address.";
        return;
    }

    // Gets a pointer which points to an unmapped struct, but it contains
    // values which are useful
    this->gameStateBuffer.statusStruktAddress = getPointerValue(this->gameStateBuffer.h3BaseAddress + EXE_TO_STATUS_OFFSET);
    if(this->gameStateBuffer.statusStruktAddress == 0)
    {
        qWarning() << "Could not get Status struct address.";
        return;
    }

    if (this->gameStateBuffer.isSoD)
    {
        // Gets the address of a struct which contains a pointer to the player
        // section in memory, containing ALL players.
        this->gameStateBuffer.initStructAddress = getPointerValue(this->gameStateBuffer.h3BaseAddress + SOD_EXE_TO_INIT_OFFSET);

        // Follows that pointer to the player section.
        uintptr_t addr = getPointerValue(this->gameStateBuffer.initStructAddress);

        // In SoD, after the player section is the Hero section, containing
        // info of all heroes.
        this->gameStateBuffer.heroSectionAddress = addr + SOD_PLAYERS_TO_HEROES_OFFSET;

        // Heroes3.exe is not rebuilt, so nothing has to be detected here.
        publishOffsetStatus(tr("Shadow of Death uses fixed offsets, nothing to detect."));
    }
    else
    {
        // Gets the address of a struct which contains a pointer to the player
        // section and hero section.
        this->gameStateBuffer.initStructAddress = getPointerValue(this->gameStateBuffer.h3BaseAddress + HOTA_EXE_TO_INIT_OFFSET);

        // The offset to the hero section pointer moves with every hota.dll
        // build, so it is detected from the running game. Until a map is loaded
        // there is nothing to confirm a candidate against, which is why this
        // runs on the gameAddressUpdateTimer rather than once on attach.
        const uint32_t heroPointerOffset =
                this->heroPointerLocator.resolve(this->gameStateBuffer.processHandler,
                                                 this->gameStateBuffer.dllBaseAddress,
                                                 HOTA_DLL_TO_HERO_SECTION_POINTER_FALLBACK);

        // Folows the pointer to the hero section
        this->gameStateBuffer.heroSectionAddress = getPointerValue(this->gameStateBuffer.dllBaseAddress +
                                                                   heroPointerOffset);

        publishOffsetStatus(this->heroPointerLocator.isConfirmed()
                            ? tr("Hero data offset 0x%1, auto detected for hota.dll build %2.")
                              .arg(heroPointerOffset, 0, 16)
                              .arg(this->heroPointerLocator.moduleIdentity())
                            : tr("Hero data offset 0x%1, not confirmed yet. Load a map to let it "
                                 "detect the offset for this hota.dll build.")
                              .arg(heroPointerOffset, 0, 16));
    }
}

void MemoryScanner::publishOffsetStatus(const QString &status)
{
    if (status == this->lastOffsetStatus)
    {
        return;
    }
    this->lastOffsetStatus = status;
    emit offsetStatusChanged(status);
}

void MemoryScanner::rescanMemoryOffsets()
{
    qInfo() << "Detecting the hota.dll hero pointer offset again on request.";
    this->heroPointerLocator.forgetCachedOffset();
    publishOffsetStatus(tr("Detecting the hero data offset..."));
    updateGameAddresses();
}

bool MemoryScanner::findActivePlayer()
{
    // Checks for a ponter on a specific offset.
    // It points to the active player's struct in memory.
    // This might break when the game is updated.
    int playerSectionOffset = gameStateBuffer.isSoD ? SOD_INIT_TO_PLAYERS_SECTION_OFFSET : HOTA_INIT_TO_PLAYER_POINTER_OFFSET;
    uintptr_t playerSectionAddress = this->gameStateBuffer.initStructAddress + playerSectionOffset;
    this->playerBuffer.activePlayerAddress = getPointerValue(playerSectionAddress);

    if(!this->playerBuffer.activePlayerAddress)
    {
        return false;
    }
    qDebug() << "Player address: " << POINTER_CASTING(this->playerBuffer.activePlayerAddress);
    return true;
}


bool MemoryScanner::readPlayerInfo()
{
    size_t bytesRead = 0;
    if(!ReadProcessMemory(this->gameStateBuffer.processHandler,
                          POINTER_CASTING(this->playerBuffer.activePlayerAddress),
                          &this->playerBuffer,
                          offsetof(PlayerStruct, isLocal) + sizeof(PlayerStruct::isLocal),
                          &bytesRead))
    {
        DWORD lastError = GetLastError();
        qWarning() << "Could not read Player id. Error code: " << lastError;
        return false;
    }
    return true;
}


bool MemoryScanner::updatePlayer()
{
    if (!this->gameStateBuffer.processHandler)
    {
        return false;
    }

    if(this->gameStateBuffer.heroSectionAddress == 0)
    {
        return false;
    }

    qDebug() << "Hero section address: " << POINTER_CASTING(this->gameStateBuffer.heroSectionAddress);

    if (findActivePlayer() == false)
    {
        return false;
    }

    if (readPlayerInfo() == false)
    {
        attachToProcess();
        return false;
    }

    // In the player section, each player have a flag saying if the player is
    // local and another flag if the player is remote (online play).
    // Checking if the local flag is set.
    if(this->playerBuffer.isLocal == false)
    {
        return false;
    }

    // If no hero is selected, it could mean that a town is selected or the
    // last hero for that player is hidden in a town. We still want to track
    // that hero.
    if(this->playerBuffer.selectedHeroID == NO_HERO_SELECTED)
    {
        this->playerBuffer.selectedHeroID = this->playerBuffer.previousSelectedHeroID;
    }
    this->playerBuffer.previousSelectedHeroID = this->playerBuffer.selectedHeroID;

    // Get the offset from the hero section to the selected hero.
    // Hero ID matches the order they are in memory so we just multiply it with
    // the hero section size to land on the correct address.
    uint32_t offset = (this->gameStateBuffer.heroSectionAddress + sizeof(BaseHeroStruct) * this->playerBuffer.selectedHeroID);
    this->playerBuffer.selectedHeroAddress = POINTER_CASTING(offset);
    qDebug() << "The selected hero is at address " << this->playerBuffer.selectedHeroAddress;

    scanForMapInteractions();
    scanIfOnBattleResault();

    return true;
}


bool MemoryScanner::updateHero()
{
    bool returnValue = false;
    if (!this->gameStateBuffer.processHandler)
    {
        return returnValue;
    }

    if(!this->playerBuffer.isLocal)
    {
        return returnValue;
    }

    if (this->playerBuffer.selectedHeroID != NO_HERO_SELECTED)
    {
        returnValue = copySelectedHeroFromMemory();
    }

    return returnValue;
}


bool MemoryScanner::copySelectedHeroFromMemory()
{
    if (this->gameStateBuffer.processHandler == 0 ||
            this->playerBuffer.selectedHeroAddress == nullptr)
    {
        return false;
    }

    size_t bytesRead = 0;

    if (ReadProcessMemory(this->gameStateBuffer.processHandler,
                          this->playerBuffer.selectedHeroAddress,
                          &this->heroBuffer.base,
                          sizeof(BaseHeroStruct),
                          &bytesRead))
    {
        if(bytesRead != sizeof(BaseHeroStruct))
        {
            qWarning() << "Only read " << bytesRead << "bytes, expected " << sizeof(BaseHeroStruct) << "bytes";
        }

        if(this->heroBuffer.base.color != this->playerBuffer.color)
        {
            return false;
        }
    }
    else
    {
        DWORD lastError = GetLastError();
        qWarning() << "Error trying to scan Hero data. Error code: " << lastError;

        return false;
    }
    if(this->gameStateBuffer.isSoD)
    {
        // Hota reused the skill order location to allow for more skills,
        // "interferenceSkill" (in Hota) is the start (pathfinder) of
        // the SoD skill order location.
        // Copying the SoD skill order data to HoTa location to reuse the same
        // logic to figure out skill order.
        memcpy(&this->heroBuffer.hotaSkillOrder,
               &this->heroBuffer.base.interferenceSkill,
               sizeof(uint8_t) * NUMBER_OF_SKILLS_IN_SOD);
    }
    else
    {
        // In HotA, they added new sections to the hero info but stored in a
        // different location. Have to follow a pointer in the original struct
        // to find the new struct.
        ReadProcessMemory(this->gameStateBuffer.processHandler,
                          POINTER_CASTING(this->heroBuffer.base.hotaHeroStructPointer),
                          &this->heroBuffer.hota,
                          sizeof(HotAHeroStruct),
                          &bytesRead);

        // The new struct have a ponter to yet another struct which contains the
        // secondary skill order (The order the skills were selected).
        ReadProcessMemory(this->gameStateBuffer.processHandler,
                          POINTER_CASTING(this->heroBuffer.hota.hotaSkillOrderPointer),
                          &this->heroBuffer.hotaSkillOrder,
                          sizeof(uint8_t) * NUMBER_OF_SKILLS_IN_HOTA,
                          &bytesRead);
    }
    return true;
}


void MemoryScanner::scanForMapInteractions()
{
    size_t bytesRead;

    if (this->gameStateBuffer.statusStruktAddress == 0)
    {
        return;
    }
    // This contains a byte with the ID of what type of image/animation should
    // appear in the bottom right square, things like sun rise animation or
    // interacting with map object. "Normal" state is value 3, interaction is
    // value 6 and so on.
    // It also contains another slightly unknown value, but it seems to be
    // updated at convinent times. For example, picking up two objects at close
    // to each other will make the above value not change (stay at 6 for both
    // pickups) while this value will change for each pickup.
    // Using it to reset "hide" timer.
    ReadProcessMemory(this->gameStateBuffer.processHandler,
                      POINTER_CASTING(this->gameStateBuffer.statusStruktAddress),
                      &this->gameStateBuffer.status,
                      sizeof(StatusWindowStruct),
                      &bytesRead);
    return;
}


void MemoryScanner::scanIfOnBattleResault()
{
    size_t bytesRead;

    ReadProcessMemory(this->gameStateBuffer.processHandler,
                      POINTER_CASTING(this->gameStateBuffer.h3BaseAddress + EXE_TO_BATTLE_RESULT_OFFSET),
                      &this->gameStateBuffer.isOnBattleResult,
                      sizeof(uint8_t),
                      &bytesRead);
}

void MemoryScanner::updateState()
{
    this->gameStateBuffer.sampleCount++;
    if(updatePlayer())
    {
        emit playerUpdated();
    }
    else
    {
        return;
    }
    if(updateHero())
    {
        emit heroUpdated();
    }
}

