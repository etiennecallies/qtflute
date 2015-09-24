#include <QCoreApplication>
#include <QDebug>

#include "fftw3.h"

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cmath>

#include <iostream>

#include "fft.h"
#include "audio_process.h"

#include <QAudioEncoderSettings>

using namespace std;

#define SAMPLE_RATE 8000
#define BIT_RATE 320000
#define FFT_SIZE 4096

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "DEB";

    QAudioEncoderSettings s;
        // réglages
        s.setCodec("audio/pcm");
        s.setChannelCount(1);
        //s.setSampleRate(SAMPLE_RATE);
        s.setBitRate(BIT_RATE);
        s.setEncodingMode(QMultimedia::ConstantBitRateEncoding);

    FFT fft(FFT_SIZE);

    /*fftw_real c[FFT_SIZE], f = 0, amp = 0;
    for(int i = 0; i < FFT_SIZE; i++)
        c[i] = 13*cos(2*M_PI*FS/FE*i);

    f = FE * x.get_highest_peak(c, amp);

    cout << "Freq = " << f << endl;
    cout << "Ampl = " << amp << endl;*/

    AnalyseMicro am(s, fft);
    fftw_real fsig = 0;

    am.enreg(fsig);
    while(!am.pret)
        QTest::qWait(50);

    qDebug() << fsig;

    return a.exec();
}
