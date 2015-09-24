#ifndef PARTITION_H
#define PARTITION_H

#include "note.h"
#include "note_symb.h"

#include "Accordeur/accordage.h"
#include "param_lecture.h"
#include "comm.h"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

// DEBUG
#define DEBUG

#ifdef DEBUG
    #include <iostream>
#endif

#include <QTest>

class Partition
{
public:
    Partition(const std::string & fname);
    // ~Partition();

    // lit la partition avec la table de correspondance c, de la note n1 (indice ; 0 = début) à la note n2 (-1 = la fin)
    //void lire(MasterThread & t, const Corresp & c, const ParamLecture & pl, size_t n1 = 0, size_t n2 = -1);
    void lire(Comm & t, const Accordage & ac, const ParamLecture & pl, std::size_t n1 = 0, std::size_t n2 = -1);

    // idem en temps de mesure
    //void lire(Corresp c, size_t t1 = 0, size_t t2 = -1);

    std::size_t derniere_note; // dernière indice joué dans la partition

// protected:
public:
    std::string titre;
    std::vector<Note> notes;
};

#endif // PARTITION_H
