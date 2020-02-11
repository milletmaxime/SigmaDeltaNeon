#include "valgo.h"
#include "vnrdef.h"
#include "vnrutil.h"
#include <stdio.h>
#include <assert.h>

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

int main()
{	
	vuint8 r50 = vmovq_n_u8(50);
	vuint8 r100 = vmovq_n_u8(100);
	vuint8 r150 = vmovq_n_u8(150);
	
	vuint8 r101 = vmovq_n_u8(101);
	vuint8 r99 = vmovq_n_u8(99);
	vuint8 r0 = vmovq_n_u8(0);
	vuint8 r1 = vmovq_n_u8(1);
	vuint8 r3 = vmovq_n_u8(3);
	vuint8 r2 = vmovq_n_u8(2);
	vuint8 rVmin = vmovq_n_u8(Vmin);
	vuint8 rVmin_m1 = vmovq_n_u8(Vmin - 1);
	vuint8 rVmax = vmovq_n_u8(Vmax);
	
	printf("-----------------------\nTests multiplication saturée SIMD\n-----------------------\n");
	
	display_vuint8(r99, "%u ", " ");
	display_vuint8(r1, "%u ", "*");
	display_vuint8(vmulq_sat_u8(r99,r1), "%u ", "=");
	display_vuint8(r99, "%u ", "=");
	printf("\n");
	
	display_vuint8(r99, "%u ", " ");
	display_vuint8(r1, "%u ", "*");
	display_vuint8(vmulq_sat_u8(r99,r2), "%u ", "=");
	display_vuint8(vmovq_n_u8(99*2), "%u ", "=");
	printf("\n");
	
	display_vuint8(r99, "%u ", " ");
	display_vuint8(r3, "%u ", "*");
	display_vuint8(vmulq_sat_u8(r99,r3), "%u ", "=");
	display_vuint8(rVmax, "%u ", "=");

	
	printf("-----------------------\nTests mouvement SIMD\n-----------------------\n");
	

	

	uint8 t1[16] = {1,2,3,4,5,0,0,0,0,0,0,0,0,0,0,0};
	uint8 t2[16] = {1,1,4,2,10,0,0,0,0,0,0,0,0,0,0,0};
	vuint8 rt1 = vld1q_u8(t1);
	vuint8 rt2 = vld1q_u8(t2);
	
	
	uint8 tres1[16] = {1,1,4,3,6,0,0,0,0,0,0,0,0,0,0,0};
	vuint8 res1 = vld1q_u8(tres1);
	
	uint8 tres2[16] = {0,1,1,2,5,0,0,0,0,0,0,0,0,0,0,0};
	vuint8 res2 = vld1q_u8(tres2);
	
	uint8 tres3[16] = {1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1};
	vuint8 res3 = vld1q_u8(tres3);
	
	vuint8 step1_1 = SD_test_simd_step1(r100, r150);
	display_vuint8(step1_1, "%u ", "");
	display_vuint8(r101, "%u ", "");
	printf("\n");
	vuint8 step1_2 = SD_test_simd_step1(r100, r50);
	display_vuint8(step1_2, "%u ", "");
	display_vuint8(r99, "%u ", "");
	printf("\n");
	vuint8 step1_3 = SD_test_simd_step1(r100,r100);
	display_vuint8(step1_3, "%u ", "");
	display_vuint8(r100, "%u ", "");
	printf("\n");
	vuint8 step1_4 = SD_test_simd_step1(rt1,rt2);
	display_vuint8(step1_4, "%u ", "");
	display_vuint8(res1, "%u ", "");


	vuint8 step2_1 = SD_test_simd_step2(r50, r100);
	display_vuint8(step2_1, "%u ", "");
	display_vuint8(r50, "%u ", "");
	printf("\n");
	vuint8 step2_2 = SD_test_simd_step2(r100, r50);
	display_vuint8(step2_2, "%u ", "");
	display_vuint8(r50, "%u ", "");
	printf("\n");
	vuint8 step2_3 = SD_test_simd_step2(r100,r100);
	display_vuint8(step2_3, "%u ", "");
	display_vuint8(r0, "%u ", "");
	printf("\n");
	vuint8 step2_4 = SD_test_simd_step2(rt1,rt2);
	display_vuint8(step2_4, "%u ", "");
	display_vuint8(res2, "%u ", "");
	
	
	vuint8 step3_1 = SD_test_simd_step3(r100, r150, r1);
	display_vuint8(step3_1, "%u ", "");
	display_vuint8(r101, "%u ", "");
	printf("\n");
	vuint8 step3_2 = SD_test_simd_step3(r100, r50, r1);
	display_vuint8(step3_2, "%u ", "");
	display_vuint8(r99, "%u ", "");
	printf("\n");
	vuint8 step3_3 = SD_test_simd_step3(r100,r100, r1);
	display_vuint8(step3_3, "%u ", "");
	display_vuint8(r100, "%u ", "");
	printf("\n");
	vuint8 step3_vmax = SD_test_simd_step3(rVmax,rVmax, r1);
	display_vuint8(step3_vmax, "%u ", "");
	display_vuint8(rVmax, "%u ", "");
	printf("\n");
	vuint8 step3_vmin = SD_test_simd_step3(rVmin,rVmin_m1, r1);
	display_vuint8(step3_vmin, "%u ", "");
	display_vuint8(rVmin, "%u ", "");
	printf("\n");

	
	vuint8 step4_1 = SD_test_simd_step4(r50, r100);
	display_vuint8(step4_1, "%u ", "");
	display_vuint8(r0, "%u ", "");
	printf("\n");
	vuint8 step4_2 = SD_test_simd_step4(r100, r50);
	display_vuint8(step4_2, "%u ", "");
	display_vuint8(r1, "%u ", "");
	printf("\n");
	vuint8 step4_3 = SD_test_simd_step4(r100,r100);
	display_vuint8(step4_3, "%u ", "");
	display_vuint8(r1, "%u ", "");
	printf("\n");
	vuint8 step4_4 = SD_test_simd_step4(rt1,rt2);
	display_vuint8(step4_4, "%u ", "");
	display_vuint8(res3, "%u ", "");
	printf("\n");
	return 0;
}
