#include "vnrdef.h"

#ifndef __MORPHO_SIMD_H__
#define __MORPHO_SIMD_H__

// Sans optimisation
vuint8** dilatation_SIMD(vuint8** m, int vi0, int vi1, int vj0, int vj1);
vuint8** erosion_SIMD(vuint8** m, int vi0, int vi1, int vj0, int vj1);

// Réduction de colonnes
vuint8** dilatation_SIMD_reduction(vuint8** m, int vi0, int vi1, int vj0, int vj1);
vuint8** erosion_SIMD_reduction(vuint8** m, int vi0, int vi1, int vj0, int vj1);

// Réduction de colonnes + déroulage de boucle
vuint8** dilatation_SIMD_reduction_deroulage(vuint8** m, int vi0, int vi1, int vj0, int vj1);
vuint8** erosion_SIMD_reduction_deroulage(vuint8** m, int vi0, int vi1, int vj0, int vj1);

// Boucle externe parallélisée avec openmp (sans optimisation logicielle)
vuint8** dilatation_SIMD_openmp(vuint8** m, int vi0, int vi1, int vj0, int vj1);
vuint8** erosion_SIMD_openmp(vuint8** m, int vi0, int vi1, int vj0, int vj1);

#endif
