#include "nrdef.h"

#ifndef __MORPHO_H__
#define __MORPHO_H__

// Sans optimisation
uint8** erosion(uint8** m, int i0, int i1, int j0, int j1);
uint8** dilatation(uint8** m, int i0, int i1, int j0, int j1);

// Réduction de colonnes
uint8** dilatation_reduction(uint8** m, int i0, int i1, int j0, int j1);
uint8** erosion_reduction(uint8** m, int i0, int i1, int j0, int j1);

// Réduction de colonnes + déroulage de boucle
uint8** dilatation_reduction_deroulage(uint8** m, int i0, int i1, int j0, int j1);
uint8** erosion_reduction_deroulage(uint8** m, int i0, int i1, int j0, int j1);

// Boucle externe parallélisée avec openmp (sans optimisation logicielle)
uint8** erosion_openmp(uint8** m, int i0, int i1, int j0, int j1);
uint8** dilatation_openmp(uint8** m, int i0, int i1, int j0, int j1);

#endif

