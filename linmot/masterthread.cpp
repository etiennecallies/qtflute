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
    count = 0;
    int i = 57;
    QString a = QString("%1").arg(i, 2, 16, QChar('0'));
    qDebug() << a;
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

        if(this->request == new QString("start")){
             engineOn();
        }
        else if(this->request == new QString("stop")){
            engineOff();
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
        else if(this->request == new QString("read")){
            read();
        }


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

void MasterThread::engineOn(){
    send(MasterThread::switchOffSequence());
    send(MasterThread::switchOnSequence());
}

void MasterThread::engineOff(){
    send(MasterThread::switchOffSequence());
}

void MasterThread::homing(){
    send(MasterThread::homeSequence());
}

void MasterThread::prepare(){
    send(MasterThread::switchOnSequence());
}

void MasterThread::move(){
    send(MasterThread::gotoSequence(500000));
}

void MasterThread::read(){
    send(MasterThread::readSequence(), true);
}

char MasterThread::getCount(){
    if(count == 0){
        count = 1;
        return 0x00;
    }
    else {
        count = 0;
        return 0x01;
    }
}

char* MasterThread::byteDecomposition(int number){
    char* response = new char[4];

    response[0] = (number & 0xff000000L) >> 24;;
    response[1] = (number & 0x00ff0000L) >> 16;
    response[2] = (number & 0x0000ff00L) >> 8;
    response[3] = (number & 0x000000ffL);

    return response;
}

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


void MasterThread::send(QByteArray ba, bool read){
    serial.write(ba);

    QByteArray buffer;
    while(buffer.size() == 0){
        QTest::qWait(10);
        buffer = serial.readAll();
    }

    QByteArray responseData = buffer;

    QString responseString = "";
    if(read){
        std::cout << "responseData.size(): " << responseData.size() << std::endl;
        for(int i = 0; i < responseData.size(); i++){
            int j = (((int)responseData.at(i)) + 256) % 256;
            std::cout << j << std::endl;
            responseString += QString("%1").arg(j, 2, 16, QChar('0')) + QString(" ");
        }
    }
    emit this->response(responseString);
}
