#include "valgo.h"
#include "vnrdef.h"
#include "vnrutil.h"
#include "nrutil.h"

// Ajouter bordure dans une matrice de vuint8
static inline void set_border(vuint8** m, int vi0, int vi1, int vj0, int vj1, vuint8 value, int width_border)
{
	for(int i = vi0 - width_border ; i <= vi1 + width_border ; i++)
	{
		for(int j = vj0 - width_border; j <= vj1 + width_border ; j++)
		{
			m[i][j] = value;
			if(i >= vi0 && i <= vi1 && j == vj0-1) j = vj1;
		}
	}
}

/* ************************* *
 * EROSION SANS OPTIMISATION *
 * ************************* */

// Corps de la double boucle pour une itération
static inline vuint8 erosion_SIMD_loop(vuint8** m, int i, int j)
{
	// vg = Vecteur à gauche, vm = Vecteur au milieu, vd = Vecteur à droite
	vuint8 vg1 = m[i-1][j-1]; vuint8 vm1 = m[i-1][j  ]; vuint8 vd1 = m[i-1][j+1]; 
	vuint8 vg2 = m[i  ][j-1]; vuint8 vm2 = m[i  ][j  ]; vuint8 vd2 = m[i  ][j+1]; 
	vuint8 vg3 = m[i+1][j-1]; vuint8 vm3 = m[i+1][j  ]; vuint8 vd3 = m[i+1][j+1]; 

	// Vecteurs milieu décalés de 1 à gauche
	vuint8 vm1_g = vextq_u8(vg1, vm1, 15);
	vuint8 vm2_g = vextq_u8(vg2, vm2, 15);
	vuint8 vm3_g = vextq_u8(vg3, vm3, 15);

	// Vecteurs milieu décalés de 1 à droite
	vuint8 vm1_d = vextq_u8(vm1, vd1, 1);
	vuint8 vm2_d = vextq_u8(vm2, vd2, 1);
	vuint8 vm3_d = vextq_u8(vm3, vd3, 1);

	// minimum dans les 9 vecteurs
	return vminq_u8(
					vminq_u8(vminq_u8(vm1, vm2), vm3),
					vminq_u8(
						vminq_u8(vminq_u8(vm1_g, vm2_g), vm3_g),
						vminq_u8(vminq_u8(vm1_d, vm2_d), vm3_d)
					)
				);
	}

vuint8** erosion_SIMD(vuint8** m, int vi0, int vi1, int vj0, int vj1)
{
	vuint8** res = vui8matrix(vi0 -1, vi1 +1, vj0 -1, vj1 +1);
	set_border(m, vi0, vi1, vj0, vj1, vmovq_n_u8(Vmax),1);
	
	for(int i = vi0 ; i <= vi1 ; i++)
	{
		for(int j = vj0 ; j <= vj1 ; j++)
		{
			res[i][j] = erosion_SIMD_loop(m, i, j);
		}
	}
	return res;
}

vuint8** erosion_SIMD_openmp(vuint8** m, int vi0, int vi1, int vj0, int vj1)
{
	vuint8** res = vui8matrix(vi0 -1, vi1 +1, vj0 -1, vj1 +1);
	set_border(m, vi0, vi1, vj0, vj1, vmovq_n_u8(Vmax),1);
	
	#pragma omp parallel for
	for(int i = vi0 ; i <= vi1 ; i++)
	{
		for(int j = vj0 ; j <= vj1 ; j++)
		{
			res[i][j] = erosion_SIMD_loop(m, i, j);
		}
	}
	return res;
}

/* ********************** *
 * EROSION AVEC REDUCTION *
 * ********************** */

vuint8** erosion_SIMD_reduction(vuint8** m, int vi0, int vi1, int vj0, int vj1)
{
	vuint8** res = vui8matrix(vi0 -1, vi1 +1, vj0 -1, vj1 +1);
	set_border(m, vi0, vi1, vj0, vj1, vmovq_n_u8(Vmin),1);
	
	// vg = Vecteur à gauche, vm = Vecteur au milieu, vd = Vecteur à droite
	vuint8 vg1 , vm1, vd1;
	vuint8 vg2 , vm2, vd2;
	vuint8 vg3 , vm3, vd3;

	vuint8 min_vmg, min_vm, min_vmd;
	
	for(int i = vi0 ; i <= vi1 ; i++)
	{
		vg1 = m[i-1][vj0-1]; vm1 = m[i-1][vj0  ];
		vg2 = m[i  ][vj0-1]; vm2 = m[i  ][vj0  ];
		vg3 = m[i+1][vj0-1]; vm3 = m[i+1][vj0  ];
		
		// Max des vecteurs milieu décalés de 1 à gauche
		min_vmg = vminq_u8(vminq_u8(vextq_u8(vg1, vm1, 15),vextq_u8(vg2, vm2, 15)),vextq_u8(vg3, vm3, 15));
		min_vm = vminq_u8(vminq_u8(vm1, vm2), vm3);

		for(int j = vj0 ; j <= vj1 ; j++)
		{
			vd1 = m[i-1][j+1]; 
			vd2 = m[i  ][j+1]; 
			vd3 = m[i+1][j+1];
			
			min_vmd = vminq_u8(vminq_u8(vextq_u8(vm1, vd1, 1),vextq_u8(vm2, vd2, 1)),vextq_u8(vm3, vd3, 1));


			// minimum dans les 9 vecteurs
			res[i][j] = vminq_u8(min_vm, vminq_u8(min_vmg,min_vmd));

			vg1 = vm1; vm1 = vd1;
			vg2 = vm2; vm2 = vd2;
			vg3 = vm3; vm3 = vd3;
			min_vmg = vminq_u8(vminq_u8(vextq_u8(vg1, vm1, 15),vextq_u8(vg2, vm2, 15)),vextq_u8(vg3, vm3, 15));
			min_vm = vminq_u8(vminq_u8(vm1, vm2), vm3);
		}
	}
	return res;
}


/* *********************************** *
 * EROSION AVEC REDUCTION ET DEROULAGE *
 * *********************************** */

vuint8** erosion_SIMD_reduction_deroulage(vuint8** m, int vi0, int vi1, int vj0, int vj1)
{
	vuint8** res = vui8matrix(vi0 -1, vi1 +1, vj0 -1, vj1 +1);
	set_border(m, vi0, vi1, vj0, vj1, vmovq_n_u8(Vmin),1);
	
	// vg = Vecteur à gauche, vm = Vecteur au milieu, vd = Vecteur à droite
	vuint8 vg1 , vm1, vd1;
	vuint8 vg2 , vm2, vd2;
	vuint8 vg3 , vm3, vd3;

	vuint8 min_vmg, min_vm, min_vmd;
	
	for(int i = vi0 ; i <= vi1 ; i++)
	{
		vg1 = m[i-1][vj0-1]; vm1 = m[i-1][vj0  ];
		vg2 = m[i  ][vj0-1]; vm2 = m[i  ][vj0  ];
		vg3 = m[i+1][vj0-1]; vm3 = m[i+1][vj0  ];
		
		// Max des vecteurs milieu décalés de 1 à gauche
		min_vmg = vminq_u8(vminq_u8(vextq_u8(vg1, vm1, 15),vextq_u8(vg2, vm2, 15)),vextq_u8(vg3, vm3, 15));
		min_vm = vminq_u8(vminq_u8(vm1, vm2), vm3);

		for(int j = vj0 ; j <= vj1 ; j+=3)
		{
		// déroulage 1
			vd1 = m[i-1][j+1]; 
			vd2 = m[i  ][j+1]; 
			vd3 = m[i+1][j+1];
			
			min_vmd = vminq_u8(vminq_u8(vextq_u8(vm1, vd1, 1),vextq_u8(vm2, vd2, 1)),vextq_u8(vm3, vd3, 1));

			// minimum dans les 9 vecteurs
			res[i][j] = vminq_u8(min_vm, vminq_u8(min_vmg,min_vmd));

			vg1 = vm1; vm1 = vd1;
			vg2 = vm2; vm2 = vd2;
			vg3 = vm3; vm3 = vd3;
			min_vmg = vminq_u8(vminq_u8(vextq_u8(vg1, vm1, 15),vextq_u8(vg2, vm2, 15)),vextq_u8(vg3, vm3, 15));
			min_vm = vminq_u8(vminq_u8(vm1, vm2), vm3);

		// déroulage 2
			vd1 = m[i-1][j+2]; 
			vd2 = m[i  ][j+2]; 
			vd3 = m[i+1][j+2];
			
			min_vmd = vminq_u8(vminq_u8(vextq_u8(vm1, vd1, 1),vextq_u8(vm2, vd2, 1)),vextq_u8(vm3, vd3, 1));

			// minimum dans les 9 vecteurs
			res[i][j+1] = vminq_u8(min_vm, vminq_u8(min_vmg,min_vmd));

			vg1 = vm1; vm1 = vd1;
			vg2 = vm2; vm2 = vd2;
			vg3 = vm3; vm3 = vd3;
			min_vmg = vminq_u8(vminq_u8(vextq_u8(vg1, vm1, 15),vextq_u8(vg2, vm2, 15)),vextq_u8(vg3, vm3, 15));
			min_vm = vminq_u8(vminq_u8(vm1, vm2), vm3);

		// déroulage 3
			vd1 = m[i-1][j+3]; 
			vd2 = m[i  ][j+3]; 
			vd3 = m[i+1][j+3];
			
			min_vmd = vminq_u8(vminq_u8(vextq_u8(vm1, vd1, 1),vextq_u8(vm2, vd2, 1)),vextq_u8(vm3, vd3, 1));

			// minimum dans les 9 vecteurs
			res[i][j+2] = vminq_u8(min_vm, vminq_u8(min_vmg,min_vmd));

			vg1 = vm1; vm1 = vd1;
			vg2 = vm2; vm2 = vd2;
			vg3 = vm3; vm3 = vd3;
			min_vmg = vminq_u8(vminq_u8(vextq_u8(vg1, vm1, 15),vextq_u8(vg2, vm2, 15)),vextq_u8(vg3, vm3, 15));
			min_vm = vminq_u8(vminq_u8(vm1, vm2), vm3);
		}
	}
	return res;
}

/* **************************** *
 * DILATATION SANS OPTIMISATION *
 * **************************** */

// Corps de la double boucle pour une itération
static inline vuint8 dilatation_SIMD_loop(vuint8** m, int i, int j)
{
	// vg = Vecteur à gauche, vm = Vecteur au milieu, vd = Vecteur à droite
	vuint8 vg1 = m[i-1][j-1]; vuint8 vm1 = m[i-1][j  ]; vuint8 vd1 = m[i-1][j+1]; 
	vuint8 vg2 = m[i  ][j-1]; vuint8 vm2 = m[i  ][j  ]; vuint8 vd2 = m[i  ][j+1]; 
	vuint8 vg3 = m[i+1][j-1]; vuint8 vm3 = m[i+1][j  ]; vuint8 vd3 = m[i+1][j+1]; 

	// Vecteurs milieu décalés de 1 à gauche
	vuint8 vm1_g = vextq_u8(vg1, vm1, 15);
	vuint8 vm2_g = vextq_u8(vg2, vm2, 15);
	vuint8 vm3_g = vextq_u8(vg3, vm3, 15);

	// Vecteurs milieu décalés de 1 à droite
	vuint8 vm1_d = vextq_u8(vm1, vd1, 1);
	vuint8 vm2_d = vextq_u8(vm2, vd2, 1);
	vuint8 vm3_d = vextq_u8(vm3, vd3, 1);

	// maximum dans les 9 vecteurs
	return vmaxq_u8(
					vmaxq_u8(vmaxq_u8(vm1, vm2), vm3),
					vmaxq_u8(
						vmaxq_u8(vmaxq_u8(vm1_g, vm2_g), vm3_g),
						vmaxq_u8(vmaxq_u8(vm1_d, vm2_d), vm3_d)
					)
				);
	}

vuint8** dilatation_SIMD(vuint8** m, int vi0, int vi1, int vj0, int vj1)
{
	vuint8** res = vui8matrix(vi0 -1, vi1 +1, vj0 -1, vj1 +1);
	set_border(m, vi0, vi1, vj0, vj1, vmovq_n_u8(Vmax),1);

	for(int i = vi0 ; i <= vi1 ; i++)
	{
		for(int j = vj0 ; j <= vj1 ; j++)
		{
			res[i][j] = dilatation_SIMD_loop(m, i, j);
		}
	}
	return res;
}

vuint8** dilatation_SIMD_openmp(vuint8** m, int vi0, int vi1, int vj0, int vj1)
{
	vuint8** res = vui8matrix(vi0 -1, vi1 +1, vj0 -1, vj1 +1);
	set_border(m, vi0, vi1, vj0, vj1, vmovq_n_u8(Vmax),1);

	#pragma omp parallel for
	for(int i = vi0 ; i <= vi1 ; i++)
	{
		for(int j = vj0 ; j <= vj1 ; j++)
		{
			res[i][j] = dilatation_SIMD_loop(m, i, j);
		}
	}
	return res;
}

/* ************************* *
 * DILATATION AVEC REDUCTION *
 * ************************* */

vuint8** dilatation_SIMD_reduction(vuint8** m, int vi0, int vi1, int vj0, int vj1)
{
	vuint8** res = vui8matrix(vi0 -1, vi1 +1, vj0 -1, vj1 +1);
	set_border(m, vi0, vi1, vj0, vj1, vmovq_n_u8(Vmin),1);
	
	// vg = Vecteur à gauche, vm = Vecteur au milieu, vd = Vecteur à droite
	vuint8 vg1 , vm1, vd1;
	vuint8 vg2 , vm2, vd2;
	vuint8 vg3 , vm3, vd3;

	vuint8 max_vmg, max_vm, max_vmd;
	
	for(int i = vi0 ; i <= vi1 ; i++)
	{
		vg1 = m[i-1][vj0-1]; vm1 = m[i-1][vj0  ];
		vg2 = m[i  ][vj0-1]; vm2 = m[i  ][vj0  ];
		vg3 = m[i+1][vj0-1]; vm3 = m[i+1][vj0  ];
		
		// Max des vecteurs milieu décalés de 1 à gauche
		max_vmg = vmaxq_u8(vmaxq_u8(vextq_u8(vg1, vm1, 15),vextq_u8(vg2, vm2, 15)),vextq_u8(vg3, vm3, 15));
		max_vm = vmaxq_u8(vmaxq_u8(vm1, vm2), vm3);

		for(int j = vj0 ; j <= vj1 ; j++)
		{
			vd1 = m[i-1][j+1]; 
			vd2 = m[i  ][j+1]; 
			vd3 = m[i+1][j+1];
			
			max_vmd = vmaxq_u8(vmaxq_u8(vextq_u8(vm1, vd1, 1),vextq_u8(vm2, vd2, 1)),vextq_u8(vm3, vd3, 1));


			// maximum dans les 9 vecteurs
			res[i][j] = vmaxq_u8(max_vm, vmaxq_u8(max_vmg,max_vmd));

			vg1 = vm1; vm1 = vd1;
			vg2 = vm2; vm2 = vd2;
			vg3 = vm3; vm3 = vd3;
			max_vmg = vmaxq_u8(vmaxq_u8(vextq_u8(vg1, vm1, 15),vextq_u8(vg2, vm2, 15)),vextq_u8(vg3, vm3, 15));
			max_vm = vmaxq_u8(vmaxq_u8(vm1, vm2), vm3);
		}
	}
	return res;
}

/* ************************************** *
 * DILATATION AVEC REDUCTION ET DEROULAGE *
 * ************************************** */

vuint8** dilatation_SIMD_reduction_deroulage(vuint8** m, int vi0, int vi1, int vj0, int vj1)
{
	vuint8** res = vui8matrix(vi0 -1, vi1 +1, vj0 -1, vj1 +1);
	set_border(m, vi0, vi1, vj0, vj1, vmovq_n_u8(Vmin),1);

	// vg = Vecteur à gauche, vm = Vecteur au milieu, vd = Vecteur à droite
	vuint8 vg1 , vm1, vd1;
	vuint8 vg2 , vm2, vd2;
	vuint8 vg3 , vm3, vd3;

	vuint8 max_vmg, max_vm, max_vmd;
	
	for(int i = vi0 ; i <= vi1 ; i++)
	{
		vg1 = m[i-1][vj0-1]; vm1 = m[i-1][vj0  ];
		vg2 = m[i  ][vj0-1]; vm2 = m[i  ][vj0  ];
		vg3 = m[i+1][vj0-1]; vm3 = m[i+1][vj0  ];
		
		// Max des vecteurs milieu décalés de 1 à gauche
		max_vmg = vmaxq_u8(vmaxq_u8(vextq_u8(vg1, vm1, 15),vextq_u8(vg2, vm2, 15)),vextq_u8(vg3, vm3, 15));
		max_vm = vmaxq_u8(vmaxq_u8(vm1, vm2), vm3);

		for(int j = vj0 ; j <= vj1 ; j+=3)
		{
		// déroulage 1
			vd1 = m[i-1][j+1]; 
			vd2 = m[i  ][j+1]; 
			vd3 = m[i+1][j+1];
			
			max_vmd = vmaxq_u8(vmaxq_u8(vextq_u8(vm1, vd1, 1),vextq_u8(vm2, vd2, 1)),vextq_u8(vm3, vd3, 1));

			// maximum dans les 9 vecteurs
			res[i][j] = vmaxq_u8(max_vm, vmaxq_u8(max_vmg,max_vmd));

			vg1 = vm1; vm1 = vd1;
			vg2 = vm2; vm2 = vd2;
			vg3 = vm3; vm3 = vd3;
			max_vmg = vmaxq_u8(vmaxq_u8(vextq_u8(vg1, vm1, 15),vextq_u8(vg2, vm2, 15)),vextq_u8(vg3, vm3, 15));
			max_vm = vmaxq_u8(vmaxq_u8(vm1, vm2), vm3);

		// déroulage 2
			vd1 = m[i-1][j+2]; 
			vd2 = m[i  ][j+2]; 
			vd3 = m[i+1][j+2];
			
			max_vmd = vmaxq_u8(vmaxq_u8(vextq_u8(vm1, vd1, 1),vextq_u8(vm2, vd2, 1)),vextq_u8(vm3, vd3, 1));

			// maximum dans les 9 vecteurs
			res[i][j+1] = vmaxq_u8(max_vm, vmaxq_u8(max_vmg,max_vmd));

			vg1 = vm1; vm1 = vd1;
			vg2 = vm2; vm2 = vd2;
			vg3 = vm3; vm3 = vd3;
			max_vmg = vmaxq_u8(vmaxq_u8(vextq_u8(vg1, vm1, 15),vextq_u8(vg2, vm2, 15)),vextq_u8(vg3, vm3, 15));
			max_vm = vmaxq_u8(vmaxq_u8(vm1, vm2), vm3);

		// déroulage 3
			vd1 = m[i-1][j+3]; 
			vd2 = m[i  ][j+3]; 
			vd3 = m[i+1][j+3];
			
			max_vmd = vmaxq_u8(vmaxq_u8(vextq_u8(vm1, vd1, 1),vextq_u8(vm2, vd2, 1)),vextq_u8(vm3, vd3, 1));

			// maximum dans les 9 vecteurs
			res[i][j+2] = vmaxq_u8(max_vm, vmaxq_u8(max_vmg,max_vmd));

			vg1 = vm1; vm1 = vd1;
			vg2 = vm2; vm2 = vd2;
			vg3 = vm3; vm3 = vd3;
			max_vmg = vmaxq_u8(vmaxq_u8(vextq_u8(vg1, vm1, 15),vextq_u8(vg2, vm2, 15)),vextq_u8(vg3, vm3, 15));
			max_vm = vmaxq_u8(vmaxq_u8(vm1, vm2), vm3);
		}
	}
	return res;
}



/*
int main() {
	int i0,i1,j0,j1,vj0,vj1,vi0,vi1;
	
	uint8** src = LoadPGM_ui8matrix("./car3/car_3140.pgm", &i0, &i1, &j0, &j1);
	printf("ok\n");
	vuint8** srcbords = vui8matrix(i0 -1, i1 +1, j0 -1, j1 +1);
	printf("ok2\n");
	copy_ui8matrix_ui8matrix(src, i0, i1, j0, j1, (uint8**)srcbords);
	
	s2v(i0, i1, j0, j1, card_vuint8(), &vi0, &vi1, &vj0, &vj1);

	vuint8** d1 = dilatation_SIMD(srcbords,vi0,vi1,vj0,vj1);
	SavePGM_ui8matrix((uint8**)d1, i0, i1, j0, j1, "./dilatation.pgm");
	
	vuint8** d2 = dilatation_SIMD_reduction(srcbords,vi0,vi1,vj0,vj1);
	SavePGM_ui8matrix((uint8**)d2, i0, i1, j0, j1, "./dilatation_red.pgm");
	
	vuint8** d3 = dilatation_SIMD_reduction_deroulage(srcbords,vi0,vi1,vj0,vj1);
	SavePGM_ui8matrix((uint8**)d3, i0, i1, j0, j1, "./dilatation_red_der.pgm");

	vuint8** e1 = erosion_SIMD(srcbords,vi0,vi1,vj0,vj1);
	SavePGM_ui8matrix((uint8**)e1, i0, i1, j0, j1, "./erosion.pgm");
	
	vuint8** e2 = erosion_SIMD_reduction(srcbords,vi0,vi1,vj0,vj1);
	SavePGM_ui8matrix((uint8**)e2, i0, i1, j0, j1, "./erosion_red.pgm");
	
	vuint8** e3 = erosion_SIMD_reduction_deroulage(srcbords,vi0,vi1,vj0,vj1);
	SavePGM_ui8matrix((uint8**)e3, i0, i1, j0, j1, "./erosion_red_der.pgm");

	return 0;
}

*/
