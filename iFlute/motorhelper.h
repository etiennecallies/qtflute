#ifndef MOTORHELPER_H
#define MOTORHELPER_H

// position max à ne pas dépasser (unités moteur), la position min étant 0.
    // CRITICAL BEWARE PAS TOUCHER !!!!!!!!!!!!!!!!!!!!!!!!!
    #define LONG_TOTALE 150
    #define MOTEUR_POSITION_mm 10000
    #define MOTEUR_POSITION_MAX LONG_TOTALE*MOTEUR_POSITION_mm
    // CRITICAL BEWARE PAS TOUCHER !!!!!!!!!!!!!!!!!!!!!!!!!

    #define DEF_MAX_VIT 30
    #define DEF_MAX_ACC 300

//

// classe pour convertir des consignes en unités usuelles en consignes utilisables par le moteur
class MotorHelper
{
public:
    // entrée = mm
    //inline static int pos_to_consigne(double mm){return (int) mm*10000;} // TODO: check!!!

    // entrée = mm/s
    inline static int vel_to_consigne(double mmps){return (int) mmps*1000;}

    // // entrée = mm/s²
    inline static int accel_to_consigne(double mmps2){return (int) mmps2*100;}

private:
    MotorHelper();
};

#endif // MOTORHELPER_H
