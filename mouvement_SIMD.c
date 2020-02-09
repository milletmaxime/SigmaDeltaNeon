#include "valgo.h"
#include "vnrdef.h"
#include "vnrutil.h"

void sigma_delta_SIMD_initialisation()
{
	 // TODO
}

void sigma_delta_SIMD_iterations(vuint8** I, vuint8** M, vuint8** V, vuint8** O, vuint8** E, unsigned N, unsigned width, unsigned height)
{
	int x, y;
	vuint8 zero = vmovq_n_u8(0);
	vuint8 vVmax = vmovq_n_u8(Vmax);
	vuint8 vVmin = vmovq_n_u8(Vmin);
	vuint8 vN = vmovq_n_u8(N);
	
	// Step 1
	for(x = 0; x < width; x++) {
		for(y = 0; y < height; y++) {
			vuint8 m = M[x][y];
			vuint8 i = I[x][y];
			// Si m < i alors on a +1 sinon on a zéro
			vuint8 lt = vbslq_u8(vcltq_u8(m, i), vmovq_n_u8(1), zero);
			// Si m > i alors on a -1 sinon on a zéro
			vuint8 gt =vbslq_u8(vcgtq_u8(m, i), vmovq_n_u8(-1), zero);
			M[x][y] = vaddq_u8(m, vaddq_u8(lt, gt));
		}
	}

	// Step 2
	for(x = 0; x < width; x++) {
		for(y = 0; y < height; y++) {
			vuint8 m = M[x][y];
			vuint8 i = I[x][y];
			O[x][y] = vbslq_u8(vcgeq_u8(m, i), vsubq_u8(m, i), vsubq_u8(i,m));	
		}
	}
	
	// Step 3
	for(x = 0; x < width; x++) {
		for(y = 0; y < height; y++) {
			vuint8 v = V[x][y];
			vuint8 o = O[x][y];
			vuint8 no = vmulq_sat_u8(vN,o);
			
			vuint8 lt = vbslq_u8(vcltq_u8(v, no), vmovq_n_u8(1), zero);  //v < no;
			vuint8 gt = vbslq_u8(vcgtq_u8(v, no), vmovq_n_u8(-1), zero);  //v > no;
			
			v = vaddq_u8(v, vaddq_u8(lt, gt));
			
			// Si v < Vmax alors on prend v sinon on prend Vmax
			v = vbslq_u8(vcltq_u8(v, vVmax), v, vVmax);
			// Si v < Vmin alors on prend Vmin sinon on prend v
			v = vbslq_u8(vcltq_u8(v, vVmin), vVmin, v);
			
			V[x][y] = v;
		}
	}
	
	// Step 4
	for(x = 0; x < width; x++) {
		for(y = 0; y < height; y++) {
			vuint8 v = V[x][y];
			vuint8 o = O[x][y];
			E[x][y] = vbslq_u8(vcltq_u8(o, v), vVmin, vVmax);
		}
	}
}
