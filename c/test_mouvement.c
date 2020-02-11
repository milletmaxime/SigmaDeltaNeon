#include "algo.h"
#include "nrdef.h"
#include "nrutil.h"
#include <stdio.h>
#include <assert.h>

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

int main()
{
printf("-----------------------\nTests mouvement scalaire\n-----------------------\n");
	
	uint8 step1_1 = SD_test_scalaire_step1(100, 150);
	assert(step1_1 == 101);
	uint8 step1_2 = SD_test_scalaire_step1(100, 50);
	assert(step1_2 == 99);
	uint8 step1_3 = SD_test_scalaire_step1(100,100);
	assert(step1_3 == 100);
	printf("SD_test_scalaire_step1\tOK\n");
	
	
	uint8 step2_1 = SD_test_scalaire_step2(50, 100);
	assert(step2_1 == 50);
	uint8 step2_2 = SD_test_scalaire_step2(100, 50);
	assert(step2_2 == 50);
	uint8 step2_3 = SD_test_scalaire_step2(100,100);
	assert(step2_3 == 0);
	printf("SD_test_scalaire_step2\tOK\n");
	
	uint8 step3_1 = SD_test_scalaire_step3(100, 150, 1);
	assert(step3_1 == 101);
	uint8 step3_2 = SD_test_scalaire_step3(100, 50, 1);
	assert(step3_2 == 99);
	uint8 step3_3 = SD_test_scalaire_step3(100,100, 1);
	assert(step3_3 == 100);
	
	uint8 step3_4 = SD_test_scalaire_step3(254,255, 1);
	assert(step3_4 == Vmax);
	
	uint8 step3_5 = SD_test_scalaire_step3(1,0, 1);
	assert(step3_5 == Vmin);
	printf("SD_test_scalaire_step3\tOK\n");
	
	uint8 step4_1 = SD_test_scalaire_step4(50, 100);
	assert(step4_1 == 0);
	uint8 step4_2 = SD_test_scalaire_step4(100, 50);
	assert(step4_2 == 1);
	uint8 step4_3 = SD_test_scalaire_step4(100,100);
	assert(step4_3 == 1);
	printf("SD_test_scalaire_step4\tOK\n");
	return 0;
}
