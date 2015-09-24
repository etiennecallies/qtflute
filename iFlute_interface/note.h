#ifndef NOTE_H
#define NOTE_H

#include "note_symb.h"

// note abstraite issue d'une lecture ABC
struct Note
{
    Note(NoteSymb s, float d = 1, bool l = false) : symb(s), duree(d), liee(l) {}

    NoteSymb symb; // note en tant que telle (type : A,B,...G, octave : 0 = réf, demi = -1,0,1 = bémol,normal,dièse)
    float duree; // multiple base de temps
    bool liee; // liaision à prochaine note

    //short intensite;
};

#endif // NOTE_H
