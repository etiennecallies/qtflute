#include "note_symb.h"

// crée de manière générique un symbole depuis une chaîne de caractère longue
// on modifie i de tel manière à ce que la fin de note soit en s[i]

using namespace std;

NoteSymb::NoteSymb(const char * s, size_t & i)
{
	h = 0;
	demi = 0;
	oct = 0;

	size_t n = (s == NULL) ? 0 : strlen(s);
	if(i < n)
	{
        if(NoteSymb::caracEstNote(s[i]))
		{
			h = s[i];

            if(islower((int) h)) {
				h = (char) toupper((int) h);
				oct++;
			}

			while(++i < n && (s[i] == '\'' || s[i] == ',')) {
				oct += (s[i] == '\'' ? 1 : -1);
			}
		}
		else if(i+1 < n && (s[i] == '_' || s[i] == '^'))
		{
			demi += (s[i] == '^') ? 1 : -1;
			// on crée une note à partir de la suite.
			NoteSymb n(s, ++i);
			h = n.h;
			oct = n.oct;
			demi += n.demi; // superflu théoriquement
		}
		else if(s[i] == 'z' || s[i] == 'Z') { // silence
            h = SYMB_SILENCE; i++;
		}
	}
}

// opération de conversion d'une note en float signé (atour de l'octave de référence 0)
// C (= Do)
// C#/Db
// D
// D#/Eb
// E
// F
// F#/Gb
// G
// G#/Ab
// A
// A#/Bb
// B (= Si)

NoteSymb::operator float() const
{
    float res = 7.f*oct;

    // attention pas de demi-ton entre Si et Do (B et C) ni Mi et Fa (E et F)
    // on fait confiance à l'utilisateur pour ne pas écrire ces notes délirantes...
    // Cb, B#, E#, Fb
    res += 0.5f*demi;

    // C = 0
    if(h >= 'C' && h <= 'G')
        res += h - 'C';
    else // de A à B : A (La) = 5
        res += h - 'A' + 5;

    return res;
}

NoteSymb & NoteSymb::operator=(const NoteSymb & n)
{
	h = n.h;
	oct = n.oct;
	demi = n.demi;

	return *this;
}

NoteSymb::NoteSymb(const char * s)
{
	size_t i = 0;
	NoteSymb n(s, i);
	*this = n;
}

// pour récupérer un symbole de note simplement depuis un stream
NoteSymb::NoteSymb(QTextStream & s)
{
    char c; // déchet ';'

    // attention à l'ordre
    s >> oct >> c >> h >> c >> demi;
}
QTextStream & NoteSymb::disp(QTextStream & s) const
{
    s << oct << ';' << h << ';' << demi;
    return s;
}

// la note se fait d'abord caster en float....
/*QTextStream & operator<<(QTextStream & s, const NoteSymb & n)
{
    s << n.oct << ';' << n.h << ';' << n.demi;
    return s;
}*/

bool NoteSymb::operator==(const NoteSymb & n) const
{
    /* non exact si on considère que par exemple Do # == Ré b
    return n.h == h && n.oct == oct && n.demi == demi;
    */

    return ((float) *this) == ((float) n);
}

// ordre lexicographique
bool NoteSymb::operator<(const NoteSymb & n) const
{
    /* non exact si on considère que par exemple Do # == Ré b
	// comparaison d'octave
	if(n.oct < oct) return false;
	else if(oct < n.oct) return true;

		// même octave
		if(n.h < h) return false;
		else if(h < n.h) return true;

			// même note
			return demi < n.demi;
    */

    return ((float) *this) < ((float) n);
}
