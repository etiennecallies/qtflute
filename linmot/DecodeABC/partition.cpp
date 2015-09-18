#include "partition.h"

// décode un fichier ABC
Partition::Partition(string fname) :
    derniere_note(0)
{
    ifstream f(fname);
    string line;
    size_t n; // nbr de char par ligne
    char c, cs;

    ostringstream os("");

    // propriétés des notes
    int h;
    int d;
    bool l;

    while(f.good())
    {
        getline(f, line);
        n = line.length();

        // headers (cf. aide sur format ABC)
        if(n >= 2 && line[1] == ':')
            switch(line[0])
            {
                case 'X': // numéro du morceau
                    break;
                case 'K': // tonalité
                    break;
                case 'T': // titre
                    this->titre = line.substr(2);
                    break;
                case 'M': // mesure classique
                    break;
                case 'L': // temps de base
                    //this->base_temps=;
                    break;
            }
        else if(n > 0) // ligne non vide
        {
            // on rajoute deux caractères déchets pour finir la ligne.
            line += ';';  line += ';';

            for(size_t i = 0; i < n; i++)
            {
                c = line[i];
                cs = line[i+1]; // existe même en i=n-1

                // défauts
                h = -2;
                d = 1;
                l = false;

                // octave de base
                if(c >= 'A' && c <= 'G')
                    h = c - 'A'; // de manière à ce que 'A' vaille 0
                // octave sup
                else if(c >= 'a' && c <= 'g')
                    h = c - 'a' + 7; // de manière à ce que 'a' suive le numéro de 'G'
                // silences
                else if(c == 'z')
                    h = -1;
                // le reste est du déchet


                if(h >= -1)
                {
                    // étude d'après pour les notes et silences

                        // notes liées
                        if((cs >= 'A' && cs <= 'G') || (cs >= 'a' && cs <= 'g'))
                            l = true;
                        // durées
                        else if(cs >= '0' && cs <= '9')
                        {
                            d = cs - '0'; // durée note
                            cs = line[i+2]; // caractère encore suivant ; existe même en i=n-1
                            l = (cs >= 'A' && cs <= 'G') || (cs >= 'a' && cs <= 'g');
                            i++; // on évite le caractère d'après qui est la durée de cette note
                        }

                   // enregistrement de la note
                   this->notes.push_back(Note(h, d, l));

                   os << "Note = " << h << " [" << d << "] " << l << endl;
                }
            }
        }
    }

    #ifdef DEBUG
        cout << "=== Partoche ===" << endl << os.str() << endl;
    #endif
}


// lit la partition
void Partition::lire(Corresp c, ParamLecture pl, size_t n1 = 0, size_t n2 = -1)
{
    const Note n;

    if(n2 == -1)
        n2 = notes.size();

    for(size_t i = n1; i < n2; i++)
    {
        n = notes[i];

        // note
        if(h >= 0)
        {
            // voltage pl.intensite
            // moteur goto c[h] avec pl.transitions
        }
        // silence
        else
            // voltage 0

        // microsecondes
        QThread::msleep(base_temps * n.duree);

        if(!n.liee && h != -1)
            // voltage 0

        derniere_note = i;
    }
}
