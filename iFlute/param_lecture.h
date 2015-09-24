#ifndef PARAM_LECTURE_H
#define PARAM_LECTURE_H

#define ARDUINO_OFF 0
#define ARDUINO_SILENCE 1
#define ARDUINO_GRAVE 6
#define ARDUINO_AIGU 6
#define ARDUINO_ON ARDUINO_AIGU

#include "motorhelper.h"

#define DEF_TPS_BASE 1000
#define DEF_TPS_TRANSI 50
// duree max dépl moteur entre notes (en ms)
#define DEF_TPS_DEPL 50

struct ParamLecture
{
    ParamLecture() :
        intensite(ARDUINO_AIGU),
        maxVit(DEF_MAX_VIT), maxAcc(DEF_MAX_ACC), maxDec(DEF_MAX_ACC),
        tempsBase(DEF_TPS_BASE), tempsTransi(DEF_TPS_TRANSI), tempsMaxDepl(DEF_TPS_DEPL)
    {}

    int intensite; // "voltage" pour Arduino

    int maxVit; // mm/s
    int maxAcc; // mm/s²
    int maxDec;

    // temps de référence (en ms)
    int tempsBase; // temps de la note de durée 1;
    int tempsTransi; // temps transition (notes / souffle)
    int tempsMaxDepl; // temps max de dépl (entre deux notes)
};

#endif // PARAM_LECTURE_H