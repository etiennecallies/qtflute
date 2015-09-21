#ifndef PARAM_LECTURE_H
#define PARAM_LECTURE_H

#define ARDUINO_SILENCE 1
#define ARDUINO_FORT 2

struct ParamLecture
{
    ParamLecture() :
        intensite(ARDUINO_FORT), maxVit(20), maxAcc(200), maxDec(200) {}

    int intensite; // "voltage" pour Arduino

    // transitions (notes / souffle)

    int maxVit; // mm/s
    int maxAcc; // mm/s²
    int maxDec;
};

#endif // PARAM_LECTURE_H
