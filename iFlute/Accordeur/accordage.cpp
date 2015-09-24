#include "accordage.h"

using namespace std;

#define DEBUG_ACCORD

#include <QFile>
#include <QTextStream>

size_t Accordage::trouver_notes(Comm & com, AnalyseMicro & mic, const ParamAccordage & par)
{
    const size_t pnum = 1+floor((double) MOTEUR_POSITION_MAX / par.pasMoteur);
    fftw_real * pos_frq = new fftw_real[pnum]; // table d'équivalence : position moteur <=> fréquence

	// acquisition de toutes les fréquences tous les X pas de position
	{
        for(size_t p = 0; p < pnum; p++)
		{
			pos_frq[p] = 0;

			com.gotoPosition(p * par.pasMoteur); // bien attendre que le moteur soit en position (wait supplémentaire ?)
            QTest::qSleep( par.tpsDeplac); // attend un peu (stabil moteur)

            qDebug() << "Pos = " << p * par.pasMoteur;

            com.voltage(ARDUINO_ON);
            QTest::qSleep(par.tpsSouffle); // attend un peu (stabil souffle)

            mic.enreg( pos_frq[p] ); // adresse de la fréquence à mettre à jour
            while(!mic.pret)
                QTest::qWait(ANALYSE_SLEEP_TIME); // est-ce bien nécessaire ? mic non-bloquant ?

#ifdef DEBUG_ACCORD
    //qDebug() << p*par.pasMoteur << " <=> " << pos_frq[p] << " Hz";
            qDebug() << "Fs = " << pos_frq[p] << " Hz\n";
#endif // DEBUG_ACCORD

            com.voltage(ARDUINO_OFF);
            QTest::qSleep(par.tpsSilence); // attend un peu (stabil souffle)
		}
	}

	size_t compteur = 0;
	// correspondance entre fréquences position et notes (au mieux)
	// stratégie du barycentre (interp lin.) pour trouver nos fréquences
	{
		size_t p = 0;

		// boucle non optimisée mais bon pas grave.
        // on fait l'hypothèse que notre FFT a bien fonctionné
        // donc la fréquence est strictement DECROISSANTE avec p
        for(tabFreq::const_iterator it = par.tabFreqs.begin(); it != par.tabFreqs.end(); ++it)
		{
// bug mauvaise acquisition de la première FFT
#define SKIP_FIRST_WRONG_VALUES 1

            p = std::distance(pos_frq, std::lower_bound(pos_frq + SKIP_FIRST_WRONG_VALUES, pos_frq + pnum,
                                                        it->second,
                                                        std::greater<fftw_real>() // on doit inverser l'ordre pour comportement souhaité...
                                                       ));

			// cas spéciaux à traiter (bien réfléchir...)
            if(p > 0) { // cas normal : barycentre (même en dernière pos)
                size_t pos = floor(par.pasMoteur*(p + (pos_frq[p-1] - it->second)/(pos_frq[p-1] - pos_frq[p])));
                (*this)[it->first] = pos; // met à jour ou crée la correspondance
				compteur++;

                qDebug() << (float) it->first << " = " << this->at(it->first)/10000 << " mm";
			}
		}
	}

    delete pos_frq;

	// accordage terminé!
	return compteur;
}

// gestion import/export table d'accordage
size_t Accordage::importer_table(QTextStream & s)
{
    //size_t compt = 0;
    size_t pos = 0;
    char c; // ; et \n

    while(!s.atEnd())
    {
        try {
            // essaie de créer le symbole à partir du flux
            NoteSymb n(s);
//            qDebug() << "note = " << n;
            s >> c >> pos; // >> endli;
//            qDebug() << "pos = " << pos;
            (*this)[n] = pos;
            //compt++;*/
        }
        catch(...){}

        if(!s.atEnd())
            s >> c; // newline
    }

    return this->size();
}


size_t Accordage::exporter_table(QTextStream & s) const
{
    for(Accordage::const_iterator it = this->begin(); it != this->end(); ++it)
        it->first.disp(s) << ';' << it->second << '\n';

    return this->size();
}
