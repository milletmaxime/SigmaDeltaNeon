#include "algo.h"
#include <stdio.h>
#include "nrdef.h"
#include "nrutil.h"
#include "morpho.h"
#include "mouvement.h"

int main()
{
	printf("Algorithme\t\tCycles\t\tPoints\t\tppc\n");

	uint8 **I, **M, **O, **V, **E;
	uint8 n = 3;
	int i0, i1, j0, j1;

	char buffer[100];

	// Image de départ
	uint8** src = LoadPGM_ui8matrix("./car3/car_3000.pgm", &i0, &i1, &j0, &j1);
	#define IMAGE_ID0 3001
	#define IMAGE_ID1 3200
	unsigned nb_points = (IMAGE_ID1 - IMAGE_ID0 + 1) * ((i1-i0)*(j1-j0));
	unsigned long long nb_cycles;

	// Pointeur pour sortie SD avec ajout d'un bord pour enchainer sur morpho
	uint8** cpy;

	// Pour morphos
	uint8 **E1, **D1, **D2, **E2;


	// SD et morpho sans optimisations
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;

		sigma_delta_initialisation(src, &I ,&M , &O, &V, &E, i0, i1, j0, j1);

		CHRONO_CYCLE_AARCH64(sigma_delta_iteration(I,M,O,V,E,n,i0,i1,j0,j1),nb_cycles);

		// Ajoute un bord à la sortie de SD
		cpy= ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
		copy_ui8matrix_ui8matrix((uint8**)E, i0, i1, j0, j1, (uint8**)cpy);

		// Enchainement des morphos
		CHRONO_CYCLE_AARCH64(E1 = erosion(cpy, i0, i1, j0, j1) ,nb_cycles);
		CHRONO_CYCLE_AARCH64(D1 = dilatation(E1, i0, i1, j0, j1) ,nb_cycles);
		CHRONO_CYCLE_AARCH64(D2 = dilatation(D1, i0, i1, j0, j1) ,nb_cycles);
		CHRONO_CYCLE_AARCH64(E2 = erosion(D2, i0, i1, j0, j1) ,nb_cycles);

		free_ui8matrix(E1, i0-1,i1+1,j0-1,j1+1);
		free_ui8matrix(E2, i0-1,i1+1,j0-1,j1+1);
		free_ui8matrix(D2, i0-1,i1+1,j0-1,j1+1);
		free_ui8matrix(D1, i0-1,i1+1,j0-1,j1+1);
		free_ui8matrix(cpy, i0-1,i1+1,j0-1,j1+1);


		for(int i = IMAGE_ID0; i < IMAGE_ID1; i++)
		{
			sprintf(buffer, "./car3/car_%i.pgm", i);
			MLoadPGM_ui8matrix(buffer, i0, i1, j0, j1, (uint8**)I);

			CHRONO_CYCLE_AARCH64(sigma_delta_iteration(I,M,O,V,E,n,i0,i1,j0,j1),nb_cycles);

			// Ajout du bord à la sortie SD
			cpy= ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
			copy_ui8matrix_ui8matrix((uint8**)E, i0, i1, j0, j1, (uint8**)cpy);

			// Enchainement des morphos
			CHRONO_CYCLE_AARCH64(E1 = erosion(cpy, i0, i1, j0, j1) ,nb_cycles);
			CHRONO_CYCLE_AARCH64(D1 = dilatation(E1, i0, i1, j0, j1) ,nb_cycles);
			CHRONO_CYCLE_AARCH64(D2 = dilatation(D1, i0, i1, j0, j1) ,nb_cycles);
			CHRONO_CYCLE_AARCH64(E2 = erosion(D2, i0, i1, j0, j1) ,nb_cycles);

			free_ui8matrix(E1, i0-1,i1+1,j0-1,j1+1);
			free_ui8matrix(E2, i0-1,i1+1,j0-1,j1+1);
			free_ui8matrix(D2, i0-1,i1+1,j0-1,j1+1);
			free_ui8matrix(D1, i0-1,i1+1,j0-1,j1+1);
			free_ui8matrix(cpy, i0-1,i1+1,j0-1,j1+1);
		}
		printf("sigma_delta_iteration+E-D-D-E\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_points/(nb_cycles + 0.0));

		sigma_delta_free(I,M,O,V,E,i0,i1,j0,j1);
	}
	
	// SD producteur consommateur et morpho avec réduction
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;

		sigma_delta_initialisation(src, &I ,&M , &O, &V, &E, i0, i1, j0, j1);

		CHRONO_CYCLE_AARCH64(sigma_delta_iteration_prodcons(I,M,O,V,E,n,i0,i1,j0,j1),nb_cycles);

		// Ajoute un bord à la sortie de SD
		cpy= ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
		copy_ui8matrix_ui8matrix((uint8**)E, i0, i1, j0, j1, (uint8**)cpy);

		// Enchainement des morphos
		CHRONO_CYCLE_AARCH64(E1 = erosion_reduction(cpy, i0, i1, j0, j1) ,nb_cycles);
		CHRONO_CYCLE_AARCH64(D1 = dilatation_reduction(E1, i0, i1, j0, j1) ,nb_cycles);
		CHRONO_CYCLE_AARCH64(D2 = dilatation_reduction(D1, i0, i1, j0, j1) ,nb_cycles);
		CHRONO_CYCLE_AARCH64(E2 = erosion_reduction(D2, i0, i1, j0, j1) ,nb_cycles);

		free_ui8matrix(E1, i0-1,i1+1,j0-1,j1+1);
		free_ui8matrix(E2, i0-1,i1+1,j0-1,j1+1);
		free_ui8matrix(D2, i0-1,i1+1,j0-1,j1+1);
		free_ui8matrix(D1, i0-1,i1+1,j0-1,j1+1);
		free_ui8matrix(cpy, i0-1,i1+1,j0-1,j1+1);


		for(int i = IMAGE_ID0; i < IMAGE_ID1; i++)
		{
			sprintf(buffer, "./car3/car_%i.pgm", i);
			MLoadPGM_ui8matrix(buffer, i0, i1, j0, j1, (uint8**)I);

			CHRONO_CYCLE_AARCH64(sigma_delta_iteration_prodcons(I,M,O,V,E,n,i0,i1,j0,j1),nb_cycles);

			// Ajout du bord à la sortie SD
			cpy= ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
			copy_ui8matrix_ui8matrix((uint8**)E, i0, i1, j0, j1, (uint8**)cpy);

			// Enchainement des morphos
			CHRONO_CYCLE_AARCH64(E1 = erosion_reduction(cpy, i0, i1, j0, j1) ,nb_cycles);
			CHRONO_CYCLE_AARCH64(D1 = dilatation_reduction(E1, i0, i1, j0, j1) ,nb_cycles);
			CHRONO_CYCLE_AARCH64(D2 = dilatation_reduction(D1, i0, i1, j0, j1) ,nb_cycles);
			CHRONO_CYCLE_AARCH64(E2 = erosion_reduction(D2, i0, i1, j0, j1) ,nb_cycles);

			free_ui8matrix(E1, i0-1,i1+1,j0-1,j1+1);
			free_ui8matrix(E2, i0-1,i1+1,j0-1,j1+1);
			free_ui8matrix(D2, i0-1,i1+1,j0-1,j1+1);
			free_ui8matrix(D1, i0-1,i1+1,j0-1,j1+1);
			free_ui8matrix(cpy, i0-1,i1+1,j0-1,j1+1);
		}
		printf("sigma_delta_iteration_prodcons+E-D-D-E_reduction\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_points/(nb_cycles + 0.0));

		sigma_delta_free(I,M,O,V,E,i0,i1,j0,j1);
	}
	
	// SD producteur consommateur et morpho avec réduction et déroulage
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;

		sigma_delta_initialisation(src, &I ,&M , &O, &V, &E, i0, i1, j0, j1);

		CHRONO_CYCLE_AARCH64(sigma_delta_iteration_prodcons(I,M,O,V,E,n,i0,i1,j0,j1),nb_cycles);

		// Ajoute un bord à la sortie de SD
		cpy= ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
		copy_ui8matrix_ui8matrix((uint8**)E, i0, i1, j0, j1, (uint8**)cpy);

		// Enchainement des morphos
		CHRONO_CYCLE_AARCH64(E1 = erosion_reduction_deroulage(cpy, i0, i1, j0, j1) ,nb_cycles);
		CHRONO_CYCLE_AARCH64(D1 = dilatation_reduction_deroulage(E1, i0, i1, j0, j1) ,nb_cycles);
		CHRONO_CYCLE_AARCH64(D2 = dilatation_reduction_deroulage(D1, i0, i1, j0, j1) ,nb_cycles);
		CHRONO_CYCLE_AARCH64(E2 = erosion_reduction_deroulage(D2, i0, i1, j0, j1) ,nb_cycles);

		free_ui8matrix(E1, i0-1,i1+1,j0-1,j1+1);
		free_ui8matrix(E2, i0-1,i1+1,j0-1,j1+1);
		free_ui8matrix(D2, i0-1,i1+1,j0-1,j1+1);
		free_ui8matrix(D1, i0-1,i1+1,j0-1,j1+1);
		free_ui8matrix(cpy, i0-1,i1+1,j0-1,j1+1);


		for(int i = IMAGE_ID0; i < IMAGE_ID1; i++)
		{
			sprintf(buffer, "./car3/car_%i.pgm", i);
			MLoadPGM_ui8matrix(buffer, i0, i1, j0, j1, (uint8**)I);

			CHRONO_CYCLE_AARCH64(sigma_delta_iteration_prodcons(I,M,O,V,E,n,i0,i1,j0,j1),nb_cycles);

			// Ajout du bord à la sortie SD
			cpy= ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
			copy_ui8matrix_ui8matrix((uint8**)E, i0, i1, j0, j1, (uint8**)cpy);

			// Enchainement des morphos
			CHRONO_CYCLE_AARCH64(E1 = erosion_reduction_deroulage(cpy, i0, i1, j0, j1) ,nb_cycles);
			CHRONO_CYCLE_AARCH64(D1 = dilatation_reduction_deroulage(E1, i0, i1, j0, j1) ,nb_cycles);
			CHRONO_CYCLE_AARCH64(D2 = dilatation_reduction_deroulage(D1, i0, i1, j0, j1) ,nb_cycles);
			CHRONO_CYCLE_AARCH64(E2 = erosion_reduction_deroulage(D2, i0, i1, j0, j1) ,nb_cycles);

			free_ui8matrix(E1, i0-1,i1+1,j0-1,j1+1);
			free_ui8matrix(E2, i0-1,i1+1,j0-1,j1+1);
			free_ui8matrix(D2, i0-1,i1+1,j0-1,j1+1);
			free_ui8matrix(D1, i0-1,i1+1,j0-1,j1+1);
			free_ui8matrix(cpy, i0-1,i1+1,j0-1,j1+1);
		}
		printf("sigma_delta_iteration_prodcons+E-D-D-E_reduction_deroulage\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_points/(nb_cycles + 0.0));

		sigma_delta_free(I,M,O,V,E,i0,i1,j0,j1);
	}

	// SD producteur consommateur et morpho openmp
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;

		sigma_delta_initialisation(src, &I ,&M , &O, &V, &E, i0, i1, j0, j1);

		CHRONO_CYCLE_AARCH64(sigma_delta_iteration_prodcons(I,M,O,V,E,n,i0,i1,j0,j1),nb_cycles);

		// Ajoute un bord à la sortie de SD
		cpy= ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
		copy_ui8matrix_ui8matrix((uint8**)E, i0, i1, j0, j1, (uint8**)cpy);

		// Enchainement des morphos
		CHRONO_CYCLE_AARCH64(E1 = erosion_openmp(cpy, i0, i1, j0, j1) ,nb_cycles);
		CHRONO_CYCLE_AARCH64(D1 = dilatation_openmp(E1, i0, i1, j0, j1) ,nb_cycles);
		CHRONO_CYCLE_AARCH64(D2 = dilatation_openmp(D1, i0, i1, j0, j1) ,nb_cycles);
		CHRONO_CYCLE_AARCH64(E2 = erosion_openmp(D2, i0, i1, j0, j1) ,nb_cycles);

		free_ui8matrix(E1, i0-1,i1+1,j0-1,j1+1);
		free_ui8matrix(E2, i0-1,i1+1,j0-1,j1+1);
		free_ui8matrix(D2, i0-1,i1+1,j0-1,j1+1);
		free_ui8matrix(D1, i0-1,i1+1,j0-1,j1+1);
		free_ui8matrix(cpy, i0-1,i1+1,j0-1,j1+1);


		for(int i = IMAGE_ID0; i < IMAGE_ID1; i++)
		{
			sprintf(buffer, "./car3/car_%i.pgm", i);
			MLoadPGM_ui8matrix(buffer, i0, i1, j0, j1, (uint8**)I);

			CHRONO_CYCLE_AARCH64(sigma_delta_iteration_prodcons(I,M,O,V,E,n,i0,i1,j0,j1),nb_cycles);

			// Ajout du bord à la sortie SD
			cpy= ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
			copy_ui8matrix_ui8matrix((uint8**)E, i0, i1, j0, j1, (uint8**)cpy);

			// Enchainement des morphos
			CHRONO_CYCLE_AARCH64(E1 = erosion_openmp(cpy, i0, i1, j0, j1) ,nb_cycles);
			CHRONO_CYCLE_AARCH64(D1 = dilatation_openmp(E1, i0, i1, j0, j1) ,nb_cycles);
			CHRONO_CYCLE_AARCH64(D2 = dilatation_openmp(D1, i0, i1, j0, j1) ,nb_cycles);
			CHRONO_CYCLE_AARCH64(E2 = erosion_openmp(D2, i0, i1, j0, j1) ,nb_cycles);

			free_ui8matrix(E1, i0-1,i1+1,j0-1,j1+1);
			free_ui8matrix(E2, i0-1,i1+1,j0-1,j1+1);
			free_ui8matrix(D2, i0-1,i1+1,j0-1,j1+1);
			free_ui8matrix(D1, i0-1,i1+1,j0-1,j1+1);
			free_ui8matrix(cpy, i0-1,i1+1,j0-1,j1+1);
		}
		printf("sigma_delta_iteration_prodcons+E-D-D-E_openmp\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_points/(nb_cycles + 0.0));
		sigma_delta_free(I,M,O,V,E,i0,i1,j0,j1);
	}

	free_ui8matrix(src, i0,i1,j0,j1);
	return 0;
}
