#include <iostream>

#include "partition.h"

using namespace std;

#define LONG_TOTALE 17

int main()
{
    Corresp c_test;
    for(int i=0; i<7*NBR_OCTAVE; i++)
        c_test[i] = (i+1)*LONG_TOTALE/(7*NBR_OCTAVE); // en mm

    Partition p("C:\\Elec\\DecodeABC\\DecodeABC\\1oct.txt");

    p.lire(c_test);

    cout << "Nbr de notes = " << p.notes.size() << endl;

    return 0;
}

