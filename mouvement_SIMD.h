#include "nrdef.h"
#include "vnrdef.h"

#ifndef __MOUVEMENT_SIMD_H__
#define __MOUVEMENT_SIMD_H__

// Initialise les matrices pour l'algorithme (sans itération)
void sigma_delta_SIMD_initialisation(uint8** src, vuint8*** I, vuint8*** M, vuint8*** O, vuint8*** V, vuint8*** E, int i0, int i1, int j0, int j1, int* vi0, int* vi1, int* vj0, int* vj1);

// Itération du Sigma Delta sans aucune optimisation
void sigma_delta_SIMD_iteration(vuint8** I, vuint8** M, vuint8** O, vuint8** V, vuint8** E, uint8 N, int vi0, int vi1, int vj0, int vj1);

// Itréation du Sigma Delta avec fusion des boucles externes (modèle producteur consommateur)
void sigma_delta_SIMD_iteration_prodcons(vuint8** I, vuint8** M, vuint8** O, vuint8** V, vuint8** E, uint8 N, int vi0, int vi1, int vj0, int vj1);

// Itréation du Sigma Delta avec fusion des boucles externes et boucle externe paraléllisée avec OpenMP
void sigma_delta_SIMD_iteration_prodcons_openmp(vuint8** I, vuint8** M, vuint8** O, vuint8** V, vuint8** E, uint8 N, int vi0, int vi1, int vj0, int vj1);

// Itréation du Sigma Delta avec fusion des boucles externes et boucle externe et interne paraléllisées avec OpenMP
void sigma_delta_SIMD_iteration_prodcons_openmp_collapsed(vuint8** I, vuint8** M, vuint8** O, vuint8** V, vuint8** E, uint8 N, int vi0, int vi1, int vj0, int vj1);

// Destructeur des matrices de l'algorithme
void sigma_delta_SIMD_free(vuint8** I, vuint8** M, vuint8** O, vuint8** V, vuint8** E, int i0, int i1, int j0, int j1);

#endif
