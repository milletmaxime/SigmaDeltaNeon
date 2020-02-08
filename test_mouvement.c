#include "algo.h"
#include "nrdef.h"
#include "nrutil.h"

uint8 SD_test_scalaire_step1(uint8 m, uint8 i) {
	uint8 lt = m < i;
	uint8 gt = m > i;
	
	return m + (1 & lt) - (1 & gt);
}

uint8 SD_test_scalaire_step2(uint8 m, uint8 i) {
	int o = m - i; // int pour avoir un nombre negatif et que le test puisse retourner 0
	uint8 gtz = -(o >= 0); //-(...) pour avoir un masque complet 0xFFFF
	return (gtz & o) | ((~gtz) & -o);
}

uint8 SD_test_scalaire_step3(uint8 v, uint8 o, uint8 n) {
	uint8 no = n * o;
	uint8 lt = v < no;
	uint8 gt = v > no;
	
	v = v + (1 & lt) - (1 & gt);
	
	// clamp to [Vmin, Vmax]
	lt = -(v < Vmax); // - pour avoir le masque complet
	v = (lt & v) | (~lt & Vmax);

	gt = -(v > Vmin); // - pour avoir le masque complet
	v = (gt & v) | (~gt & Vmin);

	return v;
}

uint8 SD_test_scalaire_step4(uint8 o, uint8 v)
{
	uint8 lt = o < v;
	return (lt & 0) | (~lt & 1);
}

