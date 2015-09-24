#ifndef ACCORDAGE_H
#define ACCORDAGE_H

#include "micro.h"
#include "fft.h"

#include "../comm.h"
#include "../param_lecture.h"
#include "../motorhelper.h"
#include "../note_symb.h"

#include <QTest>

#include <algorithm>
#include <functional>
#include <map>

#define TABLE_ACCORD_FICH "tab_acc.dat"

// fréquence pour telle note (on peut l'utiliser comme ceci tabFreq["A'"] = tabFreq["a"] = tabFreq["^_a,'"] = x.)
typedef std::map<NoteSymb, fftw_real> tabFreq;

struct ParamAccordage {

	// défauts
	ParamAccordage() :
        pasMoteur(100),
        tpsSilence(200),
        tpsSouffle(100),
        tpsDeplac(1000)
    {}

    std::size_t pasMoteur; // pas déplacement moteur (unité position moteur)

    short tpsSilence; // temps d'attente en ms après le silence (pour éviter résonnance qui dure qqes tps)
    short tpsSouffle; // temps d'attente en ms pour que le souffle arrive
    short tpsDeplac; // temps d'attente en ms (pour laisser le temps du déplacer)

    tabFreq tabFreqs; // tableau des fréquences à tester
};

typedef std::map<NoteSymb, std::size_t> AccordageBaseMap;
class Accordage: public AccordageBaseMap
{
public:

	/* prend en entrée :
		- commandes du moteur et Arduino (pour souffler)
		- gestion du micro
        - paramètres d'accordage (dont notes à accorder)
	   retourne le nombre de notes trouvées parmi le tableau de fréquences
	   et modifie en conséquence sa map de correspondance interne
	*/
    std::size_t trouver_notes(Comm & com, AnalyseMicro & mic, const ParamAccordage & par);

    std::size_t importer_table(QTextStream & s);
    std::size_t exporter_table(QTextStream & s) const;

    /*// typedef nécessaire pour les iterators...
    typedef AccordageBaseMap base_container;
    typedef base_container::iterator iterator;
    typedef base_container::const_iterator const_iterator;*/
};

#endif // ACCORDAGE_H
