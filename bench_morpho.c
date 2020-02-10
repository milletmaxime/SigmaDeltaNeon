#include "algo.h"
#include <stdio.h>
#include "nrdef.h"
#include "nrutil.h"
#include "morpho.h"

int main()
{
	printf("Algorithme\t\tCycles\t\tPoints\t\tcpp\n");

	int i0, i1, j0, j1;
	char buffer[100];

	// Image de départ
	uint8** src = LoadPGM_ui8matrix("./car3/car_3080.pgm", &i0, &i1, &j0, &j1);
	uint8** m = ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
	copy_ui8matrix_ui8matrix(src, i0, i1, j0, j1, m);

	unsigned nb_points = ((i1-i0)*(j1-j0));
	unsigned long long nb_cycles;
	
	// Erosion
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;
		uint8** E;

		CHRONO_CYCLE(E = erosion(m,i0,i1,j0,j1),nb_cycles);

		printf("erosion\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		free_ui8matrix(E, i0-1,i1+1,j0-1,j1+1);
	}

	// Erosion réduction
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;
		uint8** E;

		CHRONO_CYCLE(E = erosion_reduction(m,i0,i1,j0,j1),nb_cycles);

		printf("erosion_reduction\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		free_ui8matrix(E, i0-1,i1+1,j0-1,j1+1);
	}

	// Erosion réduction déroulage
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;
		uint8** E;

		CHRONO_CYCLE(E = erosion_reduction_deroulage(m,i0,i1,j0,j1),nb_cycles);

		printf("erosion_reduction_deroulage\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		free_ui8matrix(E, i0-1,i1+1,j0-1,j1+1);
	}

	// Erosion openmp
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;
		uint8** E;

		CHRONO_CYCLE(E = erosion_openmp(m,i0,i1,j0,j1),nb_cycles);

		printf("erosion_openmp\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		free_ui8matrix(E, i0-1,i1+1,j0-1,j1+1);
	}

	// Dilatation
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;
		uint8** D;

		CHRONO_CYCLE(D = dilatation(m,i0,i1,j0,j1),nb_cycles);

		printf("dilatation\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		free_ui8matrix(D, i0-1,i1+1,j0-1,j1+1);
	}

	// Dilatation réduction
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;
		uint8** D;

		CHRONO_CYCLE(D = dilatation_reduction(m,i0,i1,j0,j1),nb_cycles);

		printf("dilatation_reduction\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		free_ui8matrix(D, i0-1,i1+1,j0-1,j1+1);
	}

	// Dilatation réduction déroulage
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;
		uint8** D;

		CHRONO_CYCLE(D = dilatation_reduction_deroulage(m,i0,i1,j0,j1),nb_cycles);

		printf("dilatation_reduction_deroulage\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		free_ui8matrix(D, i0-1,i1+1,j0-1,j1+1);
	}

	// Dilatation openmp
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;
		uint8** D;

		CHRONO_CYCLE(D = dilatation_openmp(m,i0,i1,j0,j1),nb_cycles);

		printf("dilatation_openmp\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		free_ui8matrix(D, i0-1,i1+1,j0-1,j1+1);
	}

	free_ui8matrix((uint8**)src, i0,i1,j0,j1);
	free_ui8matrix((uint8**)m, i0-1,i1+1,j0-1,j1+1);
	return 0;
}
