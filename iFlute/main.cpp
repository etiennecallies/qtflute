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

#include <QApplication>

#include "dialog.h"

#include "Accordeur/accordage.h"
#include "Accordeur/micro.h"
#include "partition.h"
#include "param_lecture.h"

#include "comm.h"

// debug
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
/* ACCORDAGE MANUEL Etienne C. et Pierre
    //0: F
    //1: F# - Gb
    //2: G
    //3: G# - Ab
    //4: A
    //5: A# - Bb
    //6: B
    //7: c
    //8: c# - db
    //9: d
    //10: d# - eb
    //11: e
    //12: f
    //13: f# - gb
    //14: g
    //15: g#
    cm[0] = 1590; //Il s'agit de dixièmes de mm //F
    cm[1] = 1450; //F#
    cm[2] = 1310; //G
    cm[3] = 1170; //G#
    cm[4] = 1060; //
    cm[5] = 940;
    cm[6] = 830;
    cm[7] = 720;
    cm[8] = 630;
    cm[9] = 535;
    cm[10] = 440;
    cm[11] = 350;
    cm[12] = 240;
    cm[13] = 100;
    cm[14] = 50;
    cm[15] = 20;
*/

    ParamLecture pl; // défaut
        pl.maxVit = 600; // en mm/s
        pl.maxAcc = 10000; // en mm/s²
        pl.maxDec = pl.maxAcc;
        pl.tempsBase = 6 * 70;
        pl.tempsTransi = 50;
        pl.tempsMaxDepl = 40;

    ParamAccordage pac;
        pac.pasMoteur = 3*MOTEUR_POSITION_mm; // unit mot
        pac.tpsSouffle = 100; // ms
        pac.tpsSilence = 200;
        pac.tpsDeplac = 2000;

        // tableau des fréquences (en Hz)
        //octave 4:
        pac.tabFreqs["^F"] = 369.99;
        pac.tabFreqs["G"] = 392.00;
        pac.tabFreqs["^G"] = 415.30;
        pac.tabFreqs["A"] = 440.00;
        pac.tabFreqs["^A"] = 466.16;
        pac.tabFreqs["B"] = 493.88;

        //octave 5 :
        pac.tabFreqs["c"] = 523.25;
        pac.tabFreqs["^c"] = 554.37;
        pac.tabFreqs["d"] = 587.33;
        pac.tabFreqs["^d"] = 622.25;
        pac.tabFreqs["e"] = 659.25;
        pac.tabFreqs["f"] = 698.46;
        pac.tabFreqs["^f"] = 739.99;
        pac.tabFreqs["g"] = 783.99;

#define FFT_POINTS 8192
#define SAMPLE_RATE 8000
//#define BIT_RATE   32000

    FFT ffta(FFT_POINTS);
    QAudioEncoderSettings pmic;
        pmic.setCodec("audio/pcm");
        pmic.setChannelCount(1);
        pmic.setSampleRate(SAMPLE_RATE);
        //pmic.setBitRate(BIT_RATE);
        pmic.setEncodingMode(QMultimedia::ConstantBitRateEncoding);
    AnalyseMicro mic(pmic, ffta);
    Accordage ac;

    // debug
    /*
    QFile dbg_sigs("sigs.csv"), dbg_spec("spec.csv");
    dbg_sigs.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text); // | QIODevice::Unbuffered);
    dbg_spec.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text); // | QIODevice::Unbuffered);
    QTextStream ssigs(&dbg_sigs), sspec(&dbg_spec);

    mic.sauveDans(ssigs);
    ffta.sauveDans(sspec);
    */

    Partition p("../iFlute/abc/electro.abc");

    qDebug() << "Nbr de notes = " << p.notes.size();

    Comm co(DEF_PORT_MOTEUR, DEF_PORT_ARDUINO, 1000);

    QApplication app(argc, argv);
    Dialog dialog(co, p, pl, ac, pac, mic);

    dialog.importer_accordage();

    co.connect();
    co.voltage(ARDUINO_OFF);
    co.engineOn();

    QTest::qWait(150);
    co.homing();

    /*
    //co.gotoPosition(MOTEUR_POSITION_MAX, 40);
    //co.voltage(ARDUINO_OFF);

    dialog.prise_controle();
    co.engineOff();
    co.voltage(2);

    fftw_real fq = 0;
    mic.enreg(fq);
    while(!mic.pret)
        QTest::qWait(50);

    qDebug() << "FREQ = " << fq;

    co.voltage(0);
    */

    dialog.show();

    //co.voltage(ARDUINO_OFF);

    // debug files
    //dbg_sigs.close();
    //dbg_spec.close();

    return app.exec();
}
