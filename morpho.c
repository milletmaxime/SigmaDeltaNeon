#include "algo.h"
#include "nrdef.h"
#include "nrutil.h"

static inline uint8 max3(uint8 a, uint8 b, uint8 c)
{
	uint8 cmp, tmp;
	cmp = -((a - b) > 0); tmp = (cmp & a) | (~cmp & b);
	cmp = -((tmp - c) > 0); return (cmp & tmp) | (~cmp & c);
}

static inline uint8 min3(uint8 a, uint8 b, uint8 c)
{
	uint8 cmp, tmp;
	cmp = -((a - b) < 0); tmp = (cmp & a) | (~cmp & b);
	cmp = -((tmp - c) < 0); return (cmp & tmp) | (~cmp & c);
}

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
	uint8** res = ui8matrix(i0 -1, i1 +1, j0 -1, j1 +1);
	
	set_border(m, i0, i1, j0, j1, Vmax, 1);
	
	for(int i = i0 ; i <= i1 ; i++)
	{
		for(int j = j0 ; j <= j1 ; j++)
		{
			res[i][j] = min3(min3(m[i-1][j-1], m[i-1][j  ], m[i-1][j+1]), 
							min3(m[i  ][j-1], m[i  ][j  ], m[i  ][j+1]), 
							min3(m[i+1][j-1], m[i+1][j  ], m[i+1][j+1]));	
		}
	}

	return res;
}

uint8** erosion_reduction(uint8** m, int i0, int i1, int j0, int j1)
{
	uint8** res = ui8matrix(i0 -1, i1 +1, j0 -1, j1 +1);

	// Pour réduction par colonne
	uint8 colg, colm, cold;

	set_border(m, i0, i1, j0, j1, Vmax, 1);
	
	for(int i = i0 ; i <= i1 ; i++)
	{
		//min sur la colonne de gauche
		colg = min3(m[i-1][j0-1], m[i  ][j0-1], m[i+1][j0-1]);

		//min sur la colonne du milieu
		colm = min3(m[i-1][j0  ], m[i  ][j0  ], m[i+1][j0  ]);

		for(int j = j0 ; j <= j1 ; j++)
		{
			//min sur la colonne de droite
			cold = min3(m[i-1][j+1], m[i  ][j+1], m[i+1][j+1]);

			//max des trois colonnes
			res[i][j] = min3(colg, colm, cold);

			// rotations de registres : col milieu devient col gauche et col droite devient col milieu
			colg = colm; colm = cold;
		}
	}

	return res;
}

uint8** erosion_reduction_deroulage(uint8** m, int i0, int i1, int j0, int j1)
{
	uint8** res = ui8matrix(i0 -1, i1 +1, j0 -1, j1 +1);
	
	uint8 colg, colm, cold;

	set_border(m, i0, i1, j0, j1, Vmax, 1);

	for(int i = i0 ; i <= i1 ; i++)
	{
		//min sur la colonne de gauche
		colg = min3(m[i-1][j0-1], m[i  ][j0-1], m[i+1][j0-1]);

		//min sur la colonne du milieu
		colm = min3(m[i-1][j0], m[i  ][j0], m[i+1][j0]);

		for(int j = j0 ; j <= j1 ; j+=3)
		{
		// déroulage 1

			//min sur la colonne de droite
			cold = min3(m[i-1][j+1], m[i  ][j+1], m[i+1][j+1]);

			//min des trois colonnes
			res[i][j] = min3(colg, colm, cold);

			colg = colm; colm = cold;

		// déroulage 2

			//min sur la colonne de droite
			cold = min3(m[i-1][j+2], m[i  ][j+2], m[i+1][j+2]);

			//min des trois colonnes
			res[i][j+1] = min3(colg, colm, cold);

			colg = colm; colm = cold;

		// déroulage 3
			
			//min sur la colonne de droite
			cold = min3(m[i-1][j+3], m[i  ][j+3], m[i+1][j+3]);

			//min des trois colonnes
			res[i][j+2] = min3(colg, colm, cold);

			colg = colm; colm = cold;
		}
		
		// pas d'épilogue grâce à la gestion des bords avec NR
	}
	
	return res;
}

uint8** dilatation(uint8** m, int i0, int i1, int j0, int j1)
{
	uint8** res = ui8matrix(i0 -1, i1 +1, j0 -1, j1 +1);

	set_border(m, i0, i1, j0, j1, Vmin, 1);

	for(int i = i0 ; i <= i1 ; i++)
	{
		for(int j = j0 ; j <= j1 ; j++)
		{	
			res[i][j] = max3(max3(m[i-1][j-1], m[i-1][j  ], m[i-1][j+1]), 
							max3(m[i  ][j-1], m[i  ][j  ], m[i  ][j+1]), 
							max3(m[i+1][j-1], m[i+1][j  ], m[i+1][j+1]));	
		}
	}
	
	return res;
}

uint8** dilatation_reduction(uint8** m, int i0, int i1, int j0, int j1)
{
	uint8** res = ui8matrix(i0 -1, i1 +1, j0 -1, j1 +1);
	
	// Pour réduction par colonne
	uint8 colg, colm, cold;

	set_border(m, i0, i1, j0, j1, Vmin, 1);

	for(int i = i0 ; i <= i1 ; i++)
	{
		//max sur la colonne de gauche
		colg = max3(m[i-1][j0-1], m[i  ][j0-1], m[i+1][j0-1]);

		//max sur la colonne du milieu
		colm = max3(m[i-1][j0  ], m[i  ][j0  ], m[i+1][j0  ]);

		for(int j = j0 ; j <= j1 ; j++)
		{
			//max sur la colonne de droite
			cold = max3(m[i-1][j+1], m[i  ][j+1], m[i+1][j+1]);

			//max des trois colonnes
			res[i][j] = max3(colg, colm, cold);

			// rotations de registres : col milieu devient col gauche et col droite devient col milieu
			colg = colm; colm = cold;
		}
	}
	
	return res;
}

uint8** dilatation_reduction_deroulage(uint8** m, int i0, int i1, int j0, int j1)
{
	uint8** res = ui8matrix(i0 -1, i1 +1, j0 -1, j1 +1);
	
	uint8 colg, colm, cold;

	set_border(m, i0, i1, j0, j1, Vmin, 1);

	for(int i = i0 ; i <= i1 ; i++)
	{
		//max sur la colonne de gauche
		colg = max3(m[i-1][j0-1], m[i  ][j0-1], m[i+1][j0-1]);

		//max sur la colonne du milieu
		colm = max3(m[i-1][j0], m[i  ][j0], m[i+1][j0]);

		for(int j = j0 ; j <= j1 ; j+=3)
		{
		// déroulage 1

			//max sur la colonne de droite
			cold = max3(m[i-1][j+1], m[i  ][j+1], m[i+1][j+1]);

			//max des trois colonnes
			res[i][j] = max3(colg, colm, cold);

			colg = colm; colm = cold;

		// déroulage 2

			//max sur la colonne de droite
			cold = max3(m[i-1][j+2], m[i  ][j+2], m[i+1][j+2]);

			//max des trois colonnes
			res[i][j+1] = max3(colg, colm, cold);

			colg = colm; colm = cold;

		// déroulage 3
			
			//max sur la colonne de droite
			cold = max3(m[i-1][j+3], m[i  ][j+3], m[i+1][j+3]);

			//max des trois colonnes
			res[i][j+2] = max3(colg, colm, cold);

			colg = colm; colm = cold;
		}
		
		// pas d'épilogue grâce à la gestion des bords avec NR
	}
	
	return res;
}

uint8** erosion_dilatation(uint8** m, int i0, int i1, int j0, int j1)
{	
	uint8** res = ui8matrix(i0 -1, i1 +1, j0 -1, j1 +1);
	
	set_border(m, i0, i1, j0, j1, Vmin, 1);
	
	uint8 col1m1, col10, col11, col12, col13;
	uint8 col2m1, col20, col21, col22, col23;
	uint8 col3m1, col30, col31, col32, col33;
	
	for(int i = i0 ; i <= i1 ; i++)
	{
		for(int j = j0 ; j <= j1 ; j+=3)
		{//erosion=min
			col1m1 = min3(m[i-1][j-2], m[i  ][j-2], m[i+1][j-2]);
			col10 = min3(m[i-1][j-1], m[i  ][j-1], m[i+1][j-1]);
			col11 = min3(m[i-1][j  ], m[i  ][j  ], m[i+1][j  ]);
			col12 = min3(m[i-1][j+1], m[i  ][j+1], m[i+1][j+1]);
			col13 = min3(m[i-1][j+2], m[i  ][j+2], m[i+1][j+2]);

			col2m1 = min3(m[i  ][j-2], m[i+1][j-2], m[i+2][j-2]);
			col20 = min3(m[i  ][j-1], m[i+1][j-1], m[i+2][j-1]);
			col21 = min3(m[i  ][j  ], m[i+1][j  ], m[i+2][j  ]);
			col22 = min3(m[i  ][j+1], m[i+1][j+1], m[i+2][j+1]);
			col23 = min3(m[i  ][j+2], m[i+1][j+2], m[i+2][j+2]);

			col3m1 = min3(m[i+1][j-2], m[i+2][j-2], m[i+3][j-2]);
			col30 = min3(m[i+1][j-1], m[i+2][j-1], m[i+3][j-1]);
			col31 = min3(m[i+1][j  ], m[i+2][j  ], m[i+3][j  ]);
			col32 = min3(m[i+1][j+1], m[i+2][j+1], m[i+3][j+1]);
			col33 = min3(m[i+1][j+2], m[i+2][j+2], m[i+3][j+2]);
			
			res[i-1][j-1] = min3(col10, col20, col30);
			res[i-1][j  ] = min3(col10, col20, col30);
			res[i-1][j+1] = min3(col10, col20, col30);
			
			res[i  ][j-1] = min3(col1m0, col10, col11);
			res[i  ][j  ] = min3(col10, col11, col12);
			res[i  ][j+1] = min3(col11, col12, col13);
			
			res[i+1][j-1] = min3(col10, col20, col30);
			res[i+1][j  ] = min3(col10, col20, col30);
			res[i+1][j+1] = min3(col10, col20, col30);
		}
	}
}


int main() {
	
	int m =4;
	
	uint8** test = ui8matrix(0 -1, m +1, 0 -1, m +1);
	test[0][0] = 66;
	test[0][1] = 1;
	test[0][2] = 2;
	test[0][3] = 3;
	
	test[1][0] = 10;
	test[1][1] = 11;
	test[1][2] = 12;
	test[1][3] = 13;
	
	test[2][0] = 20;
	test[2][1] = 21;
	test[2][2] = 22;
	test[2][3] = 23;

	test[3][0] = 20;
	test[3][1] = 21;
	test[3][2] = 22;
	test[3][3] = 33;
	
	display_ui8matrix(test,0,m,0,m,"%d\t","norm");
	display_ui8matrix(erosion(test, 0,m,0,m),0,m,0,m,"%d\t","erod");
	display_ui8matrix(erosion_reduction(test, 0,m,0,m),0,m,0,m,"%d\t","erodr");
	display_ui8matrix(erosion_reduction_deroulage(test, 0,m,0,m),0,m,0,m,"%d\t","erodrd");
	display_ui8matrix(dilatation(test, 0,m,0,m),0,m,0,m,"%d\t","dila");
	display_ui8matrix(dilatation_reduction(test, 0,m,0,m),0,m,0,m,"%d\t","dilar");
	display_ui8matrix(dilatation_reduction_deroulage(test, 0,m,0,m),0,m,0,m,"%d\t","dilard");
	return 0;
}
