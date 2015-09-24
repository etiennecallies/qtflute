#ifndef NOTE_SYMB_H
#define NOTE_SYMB_H

#include <cstring>
#include <cctype>

#include <QTextStream>

#define SYMB_SILENCE 'Z'

// définit le type de donnée pour un symbole de note
// un char + éventuellement un autre caractère (dièse, bémole, octave sup)
class NoteSymb
{
public:
    NoteSymb(const char * s, std::size_t & i);
	NoteSymb(const char * s);

    // gestion I/O
    NoteSymb(QTextStream & s);
    QTextStream & disp(QTextStream & s) const;
    //friend QTextStream & operator<<(QTextStream & s, const NoteSymb & n);

    operator float() const;

	bool operator< (const NoteSymb & n) const;
	bool operator==(const NoteSymb & n) const;
	NoteSymb & operator=(const NoteSymb & n);

    // fonctions inline courtes pour convénience
    inline bool operator<=(const NoteSymb & n) const {return *this == n || *this < n;}
    inline NoteSymb(const NoteSymb & n){*this = n;}
    inline bool vide() const {return h == 0;}
    inline bool silence() const {return h == SYMB_SILENCE;}

    inline static bool caracEstNote(char c){return (c >= 'A' && c <= 'G') || (c >= 'a' && c <= 'g');}

    // membres publics
	char h; // type note
	short oct; // octave? 0=réf
	short demi; // demi-oct (dièse +1, bémol -1, normal 0);
};

#endif // NOTE_SYMB_H
