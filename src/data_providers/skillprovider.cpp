#include "skillprovider.h"

#include "../memoryscanner.h"
#include "../gamestructs.h"
#include "../imagesMap.h"

SkillProvider::SkillProvider(QObject *parent)
    : DataProvider{parent},
      skillSlot(0),
      skills(nullptr),
      previousSkill(new uint8_t[skillList.size()]),
      skillOrder(nullptr)
{
    memset(previousSkill.get(), 0, skillList.size());
}


void SkillProvider::init(uint8_t skillSlot)
{
    this->skills = &MemoryScanner::hero->base.pathfindingSkill;
    this->skillOrder = reinterpret_cast<uint8_t*>(&MemoryScanner::hero->hotaSkillOrder);

    this->skillSlot = skillSlot;
}

QString valueToSkillLevel(const uint8_t &value)
{
    switch(value)
    {
    case 1:
        return QString("Basic");
        break;
    case 2:
        return QString("Advanced");
        break;
    case 3:
        return QString("Expert");
        break;
    }
    return QString("");
}

void SkillProvider::checkForUpdate()
{

    if(memcmp(skills, previousSkill.get(), skillList.size()) == 0)
    {
        return;
    }

    bool foundMatch = false;

    // Looping over the skillOrder struct of the game memory.
    // It can look like this: 0 0 0 0 3 0 1 0 2 0 0 ...
    // The position indicate which skill and the number indicates which order
    // the skill was selected.
    for(size_t i = 0; i < skillList.size(); i++)
    {
        if (*(skillOrder + i) == this->skillSlot)
        {
            QString skillLevel = valueToSkillLevel(*(skills + i));
            emit updated(":/images/skills/Skill_" + skillLevel + "_" + skillList.at(i));
            foundMatch = true;
            break;
        }
    }
    if(!foundMatch)
    {
        emit updated("");
    }
    memcpy(previousSkill.get(), skills, skillList.size());
}
