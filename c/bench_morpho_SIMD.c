#include "valgo.h"
#include <stdio.h>
#include "nrdef.h"
#include "nrutil.h"
#include "vnrdef.h"
#include "vnrutil.h"
#include "morpho_SIMD.h"

int main()
{
	printf("Algorithme\t\tCycles\t\tPoints\t\tcpp\n");

	int i0, i1, j0, j1;
	int vi0, vi1, vj0, vj1;
	char buffer[100];

	// Image de départ
	vuint8** src = (vuint8**) LoadPGM_ui8matrix("./car3/car_3080.pgm", &i0, &i1, &j0, &j1);
	s2v(i0, i1, j0, j1, card_vuint8(), &vi0, &vi1, &vj0, &vj1);
	vuint8** m = vui8matrix(vi0 -1, vi1 +1, vj0 -1, vj1+1);
	copy_ui8matrix_ui8matrix((uint8**)src, i0, i1, j0, j1, (uint8**)m);

	unsigned nb_points = ((i1-i0)*(j1-j0));
	unsigned long long nb_cycles;
	
	// Erosion
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;
		vuint8** E;

		CHRONO_CYCLE(E = erosion_SIMD(m,vi0,vi1,vj0,vj1),nb_cycles);

		printf("erosion_SIMD\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		free_vui8matrix(E, vi0-1,vi1+1,vj0-1,vj1+1);
	}

	// Erosion réduction
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;
		vuint8** E;

		CHRONO_CYCLE(E = erosion_SIMD_reduction(m,vi0,vi1,vj0,vj1),nb_cycles);

		printf("erosion_SIMD_reduction\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		free_vui8matrix(E, vi0-1,vi1+1,vj0-1,vj1+1);
	}

	// Erosion réduction déroulage
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;
		vuint8** E;

		CHRONO_CYCLE(E = erosion_SIMD_reduction_deroulage(m,vi0,vi1,vj0,vj1),nb_cycles);

		printf("erosion_SIMD_reduction_deroulage\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		free_vui8matrix(E, vi0-1,vi1+1,vj0-1,vj1+1);
	}

	// Erosion openmp
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;
		vuint8** E;

		CHRONO_CYCLE(E = erosion_SIMD_openmp(m,vi0,vi1,vj0,vj1),nb_cycles);

		printf("erosion_SIMD_openmp\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		free_vui8matrix(E, vi0-1,vi1+1,vj0-1,vj1+1);
	}

	// Dilatation
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;
		vuint8** D;

		CHRONO_CYCLE(D = dilatation_SIMD(m,vi0,vi1,vj0,vj1),nb_cycles);

		printf("dilatation_SIMD\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		free_vui8matrix(D, vi0-1, vi1+1, vj0-1, vj1+1);
	}

	// Dilatation réduction
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;
		vuint8** D;

		CHRONO_CYCLE(D = dilatation_SIMD_reduction(m,i0,i1,j0,j1),nb_cycles);

		printf("dilatation_SIMD_reduction\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		free_vui8matrix(D, vi0-1, vi1+1, vj0-1, vj1+1);
	}

	// Dilatation réduction déroulage
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;
		vuint8** D;

		CHRONO_CYCLE(D = dilatation_SIMD_reduction_deroulage(m,i0,i1,j0,j1),nb_cycles);

		printf("dilatation_SIMD_reduction_deroulage\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		free_vui8matrix(D, vi0-1, vi1+1, vj0-1, vj1+1);
	}

	// Dilatation openmp
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;
		vuint8** D;

		CHRONO_CYCLE(D = dilatation_SIMD_openmp(m,i0,i1,j0,j1),nb_cycles);

		printf("dilatation_SIMD_openmp\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		free_vui8matrix(D, vi0-1, vi1+1, vj0-1, vj1+1);
	}

	free_ui8matrix((uint8**)src, i0,i1,j0,j1);
	free_vui8matrix(m, vi0-1,vi1+1,vj0-1,vj1+1);
	return 0;
}
