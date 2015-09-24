#include "partition.h"

QT_USE_NAMESPACE
using namespace std;

// se met à la bonne position pour jouer la première note non silencieuse
void Partition::precharger_premiere_note(Comm & co, const Accordage & ac, size_t n1)
{
    const NoteSymb * ns = NULL;
    Accordage::const_iterator ait = ac.end();
    size_t n2 = notes.size();
    bool ok = false;

    for(size_t i = n1; i < n2 && !ok; i++)
    {
        ns = &((notes[i]).symb);
        if(!ns->silence())
        {
            ait = ac.find(*ns);
            if(ait != ac.end()) {
                co.gotoPosition(ait->second); //def params
                ok = true;
            }
        }
    }
}

// décode un fichier ABC
size_t Partition::charger_fichier(const string & fname)
{
    // enlève notes précédentes...
    notes.clear();

    ifstream f(fname.c_str(), ios_base::in);
    string line;
    size_t n; // nbr de char par ligne
    char c;

    #ifdef DEBUG
    ostringstream os("");
    #endif

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
                    //this->titre = line.substr(2);
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
                        d /= 3.f;
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
                        d = max((float) d, DIVISEUR_TPS_MINI); // [sécurité]
                    }

                    i--; // revient à l'indice juste avant


                    //l = l && !ns->silence(); // silence toujours "liés"

                    // enregistrement de la note
                    this->notes.push_back(Note(*ns, d, l));

                    #ifdef DEBUG
                    os << "Note = " << ns->h << " {" << ns->oct << ";" << ns->demi << "/2} [" << d << (l ? "l" : "") << "]" << endl;
                    #endif
                }

                delete ns;
            }
        }
    }

    #ifdef DEBUG
        qDebug() << "=== Partoche ===\n" << os.str().c_str() << '\n';
        qDebug() << "Nbr de notes = " << notes.size();
    #endif

    return notes.size();
}

// lit la partition
void Partition::lire(Comm & co, const Accordage & ac, const ParamLecture & pl, size_t n1, size_t n2)
{
    const Note * n = NULL, * npre = NULL;
    const NoteSymb * ns = NULL;
    Accordage::const_iterator ait = ac.end();
    size_t pos_prec = 0;
    double dist_note = 0.;
    size_t vit_depl = 0; // proportionnel à la distance dont on doit se déplacer
    size_t tps_transi = 0; // proportionnel à la durée de la note

    if(n2 == -1)
        n2 = notes.size();

    co.voltage(ARDUINO_OFF);
    for(size_t i = n1; i < n2; i++)
    {
        n = &notes[i];
        ns = &n->symb;

        tps_transi = 0;
        // note
        if(!ns->silence())
        {
            ait = ac.find(*ns);
            // note trouvée dans la table d'accordage
            if(ait != ac.end())
            {
                // en premier
                dist_note = abs((double) ait->second - pos_prec) / MOTEUR_POSITION_mm; // en mm
                vit_depl = min((int) floor(dist_note * 1000 / pl.tempsMaxDepl), pl.maxVit); // en mm / s (attention)
                co.gotoPosition(ait->second, vit_depl, pl.maxAcc, pl.maxDec);
                pos_prec = ait->second; // on enregistre la position où on sera au prochain tour de boucle

                //Si la note est inférieure à A (ordre des notes!!)
                if(*ns < NoteSymb("A"))
                    co.voltage(ARDUINO_GRAVE);
                else
                    co.voltage(ARDUINO_AIGU);

                qDebug() << "souffler";
                qDebug() << "position=" << ac.at(*ns);
            }

            tps_transi = ceil(pl.tempsBase * pl.fracTempsTransi * n->duree);
        }
        // silence
        else {
            co.voltage(ARDUINO_OFF);
            qDebug() << "silence";
        }

        // durée de la note : microsecondes
        QTest::qWait(ceil(pl.tempsBase * n->duree) - tps_transi); // doit tjrs être positif!

        // transition avec note suivante
        if(!n->liee){
            co.voltage(ARDUINO_SILENCE);
            qDebug() << "transition";
        }
        QTest::qWait(tps_transi);

        // pour mémoire
        derniere_note = i;
        npre = n;
    }
    co.voltage(ARDUINO_OFF);
}
