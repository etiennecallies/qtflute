#ifndef ACCORD_H
#define ACCORD_H

#include <array>

#define NBR_OCTAVE 1

// correspondance entre hauteur abstraite de note et longueur de coulisse (en mm)
typedef std::array<int, 7*NBR_OCTAVE> Corresp;

#endif // ACCORD_H
