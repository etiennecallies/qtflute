#ifndef MICRO_H
#define MICRO_H

#include <QAudioBuffer>
#include <QAudioEncoderSettings>
#include <QAudioRecorder>
#include <QAudioProbe>
#include <QAudioFormat>
#include <QObject>
#include <QTest>

#include <QDebug>

// temps insécable d'attente de buffer (buffer chunk = 40 ms)
#define ANALYSE_SLEEP_TIME 45

#include "fft.h"

// intervalle des fréquences accessibles (pré-test manuel avant accordage), en Hz
#define FREQ_ACCESSIBLE_MAX 850
#define FREQ_ACCESSIBLE_MIN 350

#include <QTextStream>

QT_USE_NAMESPACE

class AnalyseMicro: public QObject
{
    Q_OBJECT

public:
    AnalyseMicro(QAudioEncoderSettings const &, FFT &);
    ~AnalyseMicro();

    //bool enreg(fftw_real &);

    // pour débug on enregistre les spectres dans un fichier
    inline void sauveDans(QTextStream & s){dbg_f = &s;}

    bool enreg(fftw_real &);

    void reinit();

    bool pret;

public slots:
    void traiter_buffer(const QAudioBuffer &);

protected:
    // debug
    QTextStream * dbg_f;

    // gestion du micro
    QAudioRecorder r;
    QAudioProbe p;

    // FFT en temps en réel
    FFT & fft;
    size_t fft_idx;
    fftw_real * fft_res; // fréquence à mettre en jour en fin d'analyse
};

#endif // MICRO_H
