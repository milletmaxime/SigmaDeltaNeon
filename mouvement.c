#include "algo.h"
#include "nrdef.h"
#include "nrutil.h"

void sigma_delta_initialisation()
{
	// TODO
}

void sigma_delta_iterations(uint8** I, uint8** M, uint8** V, uint8** O, uint8** E, uint8 N, unsigned width, unsigned height)
{
	int x, y;
	
	// Step 1
	for(x = 0; x < width; x++) {
		for(y = 0; y < height; y++) {
			uint8 m = M[x][y];
			uint8 i = I[x][y];
			M[x][y] = m + (1 & (m < i)) - (1 & (m > i));
		}
	}
	
	// Step 2
	for(x = 0; x < width; x++) {
		for(y = 0; y < height; y++) {
			int o = M[x][y] - I[x][y]; // int pour avoir un nombre negatif et que le test puisse retourner 0
			uint8 gtz = -(o >= 0); //-(...) pour avoir un masque complet 0xFFFF
			O[x][y] = (gtz & o) | ((~gtz) & -o);
		}
	}
	
	// Step 3
	for(x = 0; x < width; x++) {
		for(y = 0; y < height; y++) {
			uint8 v = V[x][y];
			uint8 o = O[x][y];
			
			uint8 no = N * o;
			uint8 lt = v < no;
			uint8 gt = v > no;
			
			v = v + (1 & lt) - (1 & gt);
			
			// clamp to [Vmin, Vmax]
			lt = -(v < Vmax); // - pour avoir le masque complet
			v = (lt & v) | (~lt & Vmax);

			gt = -(v > Vmin); // - pour avoir le masque complet
			v = (gt & v) | (~gt & Vmin);
			
			V[x][y] = v;
		}
	}
	
	// Step 4
	for(x = 0; x < width; x++) {
		for(y = 0; y < height; y++) {
			uint8 lt = O[x][y] < V[x][y];
			E[x][y] = 255 * ((lt & 0) | (~lt & 1));
		}
	}
}
