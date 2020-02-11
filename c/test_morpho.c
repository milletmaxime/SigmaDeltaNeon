#include "nrdef.h"
#include "nrutil.h"
#include <stdio.h>

uint8 dilatation_test(	uint8 v00, uint8 v01, uint8 v02,
						uint8 v10, uint8 v11, uint8 v12,
						uint8 v20, uint8 v21, uint8 v22)
{
	uint8 cmp, max;
	cmp = -((v00 - v01) > 0) ;	max = (cmp & v00) | (~cmp & v01);
	cmp = -((max - v02) > 0) ;	max = (cmp & max) | (~cmp & v02);
	cmp = -((max - v10) > 0) ;	max = (cmp & max) | (~cmp & v10);
	cmp = -((max - v11) > 0) ;	max = (cmp & max) | (~cmp & v11);
	cmp = -((max - v12) > 0) ;	max = (cmp & max) | (~cmp & v12); 
	cmp = -((max - v20) > 0) ;	max = (cmp & max) | (~cmp & v20); 
	cmp = -((max - v21) > 0) ;	max = (cmp & max) | (~cmp & v21); 
	cmp = -((max - v22) > 0) ;	return (cmp & max) | (~cmp & v22); 

}

uint8 erosion_test(	uint8 v00, uint8 v01, uint8 v02,
					uint8 v10, uint8 v11, uint8 v12,
					uint8 v20, uint8 v21, uint8 v22)
{
	uint8 cmp, min;
	cmp = -((v00 - v01) < 0) ;	min = (cmp & v00) | (~cmp & v01);
	cmp = -((min - v02) < 0) ;	min = (cmp & min) | (~cmp & v02);
	cmp = -((min - v10) < 0) ;	min = (cmp & min) | (~cmp & v10);
	cmp = -((min - v11) < 0) ;	min = (cmp & min) | (~cmp & v11); 
	cmp = -((min - v12) < 0) ;	min = (cmp & min) | (~cmp & v12); 
	cmp = -((min - v20) < 0) ;	min = (cmp & min) | (~cmp & v20); 
	cmp = -((min - v21) < 0) ;	min = (cmp & min) | (~cmp & v21); 
	cmp = -((min - v22) < 0) ;	return (cmp & min) | (~cmp & v22); 

}
uint8 erosion_test_m(uint8** m) { return erosion_test(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]); }
uint8 dilatation_test_m(uint8** m) { return dilatation_test(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]); }

int main()
{
	printf("-----------------------\nTests morpho scalaire\n-----------------------\n");
	uint8** mx1 = ui8matrix(0,2,0,2);
	mx1[0][0] = 9;	mx1[0][1] = 5;	mx1[0][2] = 4;
	mx1[1][0] = 8;	mx1[1][1] = 0;	mx1[1][2] = 3;
	mx1[2][0] = 7;	mx1[2][1] = 6;	mx1[2][2] = 2;

	uint8** mx2 = ui8matrix(0,2,0,2);
	mx2[0][0] = 125;	mx2[0][1] = 5;	mx2[0][2] = 125;
	mx2[1][0] = 125;	mx2[1][1] = 55;	mx2[1][2] = 7;
	mx2[2][0] = 125;	mx2[2][1] = 125;	mx2[2][2] = 6;

	uint8** mx3 = ui8matrix(0,2,0,2);
	mx3[0][0] = 1;	mx3[0][1] = 0;	mx3[0][2] = 0;
	mx3[1][0] = 0;	mx3[1][1] = 0;	mx3[1][2] = 0;
	mx3[2][0] = 0;	mx3[2][1] = 0;	mx3[2][2] = 1;

	uint8** mx4 = ui8matrix(0,2,0,2);
	mx4[0][0] = 10;	mx4[0][1] = 55;	mx4[0][2] = 10;
	mx4[1][0] = 01;	mx4[1][1] = 0;	mx4[1][2] = 51;
	mx4[2][0] = 5;	mx4[2][1] = 21;	mx4[2][2] = 55;

	uint8** mx5 = ui8matrix(0,2,0,2);
	mx5[0][0] = 4;	mx5[0][1] = 8;	mx5[0][2] = 12;
	mx5[1][0] = 5;	mx5[1][1] = 5;	mx5[1][2] = 1;
	mx5[2][0] = 6;	mx5[2][1] = 4;	mx5[2][2] = 21;
	display_ui8matrix(mx1,0,2,0,2,"%d\t","mx1: ");
	printf("mx1\tdilatation = %u == 9\t\térosion = %u == 0\n\n", dilatation_test_m(mx1), erosion_test_m(mx1));
	display_ui8matrix(mx2,0,2,0,2,"%d\t","mx2: ");
	printf("mx2\tdilatation = %u == 125\t\térosion = %u == 5\n\n", dilatation_test_m(mx2), erosion_test_m(mx2));
	display_ui8matrix(mx3,0,2,0,2,"%d\t","mx3: ");
	printf("mx3\tdilatation = %u == 1\t\térosion = %u == 0\n\n", dilatation_test_m(mx3), erosion_test_m(mx3));
	display_ui8matrix(mx4,0,2,0,2,"%d\t","mx4: ");
	printf("mx3\tdilatation = %u == 55\t\térosion = %u == 0\n\n", dilatation_test_m(mx4), erosion_test_m(mx4));
	display_ui8matrix(mx5,0,2,0,2,"%d\t","mx5: ");
	printf("mx3\tdilatation = %u == 21\t\térosion = %u == 1\n\n", dilatation_test_m(mx5), erosion_test_m(mx5));

	return 0;
}
