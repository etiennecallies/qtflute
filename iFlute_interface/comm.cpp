/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "comm.h"

#include <QtSerialPort/QSerialPort>

#include <QTime>
#include <QDebug>
#include <QTest>

QT_USE_NAMESPACE

Comm::Comm()
    : home(false), pret(false)
{
    count = rand() % 16;
}

Comm::~Comm()
{
    if(serial[ARDUINO].isWritable())
        this->voltage(ARDUINO_OFF);

    if(serial[MOTOR].isWritable())
        this->engineOff();

    this->deconnect();
}

void Comm::connect(const QString &portName_motor, const QString &portName_arduino)
{
    this->deconnect();

    portName[MOTOR] = portName_motor;
    portName[ARDUINO] = portName_arduino;

    for(int i=0; i<2; i++){
        serial[i].setPortName(portName[i]);

            if (!serial[i].open(QIODevice::ReadWrite)) {
                /*emit error(tr("Can't open %1, error code %2")
                           .arg(portName[i]).arg(serial[i].error()));
                break;*/
                qDebug() << "Erreur connection " << portName[i] << serial[i].error();
            }


            serial[i].setStopBits(QSerialPort::OneStop);
            serial[i].setParity(QSerialPort::NoParity);
            serial[i].setFlowControl(QSerialPort::NoFlowControl);
            serial[i].setDataBits(QSerialPort::Data8);

            if(i == ARDUINO){
                serial[i].setDataTerminalReady(true);
                serial[i].setBaudRate(QSerialPort::Baud115200);
            }
            else { // MOTOR
                serial[i].setBaudRate(QSerialPort::Baud57600);
            }
    }
}

void Comm::deconnect()
{
    pret = false; home = false;

    serial[0].close();
    serial[1].close();
}

#define MOTEUR_TPS_ATTENTE_INIT 200
void Comm::init()
{
    if(serial[MOTOR].isWritable() && serial[ARDUINO].isWritable())
    {
        this->voltage(ARDUINO_OFF);

        this->engineOn();
        QTest::qWait(MOTEUR_TPS_ATTENTE_INIT);
        this->homing();

        pret = true;
    }
}

//COMMANDS

void Comm::engineOn(){
    send(MOTOR, Comm::switchOffSequence());
    send(MOTOR, Comm::switchOnSequence());
}

void Comm::engineOff(){
    pret = false; home = false;
    send(MOTOR, Comm::switchOffSequence());
}

void Comm::homing(){
    send(MOTOR, Comm::homeSequence());
    while(!home){
        Comm::read();
    }
    Comm::prepare();
}

void Comm::prepare(){
    send(MOTOR, Comm::switchOnSequence());
}


// !!!!!!!!!!!!!! BEWARE !!!!!!!!!!!!
// pos = en unités moteur
// autres param = unités définies dans MotorHelper
void Comm::gotoPosition(int pos, int maxvel, int maxacc, int maxdec){

    send(MOTOR, Comm::gotoSequence(
             pos, //MotorHelper::pos_to_consigne(pos),
             MotorHelper::vel_to_consigne(maxvel),
             MotorHelper::accel_to_consigne(maxacc),
             MotorHelper::accel_to_consigne(maxdec))
    );

}

void Comm::read(){
    send(MOTOR, Comm::readSequence(), true);
}

void Comm::voltage(char volt_mode){
    send(ARDUINO, Comm::voltSequence(volt_mode));
}

char Comm::getCount(){
    count += 1;
    if(count == 16){
        count = 0;
    }
    return count;
}

char* Comm::byteDecomposition(int number){
    char* response = new char[4];

    response[0] = (number & 0xff000000L) >> 24;
    response[1] = (number & 0x00ff0000L) >> 16;
    response[2] = (number & 0x0000ff00L) >> 8;
    response[3] = (number & 0x000000ffL);

    return response;
}

//SEQUENCES

QByteArray Comm::switchOffSequence(){
    const char byteArray[] = {0x01, 0x3F, 0x05, 0x02, 0x00, 0x01, 0x3E, 0x00, 0x04};
    return QByteArray(byteArray, sizeof(byteArray));
}

QByteArray Comm::switchOnSequence(){
    const char byteArray[] = {0x01, 0x3F, 0x05, 0x02, 0x00, 0x01, 0x3F, 0x00, 0x04};
    return QByteArray(byteArray, sizeof(byteArray));
}

QByteArray Comm::homeSequence(){
    const char byteArray[] = {0x01, 0x3F, 0x05, 0x02, 0x00, 0x01, 0x3F, 0x08, 0x04};
    return QByteArray(byteArray, sizeof(byteArray));
}

QByteArray Comm::readSequence(){
    const char byteArray[] = {0x01, 0x3F, 0x03, 0x02, 0x01, 0x00, 0x04};
    return QByteArray(byteArray, sizeof(byteArray));
}

QByteArray Comm::voltSequence(char volt){
    const char byteArray[] = {(char) (volt + 48)};
    return QByteArray(byteArray, sizeof(byteArray));
}

/*QByteArray Comm::gotoSequence(int position){
    char* byteDec = byteDecomposition(position);
    const char byteArray[] = {0x01, 0x3F, 0x09, 0x02, 0x00, 0x02,
                              getCount(), 0x02,
                              byteDec[3],
                              byteDec[2],
                              byteDec[1],
                              byteDec[0],
                              0x04};
    return QByteArray(byteArray, sizeof(byteArray));
}*/

QByteArray Comm::gotoSequence(int position, int maxVelocity, int maxAcc, int maxDec){

    // Sécurité
    if(position > MOTEUR_POSITION_MAX) {
        position = MOTEUR_POSITION_MAX;
        qDebug() << "Consigne moteur hors limite !";
    }

    char* byteDec = byteDecomposition(position);
    char* byteDecVel = byteDecomposition(maxVelocity);
    char* byteDecAcc = byteDecomposition(maxAcc);
    char* byteDecDec = byteDecomposition(maxDec);
    const char byteArray[] = {0x01, 0x3F, 0x15, 0x02, 0x00, 0x02,
                              getCount(), 0x01,
                              byteDec[3],
                              byteDec[2],
                              byteDec[1],
                              byteDec[0],
                              byteDecVel[3],
                              byteDecVel[2],
                              byteDecVel[1],
                              byteDecVel[0],
                              byteDecAcc[3],
                              byteDecAcc[2],
                              byteDecAcc[1],
                              byteDecAcc[0],
                              byteDecDec[3],
                              byteDecDec[2],
                              byteDecDec[1],
                              byteDecDec[0],
                              0x04};
    return QByteArray(byteArray, sizeof(byteArray));
}

//READ & WRITE

void Comm::send(int where, QByteArray ba, bool readHome){

    QString requestString = "";
    const int m = ba.size();
    for(int i = 0; i < m; i++){
        int j = (((int)ba.at(i)) + 256) % 256;
        requestString += QString("%1").arg(j, 2, 16, QChar('0')) + QString(" ");
    }
    serial[where].write(ba);

    if(where == ARDUINO)
        serial[where].waitForBytesWritten(DEF_ARDUINO_TIMEOUT); // nécessaire pour bien envoyer les données à Arduino
        //serial[where].flush();

    if(where == MOTOR){
        QByteArray buffer;
        while(buffer.size() == 0){
            QTest::qWait(10);
            buffer = serial[where].readAll();
        }

        QByteArray responseData = buffer;

        QString responseString = "";
        const int n = responseData.size();
        int byteArray [16];
        for(int i = 0; i < n; i++){
            int j = (((int)responseData.at(i)) + 256) % 256;
            if(n == 16){
                byteArray[i] = j;
            }
            responseString += QString("%1").arg(j, 2, 16, QChar('0')) + QString(" ");
        }

        if(readHome){
            if(n == 16)
                home = ((byteArray[8] & 0x08L) == 8) ;
            else
                home = false;
        }
        //qDebug() << responseString;
    }
}
