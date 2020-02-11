#include "valgo.h"
#include "vnrdef.h"
#include "vnrutil.h"
#include <stdio.h>

vuint8 dilatation_SIMD_test(vuint8 l1, vuint8 l2, vuint8 l3) // dilatation = max
{
	vuint8 bord = vmovq_n_u8(Vmin); // pour simuler les bords

	vuint8 v1 = vextq_u8(bord, l1, 15);
	vuint8 v2 = vextq_u8(bord, l2, 15);
	vuint8 v3 = vextq_u8(bord, l3, 15);
	vuint8 v4 = l1;

	vuint8 v9 = l2;

	vuint8 v5 = l3;
	vuint8 v6 = vextq_u8(l1, bord, 1);
	vuint8 v7 = vextq_u8(l2, bord, 1);
	vuint8 v8 = vextq_u8(l3, bord, 1);

	return vmaxq_u8(vmaxq_u8(vmaxq_u8(vmaxq_u8(v1 ,v2) ,vmaxq_u8(v3 ,v4)) ,vmaxq_u8(vmaxq_u8(v5 ,v6) ,vmaxq_u8(v7 ,v8))), v9);
}

vuint8 erosion_SIMD_test(vuint8 l1, vuint8 l2, vuint8 l3) // erosion = min
{
	vuint8 bord = vmovq_n_u8(Vmax); // pour simuler les bords

	vuint8 v1 = vextq_u8(bord, l1, 15); // bord gauche
	vuint8 v2 = vextq_u8(bord, l2, 15);
	vuint8 v3 = vextq_u8(bord, l3, 15);

	vuint8 v4 = l1;

	vuint8 v9 = l2;

	vuint8 v5 = l3;

	vuint8 v6 = vextq_u8(l1, bord, 1); // bord droit
	vuint8 v7 = vextq_u8(l2, bord, 1);
	vuint8 v8 = vextq_u8(l3, bord, 1);

	return vminq_u8(vminq_u8(vminq_u8(vminq_u8(v1 ,v2) ,vminq_u8(v3 ,v4)) ,vminq_u8(vminq_u8(v5 ,v6) ,vminq_u8(v7 ,v8))), v9);
}

int main()
{
	printf("-----------------------\nTests morpho simd\n-----------------------\n");
	uint8 l1[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
	vuint8 vl1 = vld1q_u8(l1);
	uint8 l2[16] = {1,2,3,4,5,6,7,0,9,10,11,12,13,14,15,16};
	vuint8 vl2 = vld1q_u8(l2);
	uint8 l3[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
	vuint8 vl3 = vld1q_u8(l3);
	
	display_vui8vector(&vl1,0,0,"%u ","l1 : ");
	display_vui8vector(&vl2,0,0,"%u ","l2 : ");
	display_vui8vector(&vl3,0,0,"%u ","l3 : ");
	
	vuint8 erosion = erosion_SIMD_test(vl1,vl2,vl3);
	display_vui8vector(&erosion,0,0,"%u ","erosion :\n");
	
	
	vuint8 dilatation = dilatation_SIMD_test(vl1,vl2,vl3);
	display_vui8vector(&dilatation,0,0,"%u ","dilatation :\n");
	
	printf("\n\n");
	
	uint8 l12[16] = {9,9,9,5,9,9,5,5,9,5,5,5,9,9,9,5};
	vuint8 vl12 = vld1q_u8(l12);
	uint8 l22[16] = {9,9,9,5,9,9,5,5,9,5,5,5,9,9,9,5};
	vuint8 vl22 = vld1q_u8(l22);
	uint8 l32[16] = {9,9,9,5,9,9,5,5,9,5,5,5,9,9,9,5};
	vuint8 vl32 = vld1q_u8(l32);
	
	display_vui8vector(&vl12,0,0,"%u ","l1 : ");
	display_vui8vector(&vl22,0,0,"%u ","l2 : ");
	display_vui8vector(&vl32,0,0,"%u ","l3 : ");
	
	vuint8 erosion2 = erosion_SIMD_test(vl12,vl22,vl32);
	display_vui8vector(&erosion2,0,0,"%u ","erosion :\n");
	
	
	vuint8 dilatation2 = dilatation_SIMD_test(vl12,vl22,vl32);
	display_vui8vector(&dilatation2,0,0,"%u ","dilatation :\n");


	return 0;
}
