#include "algo.h"
#include "nrdef.h"
#include "nrutil.h"

static inline void set_border(uint8** m, int i0, int i1, int j0, int j1, uint8 value)
{
	for(int i = i0 - 1 ; i < i1 + 1 ; i++)
		for(int j = j0 - 1 ; j < j1 + 1 ; j++)
			m[i][j] = value;
}

uint8** erosion(uint8** m, int i0, int i1, int j0, int j1)
{
	uint8** res = ui8matrix(i0 -1, i1 +1, j0 -1, j1 +1);
	
	return res;
}

uint8** dilatation(uint8** m, int i0, int i1, int j0, int j1)
{
	uint8** res = ui8matrix(i0 -1, i1 +1, j0 -1, j1 +1);
	
	return res;
}
