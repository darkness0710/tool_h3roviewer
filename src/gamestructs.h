#ifndef GAMESTRUCTS_H
#define GAMESTRUCTS_H

#include <stdint.h>
#include <windows.h>


// This 1170 bytes matches game memory
// Using this struct is more or less UB, but seems to work on x86.
#pragma pack(push, 1)
struct BaseHeroStruct
{                                                    // Byte offset:
    uint16_t posX;                                   // 0
    uint16_t posY;                                   // 2
    uint8_t padding1[20];                            // 4
    uint16_t mana;                                   // 24
    uint8_t heroSpecialty;                           // 26
    uint8_t padding2[7];                             // 27
    uint8_t color;                                   // 34
    char heroName[13];                               // 35
    uint8_t padding3[3];                             // 48
    uint8_t heroID;                                  // 51
    uint32_t plannedMoveX;                           // 52
    uint32_t plannedMoveY;                           // 56
    uint8_t padding4[11];                            // 60
    uint8_t orientation;                             // 71
    uint8_t formationAndTactics;                     // 72
    uint32_t maxMovePoints;                          // 73
    uint32_t movePoints;                             // 77
    uint32_t xp;                                     // 81
    uint8_t level;                                   // 85
    uint8_t padding5[57];                            // 86
    uint8_t randomSeed;                              // 143
    uint8_t padding6;                                // 144
    uint32_t creatureIDInSlot0;                      // 145
    uint32_t creatureIDInSlot1;                      // 149
    uint32_t creatureIDInSlot2;                      // 153
    uint32_t creatureIDInSlot3;                      // 157
    uint32_t creatureIDInSlot4;                      // 161
    uint32_t creatureIDInSlot5;                      // 165
    uint32_t creatureIDInSlot6;                      // 169
    int32_t creatureAmountInSlot0;                   // 173
    int32_t creatureAmountInSlot1;                   // 177
    int32_t creatureAmountInSlot2;                   // 181
    int32_t creatureAmountInSlot3;                   // 185
    int32_t creatureAmountInSlot4;                   // 189
    int32_t creatureAmountInSlot5;                   // 193
    int32_t creatureAmountInSlot6;                   // 197
    uint8_t pathfindingSkill;                        // 201
    uint8_t archerySkill;                            // 202
    uint8_t logisticSkill;                           // 203
    uint8_t scoutingSkill;                           // 204
    uint8_t diplomacySkill;                          // 205
    uint8_t navigationSkill;                         // 206
    uint8_t leadershipSkill;                         // 207
    uint8_t wisdomSkill;                             // 208
    uint8_t mysticismSkill;                          // 209
    uint8_t luckSkill;                               // 210
    uint8_t ballisticsSkill;                         // 211
    uint8_t egleEyeSkill;                            // 212
    uint8_t necromancySkill;                         // 213
    uint8_t estateSkill;                             // 214
    uint8_t fireMagicSkill;                          // 215
    uint8_t airMagicSkill;                           // 216
    uint8_t waterMagicSkill;                         // 217
    uint8_t earthMagicSkill;                         // 218
    uint8_t scholarSkill;                            // 219
    uint8_t tacticsSkill;                            // 220
    uint8_t artillerySkill;                          // 221
    uint8_t learningSkill;                           // 222
    uint8_t offenceSkill;                            // 223
    uint8_t armorerSkill;                            // 224
    uint8_t intelligenceSkill;                       // 225
    uint8_t sorcerySkill;                            // 226
    uint8_t resistanceSkill;                         // 227
    uint8_t firstAidSkill;                           // 228
    uint8_t interferenceSkill;                       // 229
    uint8_t runeSkill;                               // 230
    uint8_t logisticSkillOrderSod;                   // 231
    uint8_t scoutingSkillOrderSod;                   // 232
    uint8_t diplomacySkillOrderSod;                  // 233
    uint8_t navigationSkillOrderSod;                 // 234
    uint8_t leadershipSkillOrderSod;                 // 235
    uint8_t wisdomSkillOrderSod;                     // 236
    uint8_t mysticismSkillOrderSod;                  // 237
    uint8_t luckSkillOrderSod;                       // 238
    uint8_t ballisticsSkillOrderSod;                 // 239
    uint8_t egleEyeSkillOrderSod;                    // 240
    uint8_t necromancySkillOrderSod;                 // 241
    uint8_t estateSkillOrderSod;                     // 242
    uint8_t fireMagicSkillOrderSod;                  // 243
    uint8_t airMagicSkillOrderSod;                   // 244
    uint8_t waterMagicSkillOrderSod;                 // 245
    uint8_t earthMagicSkillOrderSod;                 // 246
    uint8_t scholarSkillOrderSod;                    // 247
    uint8_t tacticsSkillOrderSod;                    // 248
    uint8_t artillerySkillOrderSod;                  // 249
    uint8_t learningSkillOrderSod;                   // 250
    uint8_t offenceSkillOrderSod;                    // 251
    uint8_t armorerSkillOrderSod;                    // 252
    uint8_t intelligenceSkillOrderSod;               // 253
    uint8_t sorcerySkillOrderSod;                    // 254
    uint8_t resistanceSkillOrderSod;                 // 255
    uint8_t firstAidSkillOrderSod;                   // 256
    uint32_t nrOfSkillsOnHero;                       // 257
    uint16_t buffType;                               // 261
    uint8_t padding7[6];                             // 263
    uint8_t CastedNrOfDD;                            // 269
    uint32_t castedDisguiseLevel;                    // 270
    uint32_t castedFlyLevel;                         // 274
    uint32_t castedWaterWalkLevel;                   // 278
    int8_t temporaryMorale;                          // 282
    int8_t temporaryLuck;                            // 283
    uint8_t sleeping;                                // 284
    uint8_t padding8[12];                            // 285
    uint32_t castedVisionLevel;                      // 301
    uint32_t helmet;                                 // 301
    uint32_t helmetType;                             // 305
    uint32_t cape;                                   // 309
    uint32_t capeType;                               // 313
    uint32_t neck;                                   // 317
    uint32_t neckType;                               // 321
    uint32_t weapon;                                 // 325
    uint32_t weaponType;                             // 329
    uint32_t shield;                                 // 333
    uint32_t shieldType;                             // 337
    uint32_t chest;                                  // 341
    uint32_t chestType;                              // 345
    uint32_t ringLeft;                               // 349
    uint32_t ringLeftType;                           // 353
    uint32_t ringRight;                              // 357
    uint32_t ringRightType;                          // 361
    uint32_t boots;                                  // 365
    uint32_t bootsType;                              // 369
    uint32_t arti1;                                  // 373
    uint32_t arti1Type;                              // 377
    uint32_t arti2;                                  // 381
    uint32_t arti2Type;                              // 385
    uint32_t arti3;                                  // 389
    uint32_t arti3Type;                              // 393
    uint32_t arti4;                                  // 397
    uint32_t arti4Type;                              // 401
    uint32_t balista;                                // 405
    uint32_t balistaType;                            // 409
    uint32_t ammoCart;                               // 413
    uint32_t ammoCartType;                           // 417
    uint32_t firstAidTent;                           // 421
    uint32_t firstAidTentType;                       // 425
    uint32_t catapult;                               // 429
    uint32_t catapultType;                           // 433
    uint32_t spellBook;                              // 437
    uint32_t spellBookType;                          // 441
    uint32_t arti5;                                  // 445
    uint32_t arti5Type;                              // 451
    uint8_t padding9;                                // 453
    uint8_t helmetBlocked;                           // 454
    uint8_t capeBlocked;                             // 455
    uint8_t neckBlocked;                             // 456
    uint8_t weaponBlocked;                           // 457
    uint8_t shieldBlocked;                           // 458
    uint8_t chestBlocked;                            // 459
    uint8_t ringBlocked;                             // 460
    uint8_t bootsBlocked;                            // 461
    uint8_t artiBlocked;                             // 462
    uint8_t padding10[5];                            // 463
    uint64_t backpack[64];                           // 468
    uint8_t padding11[22];                           // 980
    uint8_t learnedSpells[70];                       // 1002
    uint8_t castableSpells[70];                      // 1072
    uint8_t attackSkill;                             // 1142
    uint8_t defenceSkill;                            // 1143
    uint8_t powerSkill;                              // 1144
    uint8_t knowledgeSkill;                          // 1145
    uint32_t hotaHeroStructPointer;                  // 1146
    uint8_t padding12[20];                           // 1150
};
#pragma pack(pop)

// In HOTA, they added hero data in other data regons to fit more functions
// into the game. This is is pointed to via the "hotaHeroStructPointer"
#pragma pack(push, 1)
struct HotAHeroStruct
{                                                     // Byte offset:
    uint8_t padding[4];                               // 0
    uint8_t buffType;                                 // 4
    uint8_t padding2[3];                              // 5
    uint32_t hotaSkillOrderPointer;                   // 8
    uint8_t padding3[140];                            // 12
};
#pragma pack(pop)

#pragma pack(push, 1)
struct HeroSkillOrderStruct
{
    uint8_t pathfindingOrderSkill;
    uint8_t archeryOrderSkill;
    uint8_t logisticOrderSkill;
    uint8_t scoutingOrderSkill;
    uint8_t diplomacyOrderSkill;
    uint8_t navigationOrderSkill;
    uint8_t leadershipOrderSkill;
    uint8_t wisdomOrderSkill;
    uint8_t mysticismOrderSkill;
    uint8_t luckOrderSkill;
    uint8_t ballisticsOrderSkill;
    uint8_t egleEyeOrderSkill;
    uint8_t necromancyOrderSkill;
    uint8_t estateOrderSkill;
    uint8_t fireMagicOrderSkill;
    uint8_t airMagicOrderSkill;
    uint8_t waterMagicOrderSkill;
    uint8_t earthMagicOrderSkill;
    uint8_t scholarOrderSkill;
    uint8_t tacticsOrderSkill;
    uint8_t artilleryOrderSkill;
    uint8_t learningOrderSkill;
    uint8_t offenceOrderSkill;
    uint8_t armorerOrderSkill;
    uint8_t intelligenceOrderSkill;
    uint8_t sorceryOrderSkill;
    uint8_t resistanceOrderSkill;
    uint8_t firstAidOrderSkill;
    uint8_t interferenceOrderSkill;
    uint8_t runeOrderSkill;
};
#pragma pack(pop)

typedef struct HeroStructs
{
    BaseHeroStruct base;
    HotAHeroStruct hota;
    HeroSkillOrderStruct hotaSkillOrder;

} HeroStructs;


#pragma pack(push, 1)
struct StatusWindowStruct
{
    uint8_t padding1[0x4];
    uint32_t isInBattlePointer;
    uint32_t isInMatchPointer;
    uint8_t padding2[0x388];
    uint8_t interactionDisplayID;
    uint8_t padding3[0x7];
    uint16_t interactionID;
};
#pragma pack(pop)

typedef struct
{
    // Aligned with game memory
    StatusWindowStruct status;

    // Not aligned with game memory
    DWORD HommPID;
    HANDLE processHandler;
    uint32_t dllBaseAddress;
    uint32_t h3BaseAddress;
    uint32_t initStructAddress;
    uint32_t heroSectionAddress;
    uint32_t statusStruktAddress;
    bool isSoD;
    uint64_t sampleCount;
    uint8_t isOnBattleResult;
} GameStateStruct;


#pragma pack(push, 1)
struct PlayerStruct
{
    // Aligned with game memory
    uint8_t color;
    uint8_t padding[3];
    uint32_t selectedHeroID;
    uint32_t startHeroID;
    uint8_t padding2[144];
    uint32_t wood;
    uint32_t mercury;
    uint32_t ore;
    uint32_t sulfur;
    uint32_t crystal;
    uint32_t gems;
    uint32_t gold;
    uint8_t padding3[41];
    uint8_t isLocal;

    // Not aligned with game memory
    uint32_t activePlayerAddress;
    LPCVOID selectedHeroAddress;
    uint32_t previousSelectedHeroID;
};
#pragma pack(pop)

#endif // GAMESTRUCTS_H
