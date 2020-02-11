#include "nrdef.h"

#ifndef __ALGO_H__
#define __ALGO_H__

#define Vmin 0
#define Vmax 255

#define NB_REPETITION_BENCH 25

//Inspiré de :
//https://github.com/google/benchmark/blob/master/src/cycleclock.h
static unsigned long long cycleclock()
{
	#if defined(__aarch64__)
		unsigned long long cycle;
		asm volatile("mrs %0, cntvct_el0" : "=r"(cycle));
		return cycle;
	#elif defined(__i386__)
		unsigned long long cycle;
		__asm__ volatile("rdtsc" : "=A"(cycle));
		return cycle;
	#elif defined(__x86_64__) || defined(__amd64__)
		unsigned long long cycle_low, cycle_high;
		__asm__ volatile("rdtsc" : "=a"(cycle_low), "=d"(cycle_high));
		return ((cycle_high << 32) | cycle_low)/100;
	#else
		return 0;
	#endif
}

#define CHRONO_CYCLE(X,chrono)				\
{													\
	unsigned long long 	t0 = cycleclock();	\
	X;												\
	chrono += cycleclock() - t0;				\
}

#endif
