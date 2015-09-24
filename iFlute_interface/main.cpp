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

#include "flutronic.h"

#include "Accordeur/accordage.h"
#include "Accordeur/micro.h"
#include "partition.h"
#include "param_lecture.h"

#include "comm.h"

// debug
#define DEBUG

#ifdef DEBUG
    #include <QFile>
    #include <QTextStream>
#endif

#define FFT_POINTS 8192
#define SAMPLE_RATE 8000
//#define BIT_RATE   32000

int main(int argc, char *argv[])
{
    ParamLecture pl; // défaut
        pl.maxVit = 600; // en mm/s
        pl.maxAcc = 10000; // en mm/s²
        pl.maxDec = pl.maxAcc;
        pl.tempsBase = 300;
        pl.fracTempsTransi = 0.2;
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

    FFT ffta(FFT_POINTS);
    QAudioEncoderSettings pmic;
        pmic.setCodec("audio/pcm");
        pmic.setChannelCount(1);
        pmic.setSampleRate(SAMPLE_RATE);
        //pmic.setBitRate(BIT_RATE);
        pmic.setEncodingMode(QMultimedia::ConstantBitRateEncoding);
    AnalyseMicro mic(pmic, ffta);
        mic.r.setAudioInput(mic.r.audioInputs().first()); // premier périphérique d'enregistrement par défaut...

    // debug
    #ifdef DEBUG
        QFile dbg_sigs("sigs.csv"), dbg_spec("spec.csv");
        dbg_sigs.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text); // | QIODevice::Unbuffered);
        dbg_spec.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text); // | QIODevice::Unbuffered);
        QTextStream ssigs(&dbg_sigs), sspec(&dbg_spec);

        mic.sauveDans(ssigs);
        ffta.sauveDans(sspec);
    #endif

    QApplication app(argc, argv);
    Flutronic iflute(pl, pac, mic);

    iflute.importer_accordage();
    iflute.show();

    return app.exec();
}
