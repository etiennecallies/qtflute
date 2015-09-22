#ifndef PARAM_LECTURE_H
#define PARAM_LECTURE_H

#define ARDUINO_OFF 0
#define ARDUINO_SILENCE 1
#define ARDUINO_GRAVE 2
#define ARDUINO_AIGU 2

struct ParamLecture
{
    ParamLecture() :
        intensite(ARDUINO_AIGU), maxVit(20), maxAcc(200), maxDec(200) {}

    int intensite; // "voltage" pour Arduino

    // transitions (notes / souffle)

    int maxVit; // mm/s
    int maxAcc; // mm/s²
    int maxDec;
};

#endif // PARAM_LECTURE_H
