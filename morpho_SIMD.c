#include "valgo.h"
#include "vnrdef.h"
#include "vnrutil.h"
#include "nrutil.h"

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

vuint8** erosion_SIMD(vuint8** m, int vi0, int vi1, int vj0, int vj1)
{
	vuint8** res = vui8matrix(vi0 -1, vi1 +1, vj0 -1, vj1 +1);
	set_border(m, vi0, vi1, vj0, vj1, vmovq_n_u8(Vmax),1);
	
	// vg = Vecteur à gauche, vm = Vecteur au milieu, vd = Vecteur à droite
	vuint8 vg1 , vm1, vd1;
	vuint8 vg2 , vm2, vd2;
	vuint8 vg3 , vm3, vd3;
	// vmX_g = Vecteur milieu X décalé de 1 vers la gauche, vmX_d = décalé vers la droite
	vuint8 vm1_g, vm2_g, vm3_g, vm1_d, vm2_d, vm3_d;
	
	for(int i = vi0 ; i <= vi1 ; i++)
	{
		for(int j = vj0 ; j <= vj1 ; j++)
		{
			vg1 = m[i-1][j-1]; vm1 = m[i-1][j  ]; vd1 = m[i-1][j+1]; 
			vg2 = m[i  ][j-1]; vm2 = m[i  ][j  ]; vd2 = m[i  ][j+1]; 
			vg3 = m[i+1][j-1]; vm3 = m[i+1][j  ]; vd3 = m[i+1][j+1]; 
			
			// Vecteurs milieu décalés de 1 à gauche
			vm1_g = vextq_u8(vg1, vm1, 15);
			vm2_g = vextq_u8(vg2, vm2, 15);
			vm3_g = vextq_u8(vg3, vm3, 15);

			// Vecteurs milieu décalés de 1 à droite
			vm1_d = vextq_u8(vm1, vd1, 1);
			vm2_d = vextq_u8(vm2, vd2, 1);
			vm3_d = vextq_u8(vm3, vd3, 1);

			// minimum dans les 9 vecteurs
			res[i][j] = vminq_u8(
							vminq_u8(vminq_u8(vm1, vm2), vm3),
							vminq_u8(
								vminq_u8(vminq_u8(vm1_g, vm2_g), vm3_g),
								vminq_u8(vminq_u8(vm1_d, vm2_d), vm3_d)
							)
						);
		}
	}
	

	return res;
}

vuint8** dilatation_SIMD(vuint8** m, int vi0, int vi1, int vj0, int vj1)
{
	vuint8** res = vui8matrix(vi0 -1, vi1 +1, vj0 -1, vj1 +1);
	set_border(m, vi0, vi1, vj0, vj1, vmovq_n_u8(Vmin),1);
	
	// vg = Vecteur à gauche, vm = Vecteur au milieu, vd = Vecteur à droite
	vuint8 vg1 , vm1, vd1;
	vuint8 vg2 , vm2, vd2;
	vuint8 vg3 , vm3, vd3;
	// vmX_g = Vecteur milieu X décalé de 1 vers la gauche, vmX_d = décalé vers la droite
	vuint8 vm1_g, vm2_g, vm3_g, vm1_d, vm2_d, vm3_d;
	
	for(int i = vi0 ; i <= vi1 ; i++)
	{
		for(int j = vj0 ; j <= vj1 ; j++)
		{
			vg1 = m[i-1][j-1]; vm1 = m[i-1][j  ]; vd1 = m[i-1][j+1]; 
			vg2 = m[i  ][j-1]; vm2 = m[i  ][j  ]; vd2 = m[i  ][j+1]; 
			vg3 = m[i+1][j-1]; vm3 = m[i+1][j  ]; vd3 = m[i+1][j+1]; 
			
			// Vecteurs milieu décalés de 1 à gauche
			vm1_g = vextq_u8(vg1, vm1, 15);
			vm2_g = vextq_u8(vg2, vm2, 15);
			vm3_g = vextq_u8(vg3, vm3, 15);

			// Vecteurs milieu décalés de 1 à droite
			vm1_d = vextq_u8(vm1, vd1, 1);
			vm2_d = vextq_u8(vm2, vd2, 1);
			vm3_d = vextq_u8(vm3, vd3, 1);

			// maximum dans les 9 vecteurs
			res[i][j] = vmaxq_u8(
							vmaxq_u8(vmaxq_u8(vm1, vm2), vm3),
							vmaxq_u8(
								vmaxq_u8(vmaxq_u8(vm1_g, vm2_g), vm3_g),
								vmaxq_u8(vmaxq_u8(vm1_d, vm2_d), vm3_d)
							)
						);
		}
	}
	return res;
}
/*
int main() {
	
	int m = 2;
	vuint8** test = vui8matrix(0 -1, m +1, 0 -1, 0 +1);
	
	uint8 t1[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
	uint8 t2[16] = {21,2,3,4,5,6,7,0,9,10,11,12,13,14,15,16};
	uint8 t3[16] = {1,2,3,4,5,6,47,8,9,10,11,12,13,14,15,16};
	test[0][0] = vld1q_u8(t1);
	test[1][0] = vld1q_u8(t2);
	test[2][0] = vld1q_u8(t3);

	display_vui8matrix(test,0,m,0,0,"%d\t","norm\n");
	display_vui8matrix(dilatation_SIMD(test, 0,m,0,0),0,m,0,0,"%d\t","dila\n");
	display_vui8matrix(erosion_SIMD(test, 0,m,0,0),0,m,0,0,"%d\t","erod\n");
	return 0;
}
*/
