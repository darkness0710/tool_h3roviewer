#ifndef PROVIDERCOLLECTION_H
#define PROVIDERCOLLECTION_H

#include <QObject>

#include "imagesMap.h"
#include "memoryscanner.h"

#include "data_providers/dataprovider.h"

#include "data_providers/battlelossprovider.h"
#include "data_providers/buffprovider.h"
#include "data_providers/creaturecountprovider.h"
#include "data_providers/creaturetypeprovider.h"
#include "data_providers/deltastatsprovider.h"
#include "data_providers/equipmentprovider.h"
#include "data_providers/heronameprovider.h"
//#include "data_providers/isinmatchprovider.h"
#include "data_providers/levelprogressprovider.h"
#include "data_providers/luckmoraleprovider.h"
#include "data_providers/manaprovider.h"
#include "data_providers/playercolorprovider.h"
#include "data_providers/primaryskillprovider.h"
#include "data_providers/skillprovider.h"
#include "data_providers/specialtyprovider.h"
#include "data_providers/statusprovider.h"
#include "data_providers/movementProvider.h"


class ProviderCollection : public QObject
{
    Q_OBJECT
public:
    ProviderCollection();

    void mapProviders();

    void updateHero();
    void updatePlayer();

    EquipmentProvider helmet;
    EquipmentProvider neck;
    EquipmentProvider chest;
    EquipmentProvider weapon;
    EquipmentProvider shield;
    EquipmentProvider cape;
    EquipmentProvider boots;
    EquipmentProvider catapult;
    EquipmentProvider firstAidTent;
    EquipmentProvider ammoCart;
    EquipmentProvider balista;
    EquipmentProvider rightRing;
    EquipmentProvider leftRing;
    EquipmentProvider arti1;
    EquipmentProvider arti2;
    EquipmentProvider arti3;
    EquipmentProvider arti4;
    EquipmentProvider arti5;
    EquipmentProvider spellBook;

    CreatureTypeProvider creature0Image;
    CreatureTypeProvider creature1Image;
    CreatureTypeProvider creature2Image;
    CreatureTypeProvider creature3Image;
    CreatureTypeProvider creature4Image;
    CreatureTypeProvider creature5Image;
    CreatureTypeProvider creature6Image;

    CreatureCountProvider creature0Count;
    CreatureCountProvider creature1Count;
    CreatureCountProvider creature2Count;
    CreatureCountProvider creature3Count;
    CreatureCountProvider creature4Count;
    CreatureCountProvider creature5Count;
    CreatureCountProvider creature6Count;

    BattleLossProvider creature0Loss;
    BattleLossProvider creature1Loss;
    BattleLossProvider creature2Loss;
    BattleLossProvider creature3Loss;
    BattleLossProvider creature4Loss;
    BattleLossProvider creature5Loss;
    BattleLossProvider creature6Loss;

    HeroNameProvider heroPotrait;
    HeroNameProvider heroName;
    MovementProvider heroMovePoints;
    SpecialtyProvider specialty;
    LevelProgressProvider heroLevelProgress;
    PrimarySkillProvider heroLevel;
    ManaProvider spellPoints;
    LuckMoraleProvider luck;
    LuckMoraleProvider morale;

    PrimarySkillProvider attackSkill;
    DeltaStatsProvider attackModifiedSkill;
    PrimarySkillProvider defenceSkill;
    DeltaStatsProvider defenceModifiedSkill;
    PrimarySkillProvider powerSkill;
    DeltaStatsProvider powerModifiedSkill;
    PrimarySkillProvider knowledgeSkill;
    DeltaStatsProvider knowledgeModifiedSkill;

    SkillProvider skillSlot0;
    SkillProvider skillSlot1;
    SkillProvider skillSlot2;
    SkillProvider skillSlot3;
    SkillProvider skillSlot4;
    SkillProvider skillSlot5;
    SkillProvider skillSlot6;
    SkillProvider skillSlot7;

    playerColorProvider playerColor;
    StatusProvider status;
    BuffProvider stabled;
    BuffProvider trailblaze;
    //IsInMatchProvider InMatch;

private:

    QList<DataProvider*> heroProviders;
    QList<DataProvider*> playerProviders;

};

#endif // PROVIDERCOLLECTION_H
