#include "algo.h"
#include "nrdef.h"
#include "nrutil.h"

void sigma_delta_iteration(uint8** I, uint8** M, uint8** O, uint8** V, uint8** E, uint8 N, int i0, int i1, int j0, int j1)
{
	int x, y;
	uint8 gtz, gt, lt, m, i, v, no;
	int o;
	
	// Step 1
	for(x = i0; x <= i1; x++) {
		for(y = j0; y <= j1; y++) {
			m = M[x][y];
			i = I[x][y];
			M[x][y] = m + (1 & (m < i)) - (1 & (m > i));
		}
	}
	
	// Step 2
	for(x = i0; x <= i1; x++) {
		for(y = j0; y <= j1; y++) {
			o = M[x][y] - I[x][y]; // int pour avoir un nombre negatif et que le test puisse retourner 0
			gtz = -(o >= 0); //-(...) pour avoir un masque complet 0xFFFF
			O[x][y] = (gtz & o) | ((~gtz) & -o);
		}
	}
	
	// Step 3
	for(x = i0; x <= i1; x++) {
		for(y = j0; y <= j1; y++) {
			v = V[x][y];
			o = O[x][y];
			
			no = N * o;
			lt = v < no;
			gt = v > no;
			
			v = v + (1 & lt) - (1 & gt);
			
			// clamp to [Vmin, Vmax]
			lt = -(v < Vmax); // - pour avoir le masque complet
			v = (lt & v) | (~lt & Vmax);

			gt = -(v > Vmin); // - pour avoir le masque complet
			v = (gt & v) | (~gt & Vmin);
			
			V[x][y] = v;
		}
	}
	
	// Step 4
	for(x = i0; x <= i1; x++) {
		for(y = j0; y <= j1; y++) {
			lt = O[x][y] < V[x][y];
			E[x][y] = 255 * ((lt & 0) | (~lt & 1));
		}
	}
}


static inline void sigma_delta_prodcons_loop(int x, int y, uint8** I, uint8** M, uint8** O, uint8** V, uint8** E, uint8 N)
{
	uint8 gtz, gt, lt, m, i, v, no;
	int o;
	
	// Step 1
	m = M[x][y];
	i = I[x][y];
	m = m + (1 & (m < i)) - (1 & (m > i));

	// Step 2
	o = M[x][y] - I[x][y]; // int pour avoir un nombre negatif et que le test puisse retourner 0
	gtz = -(o >= 0); //-(...) pour avoir un masque complet 0xFFFF
	o = (gtz & o) | ((~gtz) & -o);

	// Step 3
	v = V[x][y];

	no = N * o;
	lt = v < no;
	gt = v > no;

	v = v + (1 & lt) - (1 & gt);

	// clamp to [Vmin, Vmax]
	lt = -(v < Vmax); // - pour avoir le masque complet
	v = (lt & v) | (~lt & Vmax);

	gt = -(v > Vmin); // - pour avoir le masque complet
	v = (gt & v) | (~gt & Vmin);

	// Step 4
	lt = O[x][y] < V[x][y];
	E[x][y] = 255 * ((lt & 0) | (~lt & 1));
	
	M[x][y] = m;
	O[x][y] = o;
	V[x][y] = v;
}

void sigma_delta_iteration_prodcons(uint8** I, uint8** M, uint8** O, uint8** V, uint8** E, uint8 N, int i0, int i1, int j0, int j1)
{	
	for(int x = i0; x <= i1; x++) {
		for(int y = j0; y <= j1; y++) {
			sigma_delta_prodcons_loop(x,y,I,M,O,V,E,N);
		}
	}
}

void sigma_delta_iteration_prodcons_openmp(uint8** I, uint8** M, uint8** O, uint8** V, uint8** E, uint8 N, int i0, int i1, int j0, int j1)
{
	#pragma omp parallel for
	for(int x = i0; x <= i1; x++) {
		for(int y = j0; y <= j1; y++) {
			sigma_delta_prodcons_loop(x,y,I,M,O,V,E,N);
		}
	}
}

void sigma_delta_iteration_prodcons_openmp_collapsed(uint8** I, uint8** M, uint8** O, uint8** V, uint8** E, uint8 N, int i0, int i1, int j0, int j1)
{
	#pragma omp parallel for collapse(2)
	for(int x = i0; x <= i1; x++) {
		for(int y = j0; y <= j1; y++) {
			sigma_delta_prodcons_loop(x,y,I,M,O,V,E,N);
		}
	}
}


void sigma_delta_initialisation(uint8** src, uint8*** I, uint8*** M, uint8*** O, uint8*** V, uint8*** E, int i0, int i1, int j0, int j1)
{
	*I = ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
	*M = ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
	*O = ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
	*V = ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
	*E = ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);

	copy_ui8matrix_ui8matrix(src, i0, i1, j0, j1, *M);
	copy_ui8matrix_ui8matrix(src, i0, i1, j0, j1, *I);

	for(int i = i0;i<=i1;i++) for(int j = j0;j<=j1;j++) (*V)[i][j] = Vmin;
}

void sigma_delta_free(uint8** I, uint8** M, uint8** O, uint8** V, uint8** E, int i0, int i1, int j0, int j1)
{
	free_ui8matrix(I, i0 -1, i1 +1, j0 -1, j1 +1);
	free_ui8matrix(M, i0 -1, i1 +1, j0 -1, j1 +1);
	free_ui8matrix(O, i0 -1, i1 +1, j0 -1, j1 +1);
	free_ui8matrix(V, i0 -1, i1 +1, j0 -1, j1 +1);
	free_ui8matrix(E, i0 -1, i1 +1, j0 -1, j1 +1);
}

/*
#include <stdio.h>
int main() {

	uint8 **I, **M, **O, **V, **E;
	uint8 n = 3;
	int i0, i1, j0, j1;
	char buffer[100];
	
	uint8** src = LoadPGM_ui8matrix("./car3/car_3000.pgm", &i0, &i1, &j0, &j1);

	sigma_delta_initialisation(src, &I ,&M , &O, &V, &E, i0, i1, j0, j1);
	sigma_delta_iteration_prodcons_openmp(I,M,O,V,E,n,i0,i1,j0,j1);
	SavePGM_ui8matrix((uint8**)E, i0, i1, j0, j1, "./out/car_3000.pgm");

	for(int i = 3001; i < 3200; i++)
	{
		sprintf(buffer, "./car3/car_%i.pgm", i);
		MLoadPGM_ui8matrix(buffer, i0, i1, j0, j1, (uint8**)I);
		
		sigma_delta_iteration_prodcons_openmp(I,M,O,V,E,n,i0,i1,j0,j1);
		
		sprintf(buffer, "./out/car_%i.pgm", i);
		SavePGM_ui8matrix((uint8**)E, i0, i1, j0, j1, buffer);
	}
	
	sigma_delta_free(I,M,O,V,E,i0,i1,j0,j1);
	free_ui8matrix(src, i0,i1,j0,j1);

	return 0;
}
*/
