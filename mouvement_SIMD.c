#include "algo.h"
#include "vnrdef.h"
#include "vnrutil.h"

// Multiplication saturée
vuint8 vmulq_sat_u8(vuint8 a, vuint8 b) 
{
	// a = 2	2	2 	... 2
	// b = 1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16
	
	vuint8 zero = vmovq_n_u8(0);
	vuint16 max = vmovq_n_u16(255);
	// 2 ... 2
	vuint8 aGauche = vzip1q_u8(a, zero);
	vuint8 aDroite = vzip2q_u8(a, zero);
	
	// 1	2	3	4	5	6	7	8
	vuint8 bGauche = vzip1q_u8(b, zero);
	
	// 9	10	11	12	13	14	15	16
	vuint8 bDroite = vzip2q_u8(b, zero);

	// 2	0	4	0	6	0	8	0	10	0	12	0	14	0	16	0
	vuint8 resGauche = vreinterpretq_u8_u16(vminq_u16(vmulq_u16(vreinterpretq_u16_u8(aGauche), vreinterpretq_u16_u8(bGauche)), max));
	// 18	0	20	0	22	0	24	0	26	0	28	0	30	0	32	0
	vuint8 resDroite = vreinterpretq_u8_u16(vminq_u16(vmulq_u16(vreinterpretq_u16_u8(aDroite), vreinterpretq_u16_u8(bDroite)), max));

	// 2	4	6	8	10	12	14	16	18	20	22	24	26	28	30	32
	return vuzp1q_u8(resGauche, resDroite);
}

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
