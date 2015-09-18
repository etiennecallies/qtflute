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

#include "masterthread.h"

#include <QtSerialPort/QSerialPort>

#include <QTime>
#include <iostream>
#include <QDebug>
#include <QTest>

QT_USE_NAMESPACE

MasterThread::MasterThread(QObject *parent)
    : QThread(parent), waitTimeout(0), quit(false)
{
    count = rand() % 16;
    home = false;
}

//! [0]
MasterThread::~MasterThread()
{
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}
//! [0]

//! [1] //! [2]
void MasterThread::transaction(const QString &portName_motor, const QString &portName_arduino, int waitTimeout, const QString &request)
{
    //! [1]
    QMutexLocker locker(&mutex);
    this->portName[MOTOR] = portName_motor;
    this->portName[ARDUINO] = portName_arduino;

    this->waitTimeout = waitTimeout;
    this->request = request;

    //! [3]
    if (!isRunning())
        start();
    else
        cond.wakeOne();
}
//! [2] //! [3]

//! [4]
void MasterThread::run()
{
    int i=0;
    bool currentPortNameChanged[] = {false, false};
    QString currentPortName[2];

    mutex.lock();

    for(i=0; i<2; i++)
    {
        if (currentPortName[i] != portName[i]) {
            currentPortName[i] = portName[i];
            currentPortNameChanged[i] = true;
        }
    }

    int currentWaitTimeout = waitTimeout;
    QString currentRequest = request;

    mutex.unlock();

    while (!quit) {
        //![6] //! [7]
        for(i=0; i<2; i++){
            if (currentPortNameChanged[i]) {
                serial[i].close();
                serial[i].setPortName(currentPortName[i]);

                if (!serial[i].open(QIODevice::ReadWrite)) {
                    emit error(tr("Can't open %1, error code %2")
                               .arg(portName[i]).arg(serial[i].error()));
                    break;
                }

                serial[i].setStopBits(QSerialPort::OneStop);
                serial[i].setParity(QSerialPort::NoParity);
                serial[i].setFlowControl(QSerialPort::NoFlowControl);
                serial[i].setDataBits(QSerialPort::Data8);

                if(i == ARDUINO){
                    serial[i].setDataTerminalReady(true);
                    serial[i].setBaudRate(QSerialPort::Baud9600);
                }
                else { // MOTOR
                    serial[i].setBaudRate(QSerialPort::Baud57600);
                }
            }
        }

        if(this->request == new QString("start")){

             engineOn();
        }
        else if(this->request == new QString("stop")){
            engineOff();
            voltage(0);
        }
        else if(this->request == new QString("home")){
            homing();
        }
        else if(this->request == new QString("prepare")){
            prepare();
        }
        else if(this->request == new QString("move")){
            move();
        }
        else if(this->request == new QString("slow")){
            slow();
        }
        // new MHX
        else if(this->request == new QString("combine")){
            combine(2);
        }
        else if(this->request == new QString("read")){
            read();
        }
        else if(this->request == new QString("gohome")){
            goHome();
        }
        else if(this->request == new QString("volt")){
            voltage(1);
        }
        else if(this->request == new QString("volt0")){
            voltage(0);
        }

        //! [9]  //! [13]
        mutex.lock();
        cond.wait(&mutex);

        for(i=0; i<2; i++)
        {
            if (currentPortName[i] != portName[i]) {
                currentPortName[i] = portName[i];
                currentPortNameChanged[i] = true;
            } else {
                currentPortNameChanged[i] = false;
            }
        }

        currentWaitTimeout = waitTimeout;
        currentRequest = request;

        mutex.unlock();
    }
    //! [13]
}

//COMMANDS

void MasterThread::engineOn(){
    send(MOTOR, MasterThread::switchOffSequence());
    send(MOTOR, MasterThread::switchOnSequence());
}

void MasterThread::engineOff(){
    send(MOTOR, MasterThread::switchOffSequence());
}

void MasterThread::homing(){
    emit this->disable();
    send(MOTOR, MasterThread::homeSequence());
    while(!home){
        MasterThread::read();
    }
    MasterThread::prepare();
    emit this->enable();
}

void MasterThread::prepare(){
    send(MOTOR, MasterThread::switchOnSequence());
}

void MasterThread::move(){
    send(MOTOR, MasterThread::gotoSequence(500000));
    QTest::qWait(500);
    send(MOTOR, MasterThread::gotoSequence(50000));
    QTest::qWait(500);
    send(MOTOR, MasterThread::gotoSequence(0));
    QTest::qWait(500);
    send(MOTOR, MasterThread::gotoSequence(100000));
}

void MasterThread::combine(int volt){
    voltage(volt);
    slow();
}

// regular units BEWARE!!!!
void MasterThread::gotoPosition(int pos, int maxvel, int maxacc, int maxdec){
    //emit this->disable();
    send(MOTOR, MasterThread::gotoSlowSequence(
             MotorHelper::pos_to_consigne(pos),
             MotorHelper::vel_to_consigne(maxvel),
             MotorHelper::accel_to_consigne(maxacc),
             MotorHelper::accel_to_consigne(maxdec))
    );
    //emit this->enable();
}

void MasterThread::slow(){
    emit this->disable();
    send(MOTOR, MasterThread::gotoSlowSequence(500000, 20000, 20000, 20000));
    emit this->enable();
}

void MasterThread::goHome(){
    send(MOTOR, MasterThread::gotoSequence(0));
}

void MasterThread::read(){
    send(MOTOR, MasterThread::readSequence(), true);
}

void MasterThread::voltage(int volt){
    send(ARDUINO, MasterThread::voltSequence(volt));
}

char MasterThread::getCount(){
    count += 1;
    if(count == 16){
        count = 0;
    }
    return count;
}

char* MasterThread::byteDecomposition(int number){
    char* response = new char[4];

    response[0] = (number & 0xff000000L) >> 24;
    response[1] = (number & 0x00ff0000L) >> 16;
    response[2] = (number & 0x0000ff00L) >> 8;
    response[3] = (number & 0x000000ffL);

    return response;
}

//SEQUENCES

QByteArray MasterThread::switchOffSequence(){
    const char byteArray[] = {0x01, 0x3F, 0x05, 0x02, 0x00, 0x01, 0x3E, 0x00, 0x04};
    return QByteArray(byteArray, sizeof(byteArray));
}

QByteArray MasterThread::switchOnSequence(){
    const char byteArray[] = {0x01, 0x3F, 0x05, 0x02, 0x00, 0x01, 0x3F, 0x00, 0x04};
    return QByteArray(byteArray, sizeof(byteArray));
}

QByteArray MasterThread::homeSequence(){
    const char byteArray[] = {0x01, 0x3F, 0x05, 0x02, 0x00, 0x01, 0x3F, 0x08, 0x04};
    return QByteArray(byteArray, sizeof(byteArray));
}

QByteArray MasterThread::readSequence(){
    const char byteArray[] = {0x01, 0x3F, 0x03, 0x02, 0x01, 0x00, 0x04};
    return QByteArray(byteArray, sizeof(byteArray));
}

QByteArray MasterThread::voltSequence(int volt){
    const char byteArray[] = {volt + 48};
    return QByteArray(byteArray, sizeof(byteArray));
}


QByteArray MasterThread::gotoSequence(int position){
    char* byteDec = byteDecomposition(position);
    const char byteArray[] = {0x01, 0x3F, 0x09, 0x02, 0x00, 0x02,
                              getCount(), 0x02,
                              byteDec[3],
                              byteDec[2],
                              byteDec[1],
                              byteDec[0],
                              0x04};
    return QByteArray(byteArray, sizeof(byteArray));
}

QByteArray MasterThread::gotoSlowSequence(int position, int maxVelocity, int maxAcc, int maxDec){
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

void MasterThread::send(int where, QByteArray ba, bool readHome){

    QString requestString = "";
    const int m = ba.size();
    for(int i = 0; i < m; i++){
        int j = (((int)ba.at(i)) + 256) % 256;
        requestString += QString("%1").arg(j, 2, 16, QChar('0')) + QString(" ");
    }
    serial[where].write(ba);
    //serial[where].flush();
    if(where == ARDUINO)
        serial[where].waitForBytesWritten(waitTimeout); // nécessaire pour bien envoyer les données à Arduino

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
            if(n == 16){
                // std::cout << byteArray[8] << std::endl;
                home = ((byteArray[8] & 0x08L) == 8) ;
                // std::cout << home << std::endl;
            }
            else {
                home = false;
            }
        }
        qDebug() << responseString;
        emit this->response(requestString, responseString);
    }
}
