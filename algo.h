#include "nrdef.h"

#ifndef __ALGO_H__
#define __ALGO_H__

#define Vmin 0
#define Vmax 255

#define NB_REPETITION_BENCH 25

//Inspiré de :
//https://github.com/google/benchmark/blob/master/src/cycleclock.h
static unsigned long long cycleclock_aarch64()
{
	#if defined(__aarch64__)
		unsigned long long cycle;
		asm volatile("mrs %0, cntvct_el0" : "=r"(cycle));
		return cycle;
	#else
		return 0;
	#endif
}

#define CHRONO_CYCLE_AARCH64(X,chrono)				\
{													\
	unsigned long long 	t0 = cycleclock_aarch64();	\
	X;												\
	chrono += cycleclock_aarch64() - t0;				\
}

#endif
