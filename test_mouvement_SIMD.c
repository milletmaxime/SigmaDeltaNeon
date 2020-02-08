#include "algo.h"
#include "vnrdef.h"
#include "vnrutil.h"

vuint8 vmulq_sat_u8(vuint8 a, vuint8 b);

vuint8 SD_test_simd_step1(vuint8 m, vuint8 i) {
	vuint8 zero = vmovq_n_u8(0);
	
	// Si m < i alors on a +1 sinon on a zéro
	vuint8 lt = vbslq_u8(vcltq_u8(m, i), vmovq_n_u8(1), zero);
	// Si m > i alors on a -1 sinon on a zéro
	vuint8 gt =vbslq_u8(vcgtq_u8(m, i), vmovq_n_u8(-1), zero);
	
	return vaddq_u8(m, vaddq_u8(lt, gt));
}

vuint8 SD_test_simd_step2(vuint8 m, vuint8 i) {
	// On veut O = |M - I|
	
	// Si m >= i alors on fait m - i sinon on fait i - m
	return vbslq_u8(vcgeq_u8(m, i), vsubq_u8(m, i), vsubq_u8(i,m));	
}

vuint8 SD_test_simd_step3(vuint8 v, vuint8 o, vuint8 n) {
	vuint8 zero = vmovq_n_u8(0);
	vuint8 vVmax = vmovq_n_u8(Vmax);
	vuint8 vVmin = vmovq_n_u8(Vmin);
	
	vuint8 no = vmulq_sat_u8(n,o);
	
	vuint8 lt = vbslq_u8(vcltq_u8(v, no), vmovq_n_u8(1), zero);  //v < no;
	vuint8 gt = vbslq_u8(vcgtq_u8(v, no), vmovq_n_u8(-1), zero);  //v > no;
	
	v = vaddq_u8(v, vaddq_u8(lt, gt));
	
	// Si v < Vmax alors on prend v sinon on prend Vmax
	v = vbslq_u8(vcltq_u8(v, vVmax), v, vVmax);
	// Si v < Vmin alors on prend Vmin sinon on prend v
	v = vbslq_u8(vcltq_u8(v, vVmin), vVmin, v);

	return v;
}


vuint8 SD_test_simd_step4(vuint8 o, vuint8 v)
{
	return vbslq_u8(vcltq_u8(o, v), vmovq_n_u8(0), vmovq_n_u8(1));
}
