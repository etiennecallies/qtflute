#include "partition.h"

QT_USE_NAMESPACE

// décode un fichier ABC
Partition::Partition(string fname) :
    derniere_note(0), base_temps(1000)
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
    bool diese;
    bool bemol;

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
                diese = bemol = false;
                c = line[i];
                if(c == '^'){
                    diese = true;
                    i = i+1;
                    c = line[i];
                }
                if(c == '_'){
                    bemol = true;
                    i = i+1;
                    c = line[i];
                }
                cs = line[i+1]; // existe même en i=n-1

                // défauts
                h = -2;
                d = 1;
                l = false;

                if((c >= 'A' && c <= 'G') || (c >= 'a' && c <= 'g')){
                    h = getHauteur(c, diese, bemol);
                }
//                // octave de base
//                if(c >= 'A' && c <= 'G')
//                    h = c - 'A'; // de manière à ce que 'A' vaille 0
//                // octave sup
//                else if(c >= 'a' && c <= 'g')
//                    h = c - 'a' + 7; // de manière à ce que 'a' suive le numéro de 'G'
//                // silences
                else if(c == 'z')
                    h = -1;
                // le reste est du déchet

                if(bemol){
                    h = h - 0.5;
                }
                if(diese){
                    h = h + 0.5;
                }


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

int Partition::getHauteur(char letter, bool diese, bool bemol){
    //0: F
    //1: F# - Gb
    //2: G
    //3: G# - Ab
    //4: A
    //5: A# - Bb
    //6: B
    //7: c
    //8: c# - db
    //9: d
    //10: d# - eb
    //11: e
    //12: f
    //13: f# - gb
    //14: g
    //15: g#

    switch(letter){
    case 'F':
        if(diese){
            return 1;
        }
        return 0;
        break;

    case 'G':
        if(bemol){
            return 1;
        }
        if(diese){
            return 3;
        }
        return 2;
        break;
    case 'A':
        if(bemol){
            return 3;
        }
        if(diese){
            return 5;
        }
        return 4;
        break;
    case 'B':
        if(bemol){
            return 5;
        }
        return 6;
        break;
    case 'c':
        if(diese){
            return 8;
        }
        return 7;
        break;
    case 'd':
        if(bemol){
            return 8;
        }
        if(diese){
            return 10;
        }
        return 9;
        break;
    case 'e':
        if(bemol){
            return 10;
        }
        return 11;
        break;
    case 'f':
        if(diese){
            return 13;
        }
        return 12;
        break;
    case 'g':
        if(bemol){
            return 13;
        }
        if(diese){
            return 15;
        }
        return 14;
        break;

    }
    return -1;
}


// lit la partition
void Partition::lire(Comm & t, const Corresp & c, const ParamLecture & pl, size_t n1, size_t n2)
{
    const Note * n = NULL;
    int h;

    if(n2 == -1)
        n2 = notes.size();

    for(size_t i = n1; i < n2; i++)
    {
        n = &notes[i];
        h = n->hauteur;

        // note
        if(h >= 0)
        {
            t.voltage(pl.intensite);
            t.gotoPosition(c[h], pl.maxVit, pl.maxAcc, pl.maxDec);
            qDebug() << "t.voltage+;";
            qDebug() << "t.gotoPosition=" << c[h];
        }
        // silence
        else{
            t.voltage(ARDUINO_SILENCE);
            qDebug() << "t.voltage(0);";
        }

        // microsecondes
        //QThread::msleep(base_temps * n->duree); //t.
        QTest::qWait(base_temps * n->duree);

        if(!n->liee && h != -1){
            t.voltage(ARDUINO_SILENCE);
            QTest::qWait(50);
            qDebug() << "t.voltage(0);";
        }

        derniere_note = i;
    }
}
