#include "valgo.h"

vuint8 vmulq_sat_u8(vuint8 a, vuint8 b) 
{
	// a = 2	2	2 	... 2
	// b = 1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16

	vuint8 vZero = vmovq_n_u8(0);
	vuint16 max = vmovq_n_u16(255);
	// 2 ... 2
	vuint8 aGauche = vzip1q_u8(a, vZero);
	vuint8 aDroite = vzip2q_u8(a, vZero);
	
	// 1	2	3	4	5	6	7	8
	vuint8 bGauche = vzip1q_u8(b, vZero);
	
	// 9	10	11	12	13	14	15	16
	vuint8 bDroite = vzip2q_u8(b, vZero);

	// 2	0	4	0	6	0	8	0	10	0	12	0	14	0	16	0
	vuint8 resGauche = vreinterpretq_u8_u16(vminq_u16(vmulq_u16(vreinterpretq_u16_u8(aGauche), vreinterpretq_u16_u8(bGauche)), max));
	// 18	0	20	0	22	0	24	0	26	0	28	0	30	0	32	0
	vuint8 resDroite = vreinterpretq_u8_u16(vminq_u16(vmulq_u16(vreinterpretq_u16_u8(aDroite), vreinterpretq_u16_u8(bDroite)), max));

	// 2	4	6	8	10	12	14	16	18	20	22	24	26	28	30	32
	return vuzp1q_u8(resGauche, resDroite);
}
