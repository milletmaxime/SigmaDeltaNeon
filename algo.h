#include "nrdef.h"

#ifndef __ALGO_H__
#define __ALGO_H__

#define Vmin 1
#define Vmax 255

//Inspiré de :
//https://github.com/google/benchmark/blob/master/src/cycleclock.h
static unsigned int cycleclock_aarch64()
{
	#if defined(__aarch64__)
		unsigned int cycle;
		asm volatile("mrs %0, cntvct_el0" : "=r"(cycle));
		return cycle;
	#else
		return 0;
	#endif
}

#define NRUN 4
#define NITER 100
#define CHRONO_CYCLE_AARCH64(X,t)					\
{													\
	double tmin = 1e38;								\
	for(int run=0; run<NRUN; run++) {				\
		double t0 = (double)cycleclock_aarch64();	\
		for(int iter=0; iter<NITER; iter++) {		\
			X;										\
		}											\
		double t1 = (double)cycleclock_aarch64();	\
		double dt=t1-t0; if(dt<tmin) tmin = dt;		\
	}												\
	t = tmin / (double) NITER;						\
}

#endif
