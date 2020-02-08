#include "nrdef.h"
#include "nrutil.h"

uint8 dilatation_test(	uint8 v00, uint8 v01, uint8 v02,
									uint8 v10, uint8 v11, uint8 v12,
									uint8 v20, uint8 v21, uint8 v22)
{
	uint8 cmp, max;
	cmp = -((v00 - v01) > 0) ;	max = (cmp & v00) | (~cmp & v01);
	cmp = -((max - v02) > 0) ;	max = (cmp & max) | (~cmp & v02);
	cmp = -((max - v10) > 0) ;	max = (cmp & max) | (~cmp & v10);
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
	cmp = -((min - v12) < 0) ;	min = (cmp & min) | (~cmp & v12); 
	cmp = -((min - v20) < 0) ;	min = (cmp & min) | (~cmp & v20); 
	cmp = -((min - v21) < 0) ;	min = (cmp & min) | (~cmp & v21); 
	cmp = -((min - v22) < 0) ;	return (cmp & min) | (~cmp & v22); 

}
