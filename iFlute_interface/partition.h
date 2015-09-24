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
    #include <QDebug>
#endif

#include <QTest>

// diviseur mini (triple croche)
#define DIVISEUR_TPS_MINI 0.125f

class Partition
{
public:
    Partition() : derniere_note(0){}
    // ~Partition();

    // charge un nouveau fichier de partition
    // retourne le nbr de notes
    std::size_t charger_fichier(const std::string & fname);

    // se met à la bonne position pour jouer la première note non silencieuse
    void precharger_premiere_note(Comm &, const Accordage &, std::size_t n1 = 0);

    // lit la partition avec la table de correspondance c, de la note n1 (indice ; 0 = début) à la note n2 (-1 = la fin)
    void lire(Comm &, const Accordage &, const ParamLecture &, std::size_t n1 = 0, std::size_t n2 = -1);

    std::size_t derniere_note; // dernière indice joué dans la partition

// protected:
public:
    //std::string titre;
    std::vector<Note> notes;
};

#endif // PARTITION_H
