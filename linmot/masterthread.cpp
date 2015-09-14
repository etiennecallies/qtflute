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

QT_USE_NAMESPACE

MasterThread::MasterThread(QObject *parent)
    : QThread(parent), waitTimeout(0), quit(false)
{
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
void MasterThread::transaction(const QString &portName, int waitTimeout, const QString &request)
{
    //! [1]
    QMutexLocker locker(&mutex);
    this->portName = portName;
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
    bool currentPortNameChanged = false;

    mutex.lock();
    //! [4] //! [5]
    QString currentPortName;
    if (currentPortName != portName) {
        currentPortName = portName;
        currentPortNameChanged = true;
    }

    int currentWaitTimeout = waitTimeout;
    QString currentRequest = request;
    mutex.unlock();
    //! [5] //! [6]
    QSerialPort serial;

    while (!quit) {
        //![6] //! [7]
        if (currentPortNameChanged) {
            serial.close();
            serial.setPortName(currentPortName);

            if (!serial.open(QIODevice::ReadWrite)) {
                emit error(tr("Can't open %1, error code %2")
                           .arg(portName).arg(serial.error()));
                return;
            }

            serial.setStopBits(QSerialPort::OneStop);
            serial.setParity(QSerialPort::NoParity);
            serial.setFlowControl(QSerialPort::NoFlowControl);
            serial.setDataBits(QSerialPort::Data8);
            serial.setBaudRate(QSerialPort::Baud57600);
        }

//        ba.resize(9);
//        ba[0] = 0x01;
//        ba[1] = 0x3F;
//        ba[2] = 0x05;
//        ba[3] = 0x02;
//        ba[4] = 0x00;
//        ba[5] = 0x01;
//        ba[6] = 0x3E;
//        ba[7] = 0x01;
//        ba[8] = 0x04;
//        serial.write(ba);

        if(this->request == new QString("start")){
             engineOn(serial);
        }
        else if(this->request == new QString("stop")){
            engineOff(serial);
        }
        else if(this->request == new QString("home")){
            homing(serial);
        }
//        engineOn(serial);
//        engineOff(serial);

        //! [9]  //! [13]
        mutex.lock();
        cond.wait(&mutex);
        if (currentPortName != portName) {
            currentPortName = portName;
            currentPortNameChanged = true;
        } else {
            currentPortNameChanged = false;
        }
        currentWaitTimeout = waitTimeout;
        currentRequest = request;
        mutex.unlock();
    }
    //! [13]
}

void MasterThread::engineOn(QSerialPort &serial){
    send(serial, MasterThread::onOffSequence());
    send(serial, MasterThread::holdSequence());
}

void MasterThread::engineOff(QSerialPort &serial){
    send(serial, MasterThread::onOffSequence());
}

void MasterThread::homing(QSerialPort &serial){
    send(serial, MasterThread::homeSequence());
}

QByteArray MasterThread::onOffSequence(){
    const char byteArray[] = {0x01, 0x3F, 0x05, 0x02, 0x00, 0x01, 0x3E, 0x00, 0x04};
    return QByteArray(byteArray, sizeof(byteArray));
}

QByteArray MasterThread::homeSequence(){
    const char byteArray[] = {0x01, 0x3F, 0x09, 0x02, 0x00, 0x02, 0x05, 0x02, 0x00, 0x00, 0x00, 0x00, 0x04};
    return QByteArray(byteArray, sizeof(byteArray));
}

QByteArray MasterThread::holdSequence(){
    const char byteArray[] = {0x01, 0x3F, 0x05, 0x02, 0x00, 0x01, 0x3F, 0x00, 0x04};
    return QByteArray(byteArray, sizeof(byteArray));
}

void MasterThread::send(QSerialPort &serial, QByteArray ba){

    serial.write(ba);
    int currentWaitTimeout = waitTimeout;

    if (serial.waitForBytesWritten(waitTimeout)) {
        //! [8] //! [10]
        // read response
        if (serial.waitForReadyRead(currentWaitTimeout)) {
            QByteArray responseData = serial.readAll();
            while (serial.waitForReadyRead(10))
                responseData += serial.readAll();

            QString response(responseData);
            //! [12]
            emit this->response(response);
            //! [10] //! [11] //! [12]
        } else {
            emit timeout(tr("Wait read response timeout %1")
                         .arg(QTime::currentTime().toString()));
        }
        //! [9] //! [11]
    } else {
        emit timeout(tr("Wait write request timeout %1")
                     .arg(QTime::currentTime().toString()));
    }
}
