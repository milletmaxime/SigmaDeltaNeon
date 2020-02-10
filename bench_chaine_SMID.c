#include "valgo.h"
#include <stdio.h>
#include "nrdef.h"
#include "nrutil.h"
#include "vnrdef.h"
#include "vnrutil.h"
#include "morpho_SIMD.h"
#include "mouvement_SIMD.h"

int main()
{
	printf("Algorithme\t\tCycles\t\tPoints\t\tcpp\n");

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

	// Pointeur pour sortie SD avec ajout d'un bord pour enchainer sur morpho
	vuint8** cpy;

	// Pour morphos
	vuint8 **E1, **D1, **D2, **E2;


	// SD et morpho sans optimisations
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		sigma_delta_SIMD_initialisation(src, &I ,&M , &O, &V, &E, i0, i1, j0, j1, &vi0, &vi1, &vj0, &vj1);

		nb_cycles = 0;

		CHRONO_CYCLE(sigma_delta_SIMD_iteration(I,M,O,V,E,n,vi0,vi1,vj0,vj1),nb_cycles);

		// Ajoute un bord à la sortie de SD
		cpy= (vuint8**)vui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
		copy_ui8matrix_ui8matrix((uint8**)E, i0, i1, j0, j1, (uint8**)cpy);

		// Enchainement des morphos
		CHRONO_CYCLE(E1 = erosion_SIMD(cpy, vi0, vi1, vj0, vj1) ,nb_cycles);
		CHRONO_CYCLE(D1 = dilatation_SIMD(E1, vi0, vi1, vj0, vj1) ,nb_cycles);
		CHRONO_CYCLE(D2 = dilatation_SIMD(D1, vi0, vi1, vj0, vj1) ,nb_cycles);
		CHRONO_CYCLE(E2 = erosion_SIMD(D2, vi0, vi1, vj0, vj1) ,nb_cycles);

		free_vui8matrix(E1, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(E2, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(D2, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(D1, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(cpy, vi0-1,vi1+1,vj0-1,vj1+1);


		for(int i = IMAGE_ID0; i < IMAGE_ID1; i++)
		{
			sprintf(buffer, "./car3/car_%i.pgm", i);
			MLoadPGM_ui8matrix(buffer, i0, i1, j0, j1, (uint8**)I);

			CHRONO_CYCLE(sigma_delta_SIMD_iteration(I,M,O,V,E,n,vi0, vi1, vj0, vj1),nb_cycles);

			// Ajout du bord à la sortie SD
			cpy= (vuint8**)ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
			copy_ui8matrix_ui8matrix((uint8**)E, i0, i1, j0, j1, (uint8**)cpy);

			// Enchainement des morphos
			CHRONO_CYCLE(E1 = erosion_SIMD(cpy, vi0, vi1, vj0, vj1) ,nb_cycles);
			CHRONO_CYCLE(D1 = dilatation_SIMD(E1, vi0, vi1, vj0, vj1) ,nb_cycles);
			CHRONO_CYCLE(D2 = dilatation_SIMD(D1, vi0, vi1, vj0, vj1) ,nb_cycles);
			CHRONO_CYCLE(E2 = erosion_SIMD(D2, vi0, vi1, vj0, vj1) ,nb_cycles);

			free_vui8matrix(E1, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(E2, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(D2, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(D1, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(cpy, vi0-1,vi1+1,vj0-1,vj1+1);
		}
		printf("sigma_delta_SIMD_iteration+E-D-D-E_SIMD\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		sigma_delta_SIMD_free(I,M,O,V,E,i0,i1,j0,j1);
	}
	
	// SD producteur consommateur et morpho avec réduction
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;

		sigma_delta_SIMD_initialisation(src, &I ,&M , &O, &V, &E, i0, i1, j0, j1, &vi0, &vi1, &vj0, &vj1);

		CHRONO_CYCLE(sigma_delta_SIMD_iteration_prodcons(I,M,O,V,E,n,vi0, vi1, vj0, vj1),nb_cycles);

		// Ajoute un bord à la sortie de SD
		cpy= (vuint8**)ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
		copy_ui8matrix_ui8matrix((uint8**)E, i0, i1, j0, j1, (uint8**)cpy);

		// Enchainement des morphos
		CHRONO_CYCLE(E1 = erosion_SIMD_reduction(cpy, vi0, vi1, vj0, vj1) ,nb_cycles);
		CHRONO_CYCLE(D1 = dilatation_SIMD_reduction(E1, vi0, vi1, vj0, vj1) ,nb_cycles);
		CHRONO_CYCLE(D2 = dilatation_SIMD_reduction(D1, vi0, vi1, vj0, vj1) ,nb_cycles);
		CHRONO_CYCLE(E2 = erosion_SIMD_reduction(D2, vi0, vi1, vj0, vj1) ,nb_cycles);

		free_vui8matrix(E1, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(E2, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(D2, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(D1, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(cpy, vi0-1,vi1+1,vj0-1,vj1+1);


		for(int i = IMAGE_ID0; i < IMAGE_ID1; i++)
		{
			sprintf(buffer, "./car3/car_%i.pgm", i);
			MLoadPGM_ui8matrix(buffer, i0, i1, j0, j1, (uint8**)I);

			CHRONO_CYCLE(sigma_delta_SIMD_iteration_prodcons(I,M,O,V,E,n,vi0, vi1, vj0, vj1),nb_cycles);

			// Ajout du bord à la sortie SD
			cpy= (vuint8**)ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
			copy_ui8matrix_ui8matrix((uint8**)E, i0, i1, j0, j1, (uint8**)cpy);

			// Enchainement des morphos
			CHRONO_CYCLE(E1 = erosion_SIMD_reduction(cpy, vi0, vi1, vj0, vj1) ,nb_cycles);
			CHRONO_CYCLE(D1 = dilatation_SIMD_reduction(E1, vi0, vi1, vj0, vj1) ,nb_cycles);
			CHRONO_CYCLE(D2 = dilatation_SIMD_reduction(D1, vi0, vi1, vj0, vj1) ,nb_cycles);
			CHRONO_CYCLE(E2 = erosion_SIMD_reduction(D2, vi0, vi1, vj0, vj1) ,nb_cycles);

			free_vui8matrix(E1, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(E2, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(D2, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(D1, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(cpy, vi0-1,vi1+1,vj0-1,vj1+1);
		}
		printf("sigma_delta_SIMD_iteration_prodcons+E-D-D-E_SIMD_reduction\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		sigma_delta_SIMD_free(I,M,O,V,E,i0,i1,j0,j1);
	}
	
	// SD producteur consommateur et morpho avec réduction et déroulage
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;

		sigma_delta_SIMD_initialisation(src, &I ,&M , &O, &V, &E, i0, i1, j0, j1, &vi0, &vi1, &vj0, &vj1);

		CHRONO_CYCLE(sigma_delta_SIMD_iteration_prodcons(I,M,O,V,E,n,vi0, vi1, vj0, vj1),nb_cycles);

		// Ajoute un bord à la sortie de SD
		cpy= (vuint8**)ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
		copy_ui8matrix_ui8matrix((uint8**)E, i0, i1, j0, j1, (uint8**)cpy);

		// Enchainement des morphos
		CHRONO_CYCLE(E1 = erosion_SIMD_reduction_deroulage(cpy, vi0, vi1, vj0, vj1) ,nb_cycles);
		CHRONO_CYCLE(D1 = dilatation_SIMD_reduction_deroulage(E1, vi0, vi1, vj0, vj1) ,nb_cycles);
		CHRONO_CYCLE(D2 = dilatation_SIMD_reduction_deroulage(D1, vi0, vi1, vj0, vj1) ,nb_cycles);
		CHRONO_CYCLE(E2 = erosion_SIMD_reduction_deroulage(D2, vi0, vi1, vj0, vj1) ,nb_cycles);

		free_vui8matrix(E1, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(E2, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(D2, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(D1, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(cpy, vi0-1,vi1+1,vj0-1,vj1+1);


		for(int i = IMAGE_ID0; i < IMAGE_ID1; i++)
		{
			sprintf(buffer, "./car3/car_%i.pgm", i);
			MLoadPGM_ui8matrix(buffer, i0, i1, j0, j1, (uint8**)I);

			CHRONO_CYCLE(sigma_delta_SIMD_iteration_prodcons(I,M,O,V,E,n,vi0, vi1, vj0, vj1),nb_cycles);

			// Ajout du bord à la sortie SD
			cpy= (vuint8**)ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
			copy_ui8matrix_ui8matrix((uint8**)E, i0, i1, j0, j1, (uint8**)cpy);

			// Enchainement des morphos
			CHRONO_CYCLE(E1 = erosion_SIMD_reduction_deroulage(cpy, vi0, vi1, vj0, vj1) ,nb_cycles);
			CHRONO_CYCLE(D1 = dilatation_SIMD_reduction_deroulage(E1, vi0, vi1, vj0, vj1) ,nb_cycles);
			CHRONO_CYCLE(D2 = dilatation_SIMD_reduction_deroulage(D1, vi0, vi1, vj0, vj1) ,nb_cycles);
			CHRONO_CYCLE(E2 = erosion_SIMD_reduction_deroulage(D2, vi0, vi1, vj0, vj1) ,nb_cycles);

			free_vui8matrix(E1, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(E2, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(D2, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(D1, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(cpy, vi0-1,vi1+1,vj0-1,vj1+1);
		}
		printf("sigma_delta_SIMD_iteration_prodcons+E-D-D-E_SIMD_reduction_deroulage\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));

		sigma_delta_SIMD_free(I,M,O,V,E,i0,i1,j0,j1);
	}

	// SD producteur consommateur openmp et morpho openmp
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;

		sigma_delta_SIMD_initialisation(src, &I ,&M , &O, &V, &E, i0, i1, j0, j1, &vi0, &vi1, &vj0, &vj1);

		CHRONO_CYCLE(sigma_delta_SIMD_iteration_prodcons_openmp(I,M,O,V,E,n,vi0, vi1, vj0, vj1),nb_cycles);

		// Ajoute un bord à la sortie de SD
		cpy= (vuint8**)ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
		copy_ui8matrix_ui8matrix((uint8**)E, i0, i1, j0, j1, (uint8**)cpy);

		// Enchainement des morphos
		CHRONO_CYCLE(E1 = erosion_SIMD_openmp(cpy, vi0, vi1, vj0, vj1) ,nb_cycles);
		CHRONO_CYCLE(D1 = dilatation_SIMD_openmp(E1, vi0, vi1, vj0, vj1) ,nb_cycles);
		CHRONO_CYCLE(D2 = dilatation_SIMD_openmp(D1, vi0, vi1, vj0, vj1) ,nb_cycles);
		CHRONO_CYCLE(E2 = erosion_SIMD_openmp(D2, vi0, vi1, vj0, vj1) ,nb_cycles);

		free_vui8matrix(E1, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(E2, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(D2, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(D1, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(cpy, vi0-1,vi1+1,vj0-1,vj1+1);


		for(int i = IMAGE_ID0; i < IMAGE_ID1; i++)
		{
			sprintf(buffer, "./car3/car_%i.pgm", i);
			MLoadPGM_ui8matrix(buffer, i0, i1, j0, j1, (uint8**)I);

			CHRONO_CYCLE(sigma_delta_SIMD_iteration_prodcons_openmp(I,M,O,V,E,n,vi0,vi1,vj0,vj1),nb_cycles);

			// Ajout du bord à la sortie SD
			cpy= (vuint8**)ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
			copy_ui8matrix_ui8matrix((uint8**)E, i0, i1, j0, j1, (uint8**)cpy);

			// Enchainement des morphos
			CHRONO_CYCLE(E1 = erosion_SIMD_openmp(cpy, vi0, vi1, vj0, vj1) ,nb_cycles);
			CHRONO_CYCLE(D1 = dilatation_SIMD_openmp(E1, vi0, vi1, vj0, vj1) ,nb_cycles);
			CHRONO_CYCLE(D2 = dilatation_SIMD_openmp(D1, vi0, vi1, vj0, vj1) ,nb_cycles);
			CHRONO_CYCLE(E2 = erosion_SIMD_openmp(D2, vi0, vi1, vj0, vj1) ,nb_cycles);

			free_vui8matrix(E1, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(E2, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(D2, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(D1, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(cpy, vi0-1,vi1+1,vj0-1,vj1+1);
		}
		printf("sigma_delta_SIMD_iteration_prodcons_openmp+E-D-D-E_simd_openmp\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));
		sigma_delta_SIMD_free(I,M,O,V,E,i0,i1,j0,j1);
	}

	// SD producteur consommateur openmp collapsed et morpho openmp
	for(int i = 0; i<NB_REPETITION_BENCH; i++)
	{
		nb_cycles = 0;

		sigma_delta_SIMD_initialisation(src, &I ,&M , &O, &V, &E, i0, i1, j0, j1, &vi0, &vi1, &vj0, &vj1);

		CHRONO_CYCLE(sigma_delta_SIMD_iteration_prodcons_openmp_collapsed(I,M,O,V,E,n,vi0, vi1, vj0, vj1),nb_cycles);

		// Ajoute un bord à la sortie de SD
		cpy= (vuint8**)ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
		copy_ui8matrix_ui8matrix((uint8**)E, i0, i1, j0, j1, (uint8**)cpy);

		// Enchainement des morphos
		CHRONO_CYCLE(E1 = erosion_SIMD_openmp(cpy, vi0, vi1, vj0, vj1) ,nb_cycles);
		CHRONO_CYCLE(D1 = dilatation_SIMD_openmp(E1, vi0, vi1, vj0, vj1) ,nb_cycles);
		CHRONO_CYCLE(D2 = dilatation_SIMD_openmp(D1, vi0, vi1, vj0, vj1) ,nb_cycles);
		CHRONO_CYCLE(E2 = erosion_SIMD_openmp(D2, vi0, vi1, vj0, vj1) ,nb_cycles);

		free_vui8matrix(E1, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(E2, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(D2, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(D1, vi0-1,vi1+1,vj0-1,vj1+1);
		free_vui8matrix(cpy, vi0-1,vi1+1,vj0-1,vj1+1);


		for(int i = IMAGE_ID0; i < IMAGE_ID1; i++)
		{
			sprintf(buffer, "./car3/car_%i.pgm", i);
			MLoadPGM_ui8matrix(buffer, i0, i1, j0, j1, (uint8**)I);

			CHRONO_CYCLE(sigma_delta_SIMD_iteration_prodcons_openmp_collapsed(I,M,O,V,E,n,vi0,vi1,vj0,vj1),nb_cycles);

			// Ajout du bord à la sortie SD
			cpy= (vuint8**)ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
			copy_ui8matrix_ui8matrix((uint8**)E, i0, i1, j0, j1, (uint8**)cpy);

			// Enchainement des morphos
			CHRONO_CYCLE(E1 = erosion_SIMD_openmp(cpy, vi0, vi1, vj0, vj1) ,nb_cycles);
			CHRONO_CYCLE(D1 = dilatation_SIMD_openmp(E1, vi0, vi1, vj0, vj1) ,nb_cycles);
			CHRONO_CYCLE(D2 = dilatation_SIMD_openmp(D1, vi0, vi1, vj0, vj1) ,nb_cycles);
			CHRONO_CYCLE(E2 = erosion_SIMD_openmp(D2, vi0, vi1, vj0, vj1) ,nb_cycles);

			free_vui8matrix(E1, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(E2, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(D2, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(D1, vi0-1,vi1+1,vj0-1,vj1+1);
			free_vui8matrix(cpy, vi0-1,vi1+1,vj0-1,vj1+1);
		}
		printf("sigma_delta_SIMD_iteration_prodcons_openmp_collapsed+E-D-D-E_simd_openmp\t\t%llu\t\t%u\t\t%f\n", nb_cycles, nb_points, nb_cycles/(nb_points + 0.0));
		sigma_delta_SIMD_free(I,M,O,V,E,i0,i1,j0,j1);
	}

	free_ui8matrix(src, i0,i1,j0,j1);
	return 0;
}
