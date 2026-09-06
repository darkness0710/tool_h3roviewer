#include <array>

#include "levelprogressprovider.h"
#include "../memoryscanner.h"

const std::array <uint32_t, 75>xpReqForLevel = {0,
                                                1000,
                                                2000,
                                                3200,
                                                4600,
                                                6200,
                                                8000,
                                                10000,
                                                12200,
                                                14700,
                                                17500,
                                                20600,
                                                24320,
                                                28784,
                                                34140,
                                                40567,
                                                48279,
                                                57533,
                                                68637,
                                                81961,
                                                97949,
                                                117134,
                                                140156,
                                                167782,
                                                200933,
                                                240714,
                                                288451,
                                                345735,
                                                414475,
                                                496963,
                                                595948,
                                                714730,
                                                857268,
                                                1028313,
                                                1233567,
                                                1479871,
                                                1775435,
                                                2130111,
                                                2555722,
                                                3066455,
                                                3679334,
                                                4414788,
                                                5297332,
                                                6356384,
                                                7627246,
                                                9152280,
                                                10982320,
                                                13178368,
                                                15813625,
                                                18975933,
                                                22770702,
                                                27324424,
                                                32788890,
                                                39346249,
                                                47215079,
                                                56657675,
                                                67988790,
                                                81586128,
                                                97902933,
                                                117483099,
                                                140979298,
                                                169174736,
                                                203009261,
                                                243610691,
                                                292332407,
                                                350798466,
                                                420957736,
                                                505148860,
                                                606178208,
                                                727413425,
                                                872895685,
                                                1047474397,
                                                1256968851,
                                                1508362195,
                                                1810034207};

LevelProgressProvider::LevelProgressProvider(QObject *parent)
    : DataProvider{parent},
      currentXP(nullptr),
      previousXP(0)
{

}

void LevelProgressProvider::init(uint32_t *xpAddress)
{
    this->currentXP = xpAddress;
}

void LevelProgressProvider::checkForUpdate()
{
    if(*this->currentXP == this->previousXP)
    {
        return;
    }

    size_t level = 0;
    for (;level < xpReqForLevel.size() - 1; level++)
    {
        if(xpReqForLevel[level] > *this->currentXP)
        {
            level--;
            break;
        }
    }

    // * 100 to turn it into integer percentage.
    uint32_t percentage = ((*this->currentXP - xpReqForLevel[level]) * 100) /
            (xpReqForLevel[level + 1] - xpReqForLevel[level]);

    emit updated(QString::number(percentage));
    previousXP = *this->currentXP;

}
