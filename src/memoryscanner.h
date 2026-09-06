#ifndef MEMORYSCANNER_H
#define MEMORYSCANNER_H

#include <windows.h>
#include <tlhelp32.h>

#include <QObject>
#include <QTimer>

#include "gamestructs.h"

class MemoryScanner : public QObject
{
    Q_OBJECT

public:
    MemoryScanner();
    /**
     * @brief init .
     * @return true if extra permissions were granted by Windows
     */
    bool init();

    static HeroStructs *hero;
    static PlayerStruct *player;
    static GameStateStruct *gameState;


private slots:
    /**
     * @brief updateState Called cyclically based on the processCheckTimer.
     * It will update this applications information about the game and trigger
     * a signal if any new data is available.
     */
    void updateState();

    /**
     * @brief updateGameAddresses Fetches the memory addressess from the game.
     * This should be called each time the game is started as the addresses
     * has changed.
     */
    void updateGameAddresses();
private:
    /**
     * @brief getPointerValue Reads a memory location in the HoMMm and assumes
     * it is a 32 bit pointer and returns the value of what it is pointing to.
     * @param address The address to read the pointer value from.
     * @return The value of what the pointer is pointing to.
     */
    uintptr_t getPointerValue(const uintptr_t address);

    /**
     * @brief GetHommPid Searches trough all processes on the computer to find
     * the pid of h3hota.exe, h3hota HD.exe, Heroes3.exe or Heroes3 HD.exe.
     * If the game is already tracked, it will just return the known pid.
     * @return The pid of the HoMM3 PID.
     */
    DWORD GetHommPid();

    /**
     * @brief askForExtraPermissions Asks Windows for extra permissions. Required
     * to be allowed to attach to HoMM HotA.
     * @return True if successful, otherwise returns false.
     */
    bool askForExtraPermissions();

    /**
     * @brief attachToProcess Tries to attach to the the HoMM process.
     * @return True if successful or already attached, otherwise returns false.
     */
    bool attachToProcess();

    /**
     * @brief updatePlayer Reads the memory related to what the player is doing,
     * like which player color is currently playing or which hero is selected.
     * @return True if successful, otherwise returns false.
     */
    bool updatePlayer();

    /**
     * @brief updateHero Reads the memory data from the currently selected hero.
     * @return True if successful, otherwise returns false.
     */
    bool updateHero();    

    /**
     * @brief copySelectedHeroFromMemory Copies data from the HoMM process
     * to the heroBuffer.
     * @return True if successful, otherwise returns false.
     */
    bool copySelectedHeroFromMemory();

    /**
     * @brief scanForMapInteractions Check if the bottom right corner chages
     * to a pickup/interaction view, in that case this triggers a signal
     */
    void scanForMapInteractions();

    /**
     * @brief scanIfOnBattleResault Checks for the battle result popup from
     * the game memory.
     */
    void scanIfOnBattleResault();

    /**
     * @brief isGameStillRunning Checks if the game we might have attached to
     * is still running.
     * @return true if we are attached to the game and it is still running.
     */
    bool isGameStillRunning();

    /**
     * @brief findActivePlayer Follows a pointer to the current player
     * memory struct.
     * @return true if the pointer points to a non null value.
     */
    bool findActivePlayer();

    /**
     * @brief readPlayerInfo Reads about the current player from the game memory
     * @return true if the read was successful.
     */
    bool readPlayerInfo();

    /**
     * @brief clearBuffers sets all internal buffers to 0 and emitts
     * playerUpdated() and heroUpdated();
     */
    void clearBuffers();

    /** * * * * * * * * * *
     *  Member variables  *
     ** * * * * * * * * * */

    QTimer pollRateTimer;
    QTimer processCheckTimer;
    QTimer gameAddressUpdateTimer;

    GameStateStruct gameStateBuffer;
    HeroStructs heroBuffer;
    PlayerStruct playerBuffer;

    wchar_t const *exeName;
signals:
    void playerUpdated();
    void heroUpdated();
};

#endif // MEMORYSCANNER_H
