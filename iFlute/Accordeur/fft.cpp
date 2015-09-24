#include "FFT.h"

#include <iostream>
using namespace std;

FFT::FFT(size_t n) :
	N(n)
{
    H = (size_t) floor((double) N/2)+1; // useful because spectrum will be symmetric
	iN = (fftw_real) 1/N; // do not repeat this computation

    in  = fftw_alloc_real(N);
    out = fftw_alloc_complex(N); // H?
	mod = fftw_alloc_real(H);

	// FFTW_ESTIMATE = non-optimized ; FFTW_MEASURE = optimized but slow for init
	p = fftw_plan_dft_r2c_1d(N, in, out, FFTW_MEASURE); //FFTW_FORWARD,
}

FFT::~FFT()
{
	fftw_destroy_plan(p);
	fftw_free(in);
	fftw_free(out);
	fftw_free(mod);
}

// return the highest frequency peak in spectrum of data
// the outputed frequency has NOT to be multiplied by the sample rate to be physical (with an unit)
//fftw_real FFT::get_highest_peak(QDataStream & d, fftw_real & max_mod)
//fftw_real FFT::get_highest_peak(const fftw_real d[], fftw_real & max_mod)
fftw_real FFT::get_highest_peak(
                           fftw_real & max_mod,
                           fftw_real freq_ech,
                           fftw_real freq_min,
                           fftw_real freq_max
                           ) //const fftw_real d[],
{
    size_t m = -1;

	// input
    //for(size_t i = 0; i < N; i++)
        //in[i] = d[i]; // copie en place
        //d >> in[i];

	// execute optimized plan
	fftw_execute(p);

	// module calculus
    if(dbg_f != NULL)
        *dbg_f << "\n";

    for(size_t i = 0; i < H; i++) {
        mod[i] = out[i][0]*out[i][0] + out[i][1]*out[i][1];
        if(dbg_f != NULL)
            *dbg_f << i << ";" << mod[i] << "\n";
    }

    size_t idx_freq_min = ceil(N*freq_min/freq_ech);
    size_t idx_freq_max = floor(N*freq_max/freq_ech);

	// get the index of the maximum modulus (STL)
    m = std::distance(mod,
                      std::max_element(
                          mod + idx_freq_min,
                          mod + min(H,1+idx_freq_max)
                      )
                      );
	max_mod = 2*sqrt(mod[m])*iN;

    //qDebug() << "M = " << m;

    return freq_ech*m*iN;
}
