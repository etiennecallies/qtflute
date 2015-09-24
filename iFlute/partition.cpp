#include "partition.h"

QT_USE_NAMESPACE
using namespace std;

// décode un fichier ABC
Partition::Partition(const string & fname) :
    derniere_note(0)
{
    ifstream f(fname.c_str(), ios_base::in);
    string line;
    size_t n; // nbr de char par ligne
    char c;

    ostringstream os("");

    // propriétés des notes
    NoteSymb * ns = NULL;
    float d;
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
                // init défauts
                l = false;
                d = 1.f;

                // gestion des parenthèses de liaison '('
                // et des triolets "(3"
                c = line[i];

                analyse_parenthese:
                if(c == '(') {
                    c = line[++i];
                    if(c != '3') {
                        l = true;
                        goto analyse_parenthese;
                    }
                    else { // triolet
                        d = 1/3.f;
                        i++;
                    }
                }

                // on tente de créer un symbole de note à partir de l'indice i de la chaîne :
                ns = new NoteSymb(line.c_str(), i);
                // s'il on a réussi à créer un symbole de note (ie sans durée ni rien liaison)
                if(!ns->vide())
                {
                    // le curseur i pointe alors sur le caractère juste après le dernier caractère définissant le symbole de note
                    // ex: C,,4 : i pointera sur 4
                    c = line[i]; // existe tjrs (car on a rajouté des lignes déchets)

                    // obsolète nouveau type de liaison standard
                    // la prochaine note est DIRECTEMENT un symbole de note?
                    /*if(NoteSymb::caracEstNote(c) || c == '_' || c == '^') {
                        l = true;
                        i--; // on décrémente pour que, après la fin du cycle, le i incrémenté tombe sur ce caractère
                    }
                    else*/ if(c >= '0' && c <= '9') {

                        d = c - '0';
                        i++; // ok réfléchir.. on décrémente après

                        // obsolète nouveau type de liaison standard
                        /*cs = line[i+1]; // caractère encore après (existe tjrs)
                        l = NoteSymb::caracEstNote(cs) || cs == '_' || cs == '^'; // liaision directe?
                        i += l ? 0 : 1; // on incrémente que de 1 car à la fin du cycle on sera déjà au caractère +2*/
                    }
                    else if(c == '/') { // croches
                        while(line[i++] == '/')
                            d /= 2.f;
                        d = max((float) d, 0.125f); // diviseur mini (triple croche) [sécurité]
                        //i--; // revient à l'indice juste avant
                    }

                    i--; // revient à l'indice juste avant


                    //l = l && !ns->silence(); // silence toujours "liés"

                    // enregistrement de la note
                    this->notes.push_back(Note(*ns, d, l));

                    os << "Note = " << ns->h << " {" << ns->oct << ";" << ns->demi << "/2} [" << d << (l ? "l" : "") << "]" << endl;
                }

                delete ns;
            }
        }
    }

    #ifdef DEBUG
        cout << "=== Partoche ===" << endl << os.str() << endl;
    #endif
}

// lit la partition
void Partition::lire(Comm & t, const Accordage & ac, const ParamLecture & pl, size_t n1, size_t n2)
{
    const Note * n = NULL, * npre = NULL;
    const NoteSymb * ns = NULL;
    Accordage::const_iterator ait = ac.end();
    size_t pos_prec = 0;
    double dist_note = 0.;
    size_t vit_transi = 0;

    if(n2 == -1)
        n2 = notes.size();

    for(size_t i = n1; i < n2; i++)
    {
        n = &notes[i];
        ns = &n->symb;

        qDebug() << ns->h;

        // note
        if(!ns->silence())
        {
            ait = ac.find(*ns);
            // note trouvée dans la table d'accordage
            if(ait != ac.end())
            {
                // en premier
                dist_note = abs((double) ait->second - pos_prec) / MOTEUR_POSITION_mm; // en mm
                vit_transi = min((int) floor(dist_note * 1000 / pl.tempsMaxDepl), pl.maxVit); // en mm / s (attention)
                t.gotoPosition(ait->second, vit_transi, pl.maxAcc, pl.maxDec);
                pos_prec = ait->second; // on enregistre la position où on sera au prochain tour de boucle

                //Si la note est inférieure à A (ordre des notes!!)
                if(*ns < NoteSymb("A"))
                    t.voltage(ARDUINO_GRAVE);
                else
                    t.voltage(ARDUINO_AIGU);

                qDebug() << "t.voltage+;";
                qDebug() << "t.gotoPosition=" << ac.at(*ns);
            }
        }
        // silence
        else {
            t.voltage(ARDUINO_OFF);
            qDebug() << "t.voltage(0);";
        }

        // microsecondes
        //QThread::msleep(base_temps * n->duree); //t.
        QTest::qWait(ceil(pl.tempsBase * n->duree - pl.tempsTransi));

        if(!n->liee){
            t.voltage(ARDUINO_SILENCE);
            qDebug() << "t.voltage(0);";
        }

        QTest::qWait(pl.tempsTransi);

        derniere_note = i;
        npre = n;
    }

    t.voltage(ARDUINO_OFF);
}
