#ifndef PARTITION_H
#define PARTITION_H

#include "note.h"
#include "accord.h"
#include "param_lecture.h"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

// DEBUG
#define DEBUG

#ifdef DEBUG
    #include <iostream>
#endif

#include <QTime>
#include <QThread>
// #include <QTest>

using namespace std;

class Partition
{
public:
    Partition(string fname);
    // ~Partition();

    // lit la partition avec la table de correspondance c, de la note n1 (indice ; 0 = début) à la note n2 (-1 = la fin)
    void lire(Corresp c, ParamLecture pl, size_t n1 = 0, size_t n2 = -1);

    // idem en temps de mesure
    //void lire(Corresp c, size_t t1 = 0, size_t t2 = -1);

    size_t derniere_note; // dernière indice joué dans la partition

protected:
    string titre;
    double base_temps;
    vector<Note> notes;
};

#endif // PARTITION_H
