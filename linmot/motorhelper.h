#ifndef MOTORHELPER_H
#define MOTORHELPER_H

class MotorHelper
{
public:
    static int pos_to_consigne(int mm);
    static int vel_to_consigne(int mmps);
    static int accel_to_consigne(int mmps2);

private:
    MotorHelper();
};

#endif // MOTORHELPER_H
