#ifndef COMM_H
#define COMM_H

#include <QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTest>

// new MHX
#define MOTOR 0
#define ARDUINO 1

#define DEF_PORT_MOTEUR "COM13"
#define DEF_PORT_ARDUINO "COM10"
//#define DEF_PORT_MOTEUR "COM11"
//#define DEF_PORT_ARDUINO "COM6"

#define DEF_ARDUINO_TIMEOUT 100

#include "motorhelper.h"
#include "param_lecture.h"

class Comm
{
public:
    Comm();
    ~Comm();

    void connect(const QString &portName_motor, const QString &portName_arduino);
    void deconnect();
    void init();

    void gotoPosition(int pos, int maxVel = DEF_MAX_VIT, int maxAcc = DEF_MAX_ACC, int maxDec = DEF_MAX_ACC);

    void engineOn();
    void engineOff();
    void homing();
    void prepare();
    void read();
    void voltage(char volt_mode);

    void send(int where, QByteArray ba, bool read = false);
    char getCount();
    char* byteDecomposition(int number);

    QByteArray switchOffSequence();
    QByteArray switchOnSequence();
    QByteArray homeSequence();
    QByteArray readSequence();
    QByteArray voltSequence(char volt);
    QByteArray gotoSequence(int position, int maxVelocity, int maxAcc, int maxDec);

    bool pret; // pret à recevoir des commandes (mis vrai après init du moteur et tout)

protected:
    QString portName[2];
    QSerialPort serial[2];

    bool home;
     int count;
};

#endif // COMM_H
