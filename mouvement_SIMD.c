#include "valgo.h"
#include "vnrdef.h"
#include "vnrutil.h"
#include "nrutil.h"
#include "mouvement_SIMD.h"


/* ***************** *
 * SANS OPTIMISATION *
 * ***************** */

void sigma_delta_SIMD_iteration(vuint8** I, vuint8** M, vuint8** O, vuint8** V, vuint8** E, uint8 N, int vi0, int vi1, int vj0, int vj1)
{
	int x, y;
	vuint8 zero = vmovq_n_u8(0);
	vuint8 vVmax = vmovq_n_u8(Vmax);
	vuint8 vVmin = vmovq_n_u8(Vmin);
	vuint8 vN = vmovq_n_u8(N);

	vuint8 gt, lt, m, i, o, v, no;

	// Step 1
	for(x = vi0; x <= vi1; x++) {
		for(y = vj0; y <= vj1; y++) {
			m = M[x][y];
			i = I[x][y];
			// Si m < i alors on a +1 sinon on a zéro
			lt = vbslq_u8(vcltq_u8(m, i), vmovq_n_u8(1), zero);
			// Si m > i alors on a -1 sinon on a zéro
			gt =vbslq_u8(vcgtq_u8(m, i), vmovq_n_u8(-1), zero);
			M[x][y] = vaddq_u8(m, vaddq_u8(lt, gt));
		}
	}

	// Step 2
	for(x = vi0; x <= vi1; x++) {
		for(y = vj0; y <= vj1; y++) {
			m = M[x][y];
			i = I[x][y];
			O[x][y] = vbslq_u8(vcgeq_u8(m, i), vsubq_u8(m, i), vsubq_u8(i,m));	
		}
	}

	// Step 3
	for(x = vi0; x <= vi1; x++) {
		for(y = vj0; y <= vj1; y++) {
			v = V[x][y];
			o = O[x][y];
			no = vmulq_sat_u8(vN,o);

			lt = vbslq_u8(vcltq_u8(v, no), vmovq_n_u8(1), zero);  //v < no;
			gt = vbslq_u8(vcgtq_u8(v, no), vmovq_n_u8(-1), zero);  //v > no;

			v = vaddq_u8(v, vaddq_u8(lt, gt));

			// Si v < Vmax alors on prend v sinon on prend Vmax
			v = vbslq_u8(vcltq_u8(v, vVmax), v, vVmax);
			// Si v < Vmin alors on prend Vmin sinon on prend v
			v = vbslq_u8(vcltq_u8(v, vVmin), vVmin, v);

			V[x][y] = v;
		}
	}
	
	// Step 4
	for(x = vi0; x <= vi1; x++) {
		for(y = vj0; y <= vj1; y++) {
			v = V[x][y];
			o = O[x][y];
			E[x][y] = vbslq_u8(vcltq_u8(o, v), vVmin, vVmax);
		}
	}
}



/* ******************************************* *
 * PRODUCTEUR CONSOMMATEUR (FUSION DE BOUCLES) *
 * ******************************************* */

// Corps de la double boucle pour une itération
static inline void sigma_delta_SIMD_prodcons_loop(int x, int y, vuint8** I, vuint8** M, vuint8** O, vuint8** V, vuint8** E, vuint8 vN, vuint8 vVmax, vuint8 vVmin, vuint8 zero)
{
	vuint8 gt, lt, m, i, o, v, no;

	// Step 1
	m = M[x][y];
	i = I[x][y];
	// Si m < i alors on a +1 sinon on a zéro
	lt = vbslq_u8(vcltq_u8(m, i), vmovq_n_u8(1), zero);
	// Si m > i alors on a -1 sinon on a zéro
	gt =vbslq_u8(vcgtq_u8(m, i), vmovq_n_u8(-1), zero);
	m = vaddq_u8(m, vaddq_u8(lt, gt));

	// Step 2
	o = vbslq_u8(vcgeq_u8(m, i), vsubq_u8(m, i), vsubq_u8(i,m));	

	// Step 3
	v = V[x][y];
	no = vmulq_sat_u8(vN,o);
	
	lt = vbslq_u8(vcltq_u8(v, no), vmovq_n_u8(1), zero);  //v < no;
	gt = vbslq_u8(vcgtq_u8(v, no), vmovq_n_u8(-1), zero);  //v > no;
	
	v = vaddq_u8(v, vaddq_u8(lt, gt));
	
	// Si v < Vmax alors on prend v sinon on prend Vmax
	v = vbslq_u8(vcltq_u8(v, vVmax), v, vVmax);
	// Si v < Vmin alors on prend Vmin sinon on prend v
	v = vbslq_u8(vcltq_u8(v, vVmin), vVmin, v);

	// Step 4
	E[x][y] = vbslq_u8(vcltq_u8(o, v), vVmin, vVmax);

	M[x][y] = m;
	O[x][y] = o;
	V[x][y] = v;
}

void sigma_delta_SIMD_iteration_prodcons(vuint8** I, vuint8** M, vuint8** O, vuint8** V, vuint8** E, uint8 N, int vi0, int vi1, int vj0, int vj1)
{
	vuint8 zero = vmovq_n_u8(0);
	vuint8 vVmax = vmovq_n_u8(Vmax);
	vuint8 vVmin = vmovq_n_u8(Vmin);
	vuint8 vN = vmovq_n_u8(N);

	for(int x = vi0; x <= vi1; x++) {
		for(int y = vj0; y <= vj1; y++) {
			sigma_delta_SIMD_prodcons_loop(x,y,I,M,O,V,E,vN,vVmax, vVmin, zero);
		}
	}
}

void sigma_delta_SIMD_iteration_prodcons_openmp(vuint8** I, vuint8** M, vuint8** O, vuint8** V, vuint8** E, uint8 N, int vi0, int vi1, int vj0, int vj1)
{
	vuint8 zero = vmovq_n_u8(0);
	vuint8 vVmax = vmovq_n_u8(Vmax);
	vuint8 vVmin = vmovq_n_u8(Vmin);
	vuint8 vN = vmovq_n_u8(N);

	#pragma omp parallel for
	for(int x = vi0; x <= vi1; x++) {
		for(int y = vj0; y <= vj1; y++) {
			sigma_delta_SIMD_prodcons_loop(x,y,I,M,O,V,E,vN,vVmax, vVmin, zero);
		}
	}
}

void sigma_delta_SIMD_iteration_prodcons_openmp_collapsed(vuint8** I, vuint8** M, vuint8** O, vuint8** V, vuint8** E, uint8 N, int vi0, int vi1, int vj0, int vj1)
{
	vuint8 zero = vmovq_n_u8(0);
	vuint8 vVmax = vmovq_n_u8(Vmax);
	vuint8 vVmin = vmovq_n_u8(Vmin);
	vuint8 vN = vmovq_n_u8(N);

	#pragma omp parallel for collapse(2)
	for(int x = vi0; x <= vi1; x++) {
		for(int y = vj0; y <= vj1; y++) {
			sigma_delta_SIMD_prodcons_loop(x,y,I,M,O,V,E,vN,vVmax, vVmin, zero);
		}
	}
}


/* ***************************** *
 * INITIALISATION et DESTRUCTION *
 * ***************************** */


void sigma_delta_SIMD_initialisation(uint8** src, vuint8*** I, vuint8*** M, vuint8*** O, vuint8*** V, vuint8*** E, int i0, int i1, int j0, int j1, int* vi0, int* vi1, int* vj0, int* vj1)
{
	*I = (vuint8**) ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
	*M = (vuint8**) ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
	*O = (vuint8**) ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
	*V = (vuint8**) ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);
	*E = (vuint8**) ui8matrix(i0 -1, i1 +1, j0 -1, j1+1);


	copy_ui8matrix_ui8matrix(src, i0, i1, j0, j1, (uint8**)*I);
	copy_ui8matrix_ui8matrix(src, i0, i1, j0, j1, (uint8**)*M);

	s2v(i0, i1, j0, j1, card_vuint8(), vi0, vi1, vj0, vj1);

	vuint8 vVmin = vmovq_n_u8(Vmin);
	for(int i = *vi0;i <= *vi1;i++) for(int j = *vj0;j <= *vj1;j++) (*V)[i][j] = vVmin;
}

void sigma_delta_SIMD_free(vuint8** I, vuint8** M, vuint8** O, vuint8** V, vuint8** E, int i0, int i1, int j0, int j1)
{
	free_ui8matrix((uint8**)I, i0 -1, i1 +1, j0 -1, j1 +1);
	free_ui8matrix((uint8**)M, i0 -1, i1 +1, j0 -1, j1 +1);
	free_ui8matrix((uint8**)O, i0 -1, i1 +1, j0 -1, j1 +1);
	free_ui8matrix((uint8**)V, i0 -1, i1 +1, j0 -1, j1 +1);
	free_ui8matrix((uint8**)E, i0 -1, i1 +1, j0 -1, j1 +1);
}

/*
#include <stdio.h>
int main() {
	
	vuint8 **I, **M, **O, **V, **E;
	uint8 n = 3;
	int i0, i1, j0, j1;
	int vi0, vi1, vj0, vj1;
	
	char buffer[100];
	
	uint8** src = LoadPGM_ui8matrix("./car3/car_3000.pgm", &i0, &i1, &j0, &j1);

	sigma_delta_SIMD_initialisation(src, &I ,&M , &O, &V, &E, i0, i1, j0, j1, &vi0, &vi1, &vj0, &vj1);
	sigma_delta_SIMD_iteration_prodcons_openmp_collapsed(I,M,O,V,E,n,vi0,vi1,vj0,vj1);
	SavePGM_ui8matrix((uint8**)E, i0, i1, j0, j1, "./out_simd/car_3000.pgm");
	
	for(int i = 3001; i < 3200; i++)
	{
		sprintf(buffer, "./car3/car_%i.pgm", i);
		MLoadPGM_ui8matrix(buffer, i0, i1, j0, j1, (uint8**)I);
		
		sigma_delta_SIMD_iteration_prodcons_openmp_collapsed(I,M,O,V,E,n,vi0,vi1,vj0,vj1);
		
		sprintf(buffer, "./out_simd/car_%i.pgm", i);
		SavePGM_ui8matrix((uint8**)E, i0, i1, j0, j1, buffer);
	}
	
	sigma_delta_SIMD_free(I,M,O,V,E,i0,i1,j0,j1);
	free_ui8matrix(src, i0,i1,j0,j1);

	return 0;
}
*/
