#include "valgo.h"
#include <stdio.h>
#include "nrdef.h"
#include "nrutil.h"
#include "mouvement_SIMD.h"

int main()
{
	printf("Algorithme\t\tCycles\t\tPoints\t\tcpp\n");
	
	// Variables pour SD
	vuint8 **I, **M, **O, **V, **E;
	uint8 n = 3;
	int i0, i1, j0, j1;
	int vi0, vi1, vj0, vj1;
	
	char buffer[100];

	// Image de départ
	uint8** src = LoadPGM_ui8matrix("./car3/car_3000.pgm", &i0, &i1, &j0, &j1);
	#define IMAGE_ID0 3001
	#define IMAGE_ID1 3200
	unsigned nb_points = (IMAGE_ID1 - IMAGE_ID0 + 1) * ((i1-i0)*(j1-j0));

	unsigned long long nb_cycles;

	// Sans optimisation
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		sigma_delta_SIMD_initialisation(src, &I ,&M , &O, &V, &E, i0, i1, j0, j1, &vi0, &vi1, &vj0, &vj1);
		
		nb_cycles = 0;

		CHRONO_CYCLE(sigma_delta_SIMD_iteration(I,M,O,V,E,n,vi0,vi1,vj0,vj1),nb_cycles);

		for(int i = 3001; i < 3200; i++)
		{
			sprintf(buffer, "./car3/car_%i.pgm", i);
			MLoadPGM_ui8matrix(buffer, i0, i1, j0, j1, (uint8**)I);
			CHRONO_CYCLE(sigma_delta_SIMD_iteration(I,M,O,V,E,n,vi0,vi1,vj0,vj1),nb_cycles);
		}

		printf("sigma_delta_iteration\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		sigma_delta_SIMD_free(I,M,O,V,E,vi0,vi1,vj0,vj1);
	}


	// Producteur consommateur
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		sigma_delta_SIMD_initialisation(src, &I ,&M , &O, &V, &E, i0, i1, j0, j1, &vi0, &vi1, &vj0, &vj1);
		
		nb_cycles = 0;

		CHRONO_CYCLE(sigma_delta_SIMD_iteration_prodcons(I,M,O,V,E,n,vi0,vi1,vj0,vj1),nb_cycles);

		for(int i = 3001; i < 3200; i++)
		{
			sprintf(buffer, "./car3/car_%i.pgm", i);
			MLoadPGM_ui8matrix(buffer, i0, i1, j0, j1, (uint8**)I);
			CHRONO_CYCLE(sigma_delta_SIMD_iteration_prodcons(I,M,O,V,E,n,vi0,vi1,vj0,vj1),nb_cycles);
		}

		printf("sigma_delta_SIMD_iteration_prodcons\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		sigma_delta_SIMD_free(I,M,O,V,E,vi0,vi1,vj0,vj1);
	}

	// Producteur consommateur boucle externe openmp
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		sigma_delta_SIMD_initialisation(src, &I ,&M , &O, &V, &E, i0, i1, j0, j1, &vi0, &vi1, &vj0, &vj1);
		
		nb_cycles = 0;

		CHRONO_CYCLE(sigma_delta_SIMD_iteration_prodcons_openmp(I,M,O,V,E,n,vi0,vi1,vj0,vj1),nb_cycles);

		for(int i = 3001; i < 3200; i++)
		{
			sprintf(buffer, "./car3/car_%i.pgm", i);
			MLoadPGM_ui8matrix(buffer, i0, i1, j0, j1, (uint8**)I);
			CHRONO_CYCLE(sigma_delta_SIMD_iteration_prodcons_openmp(I,M,O,V,E,n,vi0,vi1,vj0,vj1),nb_cycles);
		}

		printf("sigma_delta_SIMD_iteration_prodcons_openmp\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		sigma_delta_SIMD_free(I,M,O,V,E,vi0,vi1,vj0,vj1);
	}

	// Producteur consommateur openmp collapsed loop
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		sigma_delta_SIMD_initialisation(src, &I ,&M , &O, &V, &E, i0, i1, j0, j1, &vi0, &vi1, &vj0, &vj1);
		
		nb_cycles = 0;

		CHRONO_CYCLE(sigma_delta_SIMD_iteration_prodcons_openmp_collapsed(I,M,O,V,E,n,vi0,vi1,vj0,vj1),nb_cycles);

		for(int i = 3001; i < 3200; i++)
		{
			sprintf(buffer, "./car3/car_%i.pgm", i);
			MLoadPGM_ui8matrix(buffer, i0, i1, j0, j1, (uint8**)I);
			CHRONO_CYCLE(sigma_delta_SIMD_iteration_prodcons_openmp_collapsed(I,M,O,V,E,n,vi0,vi1,vj0,vj1),nb_cycles);
		}

		printf("sigma_delta_SIMD_iteration_prodcons_openmp_collapsed\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		sigma_delta_SIMD_free(I,M,O,V,E,vi0,vi1,vj0,vj1);
	}

	free_ui8matrix(src, i0,i1,j0,j1);
	return 0;
}
