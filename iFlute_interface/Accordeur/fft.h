#ifndef __FFT_H__
#define __FFT_H__

#include "fftw3.h"
typedef double fftw_real;

#include <cstddef>
#include <cmath>
#include <algorithm>
#include <iterator>

// DEBUG !!!
#include <QTextStream>



class FFT
{
public:
    FFT(std::size_t);
	~FFT();

    inline void sauveDans(QTextStream & s){dbg_f = &s;}

    //fftw_real get_highest_peak(QDataStream & d, fftw_real & max_mod);

    // on recherche dans une certaine plage de fréquences données par les indices (inclus)
    fftw_real get_highest_peak(fftw_real & max_mod,
                               fftw_real freq_ech,
                               fftw_real freq_min,
                               fftw_real freq_max
                               ); //const fftw_real d[],

    std::size_t size() const {return N;}

    fftw_real	 * in; //input

protected:
	size_t N, H; // H is half N
	fftw_real iN; // inverse of N (double)

	fftw_complex * out; //output
	fftw_real	 * mod; //module

	fftw_plan p;

    QTextStream * dbg_f;
};

#endif // __FFT_H__
