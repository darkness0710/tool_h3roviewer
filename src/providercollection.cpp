#include "providercollection.h"

ProviderCollection::ProviderCollection()
{
    this->mapProviders();

    this->heroProviders.append(&helmet);
    this->heroProviders.append(&chest);
    this->heroProviders.append(&neck);
    this->heroProviders.append(&weapon);
    this->heroProviders.append(&shield);
    this->heroProviders.append(&boots);
    this->heroProviders.append(&cape);
    this->heroProviders.append(&catapult);
    this->heroProviders.append(&balista);
    this->heroProviders.append(&firstAidTent);
    this->heroProviders.append(&ammoCart);
    this->heroProviders.append(&rightRing);
    this->heroProviders.append(&leftRing);
    this->heroProviders.append(&arti1);
    this->heroProviders.append(&arti2);
    this->heroProviders.append(&arti3);
    this->heroProviders.append(&arti4);
    this->heroProviders.append(&arti5);
    this->heroProviders.append(&spellBook);
    this->heroProviders.append(&heroPotrait);
    this->heroProviders.append(&heroName);
    this->heroProviders.append(&heroMovePoints);
    this->heroProviders.append(&specialty);
    this->heroProviders.append(&skillSlot0);
    this->heroProviders.append(&skillSlot1);
    this->heroProviders.append(&skillSlot2);
    this->heroProviders.append(&skillSlot3);
    this->heroProviders.append(&skillSlot4);
    this->heroProviders.append(&skillSlot5);
    this->heroProviders.append(&skillSlot6);
    this->heroProviders.append(&skillSlot7);
    this->heroProviders.append(&attackSkill);
    this->heroProviders.append(&attackModifiedSkill);
    this->heroProviders.append(&defenceSkill);
    this->heroProviders.append(&defenceModifiedSkill);
    this->heroProviders.append(&powerSkill);
    this->heroProviders.append(&powerModifiedSkill);
    this->heroProviders.append(&knowledgeSkill);
    this->heroProviders.append(&knowledgeModifiedSkill);
    this->heroProviders.append(&heroLevelProgress);
    this->heroProviders.append(&heroLevel);
    this->heroProviders.append(&spellPoints);
    this->heroProviders.append(&creature0Count);
    this->heroProviders.append(&creature1Count);
    this->heroProviders.append(&creature2Count);
    this->heroProviders.append(&creature3Count);
    this->heroProviders.append(&creature4Count);
    this->heroProviders.append(&creature5Count);
    this->heroProviders.append(&creature6Count);
    this->heroProviders.append(&creature0Image);
    this->heroProviders.append(&creature1Image);
    this->heroProviders.append(&creature2Image);
    this->heroProviders.append(&creature3Image);
    this->heroProviders.append(&creature4Image);
    this->heroProviders.append(&creature5Image);
    this->heroProviders.append(&creature6Image);
    this->heroProviders.append(&creature0Loss);
    this->heroProviders.append(&creature1Loss);
    this->heroProviders.append(&creature2Loss);
    this->heroProviders.append(&creature3Loss);
    this->heroProviders.append(&creature4Loss);
    this->heroProviders.append(&creature5Loss);
    this->heroProviders.append(&creature6Loss);
    this->playerProviders.append(&creature0Loss);
    this->playerProviders.append(&creature1Loss);
    this->playerProviders.append(&creature2Loss);
    this->playerProviders.append(&creature3Loss);
    this->playerProviders.append(&creature4Loss);
    this->playerProviders.append(&creature5Loss);
    this->playerProviders.append(&creature6Loss);
    this->playerProviders.append(&playerColor);
    this->playerProviders.append(&status);
    //this->playerProviders.append(&InMatch);
    this->heroProviders.append(&luck);
    this->heroProviders.append(&morale);
    this->heroProviders.append(&stabled);
    this->heroProviders.append(&trailblaze);

}

void ProviderCollection::mapProviders()
{

    this->helmet.init(EquipmentProvider::Equipment,
                      &MemoryScanner::hero->base.helmet,
                      &MemoryScanner::hero->base.helmetBlocked);

    this->neck.init(EquipmentProvider::Equipment,
                    &MemoryScanner::hero->base.neck,
                    &MemoryScanner::hero->base.neckBlocked);

    this->chest.init(EquipmentProvider::Equipment,
                     &MemoryScanner::hero->base.chest,
                     &MemoryScanner::hero->base.chestBlocked);

    this->weapon.init(EquipmentProvider::Equipment,
                      &MemoryScanner::hero->base.weapon,
                      &MemoryScanner::hero->base.weaponBlocked);

    this->shield.init(EquipmentProvider::Equipment,
                      &MemoryScanner::hero->base.shield,
                      &MemoryScanner::hero->base.shieldBlocked);

    this->boots.init(EquipmentProvider::Equipment,
                     &MemoryScanner::hero->base.boots,
                     &MemoryScanner::hero->base.bootsBlocked);

    this->cape.init(EquipmentProvider::Equipment,
                    &MemoryScanner::hero->base.cape,
                    &MemoryScanner::hero->base.capeBlocked);

    this->catapult.init(EquipmentProvider::Equipment,
                        &MemoryScanner::hero->base.catapult,
                        nullptr);

    this->balista.init(EquipmentProvider::Equipment,
                       &MemoryScanner::hero->base.balista,
                       nullptr);

    this->firstAidTent.init(EquipmentProvider::Equipment,
                            &MemoryScanner::hero->base.firstAidTent,
                            nullptr);

    this->ammoCart.init(EquipmentProvider::Equipment,
                        &MemoryScanner::hero->base.ammoCart,
                        nullptr);

    this->spellBook.init(EquipmentProvider::Equipment,
                         &MemoryScanner::hero->base.spellBook,
                         nullptr);

    this->rightRing.init(EquipmentProvider::Equipment,
                         &MemoryScanner::hero->base.ringRight,
                         &MemoryScanner::hero->base.ringBlocked);

    this->leftRing.init(EquipmentProvider::LeftRing,
                         &MemoryScanner::hero->base.ringLeft,
                         &MemoryScanner::hero->base.ringBlocked);

    this->arti1.init(EquipmentProvider::Artifact,
                     &MemoryScanner::hero->base.arti1,
                     &MemoryScanner::hero->base.artiBlocked);

    this->arti2.init(EquipmentProvider::Artifact,
                     &MemoryScanner::hero->base.arti2,
                     &MemoryScanner::hero->base.artiBlocked);

    this->arti3.init(EquipmentProvider::Artifact,
                     &MemoryScanner::hero->base.arti3,
                     &MemoryScanner::hero->base.artiBlocked);

    this->arti4.init(EquipmentProvider::Artifact,
                     &MemoryScanner::hero->base.arti4,
                     &MemoryScanner::hero->base.artiBlocked);

    this->arti5.init(EquipmentProvider::Artifact,
                     &MemoryScanner::hero->base.arti5,
                     &MemoryScanner::hero->base.artiBlocked);

    this->heroPotrait.setAffix(":/images/heroes/Hero_", ".png");

    this->luck.init(LuckMoraleProvider::Luck);
    this->morale.init(LuckMoraleProvider::Morale);

    this->skillSlot0.init(1);
    this->skillSlot1.init(2);
    this->skillSlot2.init(3);
    this->skillSlot3.init(4);
    this->skillSlot4.init(5);
    this->skillSlot5.init(6);
    this->skillSlot6.init(7);
    this->skillSlot7.init(8);


    this->attackSkill.init(&MemoryScanner::hero->base.attackSkill);
    this->attackSkill.setMinMax(0, 99);

    this->attackModifiedSkill.init(&MemoryScanner::hero->base.attackSkill);

    this->defenceSkill.init(&MemoryScanner::hero->base.defenceSkill);
    this->defenceSkill.setMinMax(0, 99);

    this->defenceModifiedSkill.init(&MemoryScanner::hero->base.defenceSkill);

    this->powerSkill.init(&MemoryScanner::hero->base.powerSkill);
    this->powerSkill.setMinMax(1, 99); // You can't have < 1 power

    this->powerModifiedSkill.init(&MemoryScanner::hero->base.powerSkill);

    this->knowledgeSkill.init(&MemoryScanner::hero->base.knowledgeSkill);
    this->knowledgeSkill.setMinMax(1, 99); // You can't have < 1 knowledge

    this->knowledgeModifiedSkill.init(&MemoryScanner::hero->base.knowledgeSkill);

    this->heroLevelProgress.init(&MemoryScanner::hero->base.xp);

    this->heroLevel.init(&MemoryScanner::hero->base.level);
    this->heroLevel.setMinMax(1, 255);
    this->heroLevel.setAffix("Level ","");

    this->heroMovePoints.init(&MemoryScanner::hero->base.movePoints);;

    this->spellPoints.init(&MemoryScanner::hero->base.mana);
    this->creature0Count.init(&MemoryScanner::hero->base.creatureAmountInSlot0);
    this->creature1Count.init(&MemoryScanner::hero->base.creatureAmountInSlot1);
    this->creature2Count.init(&MemoryScanner::hero->base.creatureAmountInSlot2);
    this->creature3Count.init(&MemoryScanner::hero->base.creatureAmountInSlot3);
    this->creature4Count.init(&MemoryScanner::hero->base.creatureAmountInSlot4);
    this->creature5Count.init(&MemoryScanner::hero->base.creatureAmountInSlot5);
    this->creature6Count.init(&MemoryScanner::hero->base.creatureAmountInSlot6);

    this->creature0Loss.init(&MemoryScanner::hero->base.creatureAmountInSlot0);
    this->creature1Loss.init(&MemoryScanner::hero->base.creatureAmountInSlot1);
    this->creature2Loss.init(&MemoryScanner::hero->base.creatureAmountInSlot2);
    this->creature3Loss.init(&MemoryScanner::hero->base.creatureAmountInSlot3);
    this->creature4Loss.init(&MemoryScanner::hero->base.creatureAmountInSlot4);
    this->creature5Loss.init(&MemoryScanner::hero->base.creatureAmountInSlot5);
    this->creature6Loss.init(&MemoryScanner::hero->base.creatureAmountInSlot6);

    this->creature0Image.init(&MemoryScanner::hero->base.creatureIDInSlot0);
    this->creature1Image.init(&MemoryScanner::hero->base.creatureIDInSlot1);
    this->creature2Image.init(&MemoryScanner::hero->base.creatureIDInSlot2);
    this->creature3Image.init(&MemoryScanner::hero->base.creatureIDInSlot3);
    this->creature4Image.init(&MemoryScanner::hero->base.creatureIDInSlot4);
    this->creature5Image.init(&MemoryScanner::hero->base.creatureIDInSlot5);
    this->creature6Image.init(&MemoryScanner::hero->base.creatureIDInSlot6);

    this->playerColor.init(&MemoryScanner::hero->base.color);
    this->stabled.init(BuffProvider::Stabled);
    this->trailblaze.init(BuffProvider::Trailblazer);
}

void ProviderCollection::updateHero()
{
    for(DataProvider* provider : this->heroProviders)
    {
        provider->checkForUpdate();
    }
}

void ProviderCollection::updatePlayer()
{
    for(DataProvider* provider : this->playerProviders)
    {
        provider->checkForUpdate();
    }
}
