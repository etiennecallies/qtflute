#include "micro.h"

// #include <QStringList>

AnalyseMicro::AnalyseMicro(const QAudioEncoderSettings & s, FFT & f) :
    fft(f), dbg_f(NULL)
{
    r.setAudioSettings(s);
    QStringList sl(r.audioInputs());

    //for(QStringList::const_iterator it = sl.cbegin(); it != sl.cend(); ++it)
    //    qDebug() << *it;

    r.setAudioInput(sl.first());

    QObject::connect(&p, SIGNAL(audioBufferProbed(QAudioBuffer)), this, SLOT(traiter_buffer(QAudioBuffer)));
    p.setSource(&r);
}

AnalyseMicro::~AnalyseMicro()
{}

bool AnalyseMicro::enreg(fftw_real & frq_res)
{
    reinit();

    bool avail = r.isAvailable();

    if(avail)
    {
        // nouvelle ligne
        if(dbg_f != NULL)
            *dbg_f << "\n";

        fft_res = &frq_res;
        r.record();
    }

    return avail;
}

void AnalyseMicro::reinit()
{
    p.flush();
    fft_idx = 0;
    fft_res = NULL;
    pret = false;
}

void AnalyseMicro::traiter_buffer(const QAudioBuffer & b)
{
    // nbr de nouveaux points
    size_t nvPts = b.sampleCount();
    if(fft_idx + nvPts >= fft.size())
        r.stop(); // on arrête l'acquisition (au plus tôt)

    const quint16 * data = b.constData<quint16>();

    //qDebug() << "valid? = " << b.isValid();
    //qDebug() << "duration = " << b.duration();
    //qDebug() << "sample count = " << b.sampleCount();
    //qDebug() << "byte count = " << b.byteCount();
    //qDebug() << "frame count = " << b.frameCount();
    //qDebug() << "sample rate = " << af.sampleRate();
    //qDebug() << "bit depth = " << af.sampleSize();
    //qDebug() << "channels = " << af.channelCount();
    //qDebug() << endl;

    for(size_t i = 0; i < nvPts && i + fft_idx < fft.size(); i++)
    {
        fft.in[fft_idx + i] = (fftw_real) data[i];
        if(dbg_f != NULL)
            *dbg_f << fft_idx + i << ";" << (fftw_real) data[i] << "\n";
    }

    // mise à jour de l'insertion
    fft_idx += nvPts;
    //qDebug() << "Nombres de points FFT : " << fft_idx << " / " << fft.size();
    if(fft_idx >= fft.size() && fft_res != NULL)
    {
        fftw_real max_amp = 0;
        fftw_real freq_ech = b.format().sampleRate();
        fftw_real freq_sig = fft.get_highest_peak(max_amp
                                      ,freq_ech
                                      ,FREQ_ACCESSIBLE_MIN
                                      ,FREQ_ACCESSIBLE_MAX
                                      );

        // on met à jour la case mémoire
        qDebug() << "Fe = " << freq_ech << " Hz";
        *fft_res = freq_sig; //freq_ech * freq_p;
        //qDebug() << "Freq princ = " << *fft_res;

        pret = true;
        fft_res = NULL; // on ne met pas à jour la suite (déchet)
    }
    else
    {
        // on attend plus de données...
        //QTest::qWait(ANALYSE_SLEEP_TIME);
        QTest::qSleep(ANALYSE_SLEEP_TIME);
    }
}
