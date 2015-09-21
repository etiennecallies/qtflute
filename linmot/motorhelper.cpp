#include "motorhelper.h"

// classe pour convertir des consignes en unités usuelles en consignes utilisables par le moteur

// entrée = mm
int MotorHelper::pos_to_consigne(int mm_10th)
{
    return mm_10th*1000; // Ce n'est pas des mm mais des dixièmes de mm
}

// entrée = mm/s
int MotorHelper::vel_to_consigne(int mmps)
{
    return mmps*1000;
}

// entrée = mm/s^2
int MotorHelper::accel_to_consigne(int mmps2)
{
    return mmps2*100;
}
