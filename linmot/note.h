#ifndef NOTE_H
#define NOTE_H

// note abstraite issue d'une lecture ABC
struct Note
{
    Note(int h, int d = 1, bool l = false) : hauteur(h), duree(d), liee(l) {}

    int hauteur; // hauteur de note (entier répresentant un type de note : A, B, C...)
    int duree; // multiple base de temps
    bool liee; // liaision à prochaine note

    //int intensite;
};

#endif // NOTE_H
