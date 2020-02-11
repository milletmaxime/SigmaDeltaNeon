#include "nrdef.h"

#ifndef __MOUVEMENT_H__
#define __MOUVEMENT_H__

// Initialise les matrices pour l'algorithme (sans itération)
void sigma_delta_initialisation(uint8** src, uint8*** I, uint8*** M, uint8*** O, uint8*** V, uint8*** E, int i0, int i1, int j0, int j1);

// Itération du Sigma Delta sans aucune optimisation
void sigma_delta_iteration(uint8** I, uint8** M, uint8** O, uint8** V, uint8** E, uint8 N, int i0, int i1, int j0, int j1);

// Itréation du Sigma Delta avec fusion des boucles externes (modèle producteur consommateur)
void sigma_delta_iteration_prodcons(uint8** I, uint8** M, uint8** O, uint8** V, uint8** E, uint8 N, int i0, int i1, int j0, int j1);

// Itréation du Sigma Delta avec fusion des boucles externes et boucle externe paraléllisée avec OpenMP
void sigma_delta_iteration_prodcons_openmp(uint8** I, uint8** M, uint8** O, uint8** V, uint8** E, uint8 N, int i0, int i1, int j0, int j1);

// Itréation du Sigma Delta avec fusion des boucles externes et boucle externe et interne paraléllisées avec OpenMP
void sigma_delta_iteration_prodcons_openmp_collapsed(uint8** I, uint8** M, uint8** O, uint8** V, uint8** E, uint8 N, int i0, int i1, int j0, int j1);

// Destructeur des matrices de l'algorithme
void sigma_delta_free(uint8** I, uint8** M, uint8** O, uint8** V, uint8** E, int i0, int i1, int j0, int j1);

#endif
