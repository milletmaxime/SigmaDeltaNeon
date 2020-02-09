#include "algo.h"
#include "nrdef.h"
#include "nrutil.h"

static inline void set_border(uint8** m, int i0, int i1, int j0, int j1, uint8 value, int width_border)
{
	for(int i = i0 - width_border ; i <= i1 + width_border ; i++)
	{
		for(int j = j0 - width_border; j <= j1 + width_border ; j++)
		{
			m[i][j] = value;
			if(i >= i0 && i <= i1 && j == j0-1) j = j1;
		}
	}
}

uint8** erosion(uint8** m, int i0, int i1, int j0, int j1)
{
	uint8 cmp, min;
	uint8** res = ui8matrix(i0 -1, i1 +1, j0 -1, j1 +1);
	
	set_border(m, i0, i1, j0, j1, Vmax, 1);
	
	for(int i = i0 ; i <= i1 ; i++)
	{
		for(int j = j0 ; j <= j1 ; j++)
		{
			cmp = -((m[i-1][j-1] - m[i-1][j  ]) < 0);min = (cmp & m[i-1][j-1]) | (~cmp & m[i-1][j  ]);
			cmp = -((min - m[i-1][j+1]) < 0) ; min = (cmp & min) | (~cmp & m[i-1][j+1]);
			cmp = -((min - m[i  ][j-1]) < 0) ; min = (cmp & min) | (~cmp & m[i  ][j-1]);
			cmp = -((min - m[i  ][j  ]) < 0) ; min = (cmp & min) | (~cmp & m[i  ][j  ]); 
			cmp = -((min - m[i  ][j+1]) < 0) ; min = (cmp & min) | (~cmp & m[i  ][j+1]); 
			cmp = -((min - m[i+1][j-1]) < 0) ; min = (cmp & min) | (~cmp & m[i+1][j-1]); 
			cmp = -((min - m[i+1][j  ]) < 0) ; min = (cmp & min) | (~cmp & m[i+1][j  ]); 
			cmp = -((min - m[i+1][j+1]) < 0) ; res[i][j] = (cmp & min) | (~cmp & m[i+1][j+1]);
			
		}
	}

	return res;
}

uint8** dilatation(uint8** m, int i0, int i1, int j0, int j1)
{
	uint8 cmp, max;
	uint8** res = ui8matrix(i0 -1, i1 +1, j0 -1, j1 +1);
	
	set_border(m, i0, i1, j0, j1, Vmin, 1);
	
	for(int i = i0 ; i <= i1 ; i++)
	{
		max = m[i][0];
		for(int j = j0 ; j <= j1 ; j++)
		{	
			cmp = -((m[i-1][j-1] - m[i-1][j  ]) > 0);max = (cmp & m[i-1][j-1]) | (~cmp & m[i-1][j  ]);
			cmp = -((max - m[i-1][j+1]) > 0) ; max = (cmp & max) | (~cmp & m[i-1][j+1]);
			cmp = -((max - m[i  ][j-1]) > 0) ; max = (cmp & max) | (~cmp & m[i  ][j-1]);
			cmp = -((max - m[i  ][j  ]) > 0) ; max = (cmp & max) | (~cmp & m[i  ][j  ]);
			cmp = -((max - m[i  ][j+1]) > 0) ; max = (cmp & max) | (~cmp & m[i  ][j+1]); 
			cmp = -((max - m[i+1][j-1]) > 0) ; max = (cmp & max) | (~cmp & m[i+1][j-1]); 
			cmp = -((max - m[i+1][j  ]) > 0) ; max = (cmp & max) | (~cmp & m[i+1][j  ]); 
			cmp = -((max - m[i+1][j+1]) > 0) ; res[i][j] = (cmp & max) | (~cmp & m[i+1][j+1]);
		}
	}
	
	return res;
}

/*
int main() {
	uint8** test = ui8matrix(0 -1, 2 +1, 0 -1, 2 +1);
	test[0][0] = 66;
	test[0][1] = 1;
	test[0][2] = 2;
	
	test[1][0] = 10;
	test[1][1] = 11;
	test[1][2] = 12;
	
	test[2][0] = 20;
	test[2][1] = 21;
	test[2][2] = 22;
	
	display_ui8matrix(test,0,2,0,2,"%d\t","norm");
	display_ui8matrix(dilatation(test, 0,2,0,2),0,2,0,2,"%d\t","dila");
	display_ui8matrix(erosion(test, 0,2,0,2),0,2,0,2,"%d\t","erod");
	return 0;
}*/
