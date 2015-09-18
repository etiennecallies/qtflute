#include "motorhelper.h"

// classe pour convertir des consignes en unités usuelles en consignes utilisables par le moteur

// entrée = mm
int MotorHelper::pos_to_consigne(int mm)
{
    return mm*10000;
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
