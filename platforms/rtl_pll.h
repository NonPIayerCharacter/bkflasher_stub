#ifndef RTL_PLL_H
#define RTL_PLL_H

#include <stdint.h>

static inline uint32_t rtl_pll_fraction(uint32_t remainder, uint32_t divisor)
{
	/* The caller supplies a remainder below divisor, which is XTAL_ClkGet() >> 3.
	 * Doubling it is safe in uint32_t. Emit one quotient bit per iteration. */
	uint32_t fraction = 0;
	for(unsigned bit = 0; bit < 13; ++bit)
	{
		remainder <<= 1;
		fraction <<= 1;
		if(remainder >= divisor)
		{
			remainder -= divisor;
			fraction |= 1;
		}
	}
	return fraction;
}

#endif
