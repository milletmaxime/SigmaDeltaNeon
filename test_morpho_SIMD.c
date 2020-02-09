#include "valgo.h"
#include "vnrdef.h"
#include "vnrutil.h"

vuint8 dilatation_SIMD_test(vuint8 l1, vuint8 l2, vuint8 l3) // dilatation = max
{
	vuint8 bord = vmovq_n_u8(Vmin); // pour simuler les bords

	vuint8 v1 = vextq_u8(bord, l1, 15);
	vuint8 v2 = vextq_u8(bord, l2, 15);
	vuint8 v3 = vextq_u8(bord, l3, 15);
	vuint8 v4 = l1;

	vuint8 v9 = l2;

	vuint8 v5 = l3;
	vuint8 v6 = vextq_u8(l1, bord, 1);
	vuint8 v7 = vextq_u8(l2, bord, 1);
	vuint8 v8 = vextq_u8(l3, bord, 1);

	return vmaxq_u8(vmaxq_u8(vmaxq_u8(vmaxq_u8(v1 ,v2) ,vmaxq_u8(v3 ,v4)) ,vmaxq_u8(vmaxq_u8(v5 ,v6) ,vmaxq_u8(v7 ,v8))), v9);
}

vuint8 erosion_SIMD_test(vuint8 l1, vuint8 l2, vuint8 l3) // erosion = min
{
	vuint8 bord = vmovq_n_u8(Vmax); // pour simuler les bords

	vuint8 v1 = vextq_u8(bord, l1, 15); // bord gauche
	vuint8 v2 = vextq_u8(bord, l2, 15);
	vuint8 v3 = vextq_u8(bord, l3, 15);

	vuint8 v4 = l1;

	vuint8 v9 = l2;

	vuint8 v5 = l3;

	vuint8 v6 = vextq_u8(l1, bord, 1); // bord droit
	vuint8 v7 = vextq_u8(l2, bord, 1);
	vuint8 v8 = vextq_u8(l3, bord, 1);

	return vminq_u8(vminq_u8(vminq_u8(vminq_u8(v1 ,v2) ,vminq_u8(v3 ,v4)) ,vminq_u8(vminq_u8(v5 ,v6) ,vminq_u8(v7 ,v8))), v9);
}
