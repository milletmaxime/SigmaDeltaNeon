#include "algo.h"
#include "vnrdef.h"
#include "vnrutil.h"
#include "nrutil.h"

static inline void set_border(vuint8** m, int vi0, int vi1, int vj0, int vj1, vuint8 value)
{
	for(int i = vi0 - 1 ; i < vi1 + 1 ; i++)
		for(int j = vj0 - 1 ; j < vj1 + 1 ; j++)
			m[i][j] = value;
}

vuint8** erosion_SIMD(vuint8** m, int vi0, int vi1, int vj0, int vj1)
{
	vuint8** res = vui8matrix(vi0 -1, vi1 +1, vj0 -1, vj1 +1);
	set_border(m, vi0, vi1, vj0, vi1, vmovq_n_u8(Vmax));
	
	return res;
}

vuint8** dilatation_SIMD(vuint8** m, int vi0, int vi1, int vj0, int vj1)
{
	vuint8** res = vui8matrix(vi0 -1, vi1 +1, vj0 -1, vj1 +1);
	set_border(m, vi0, vi1, vj0, vi1, vmovq_n_u8(Vmin));
	
	return res;
}
