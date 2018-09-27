/*
 * $Id: tst-zigzag.c,v 1.1 2018/09/27 20:47:40 urs Exp $
 */

#include <stdio.h>
#include <stdint.h>
#include <limits.h>

static unsigned long long zigzag(signed long long si)
{
    unsigned long long ui = si;
    return (ui << 1) ^ -(ui >> (64 - 1));
}

static signed long long de_zigzag(unsigned long long ui)
{
    return (ui >> 1) ^ -(ui & 1);
}

static uint32_t zigzag32(int32_t si)
{
    uint32_t ui = si;
    return (ui << 1) ^ -(ui >> (32 - 1));
}

static int32_t de_zigzag32(uint32_t ui)
{
    return (ui >> 1) ^ -(ui & 1);
}

int main(void)
{
    for (long long n = INT_MIN; n <= INT_MAX; n++) {
	unsigned long long z = zigzag(n);

	/* Check whether the return val of zigzag() is correct. */
	if (z > UINT_MAX)
	    printf("%lld: out of range\n", n);
	if (n >= 0 && z != 2 * n)
	    printf("%lld: wrong val\n", n);
	if (n < 0 && z != -2 * n - 1)
	    printf("%lld: wrong val\n", n);

	/* Check whether de_zigzag(zigzag(n)) is indeeed the identity. */
	if (de_zigzag(z) != n)
	    printf("%lld: not inv\n", n);

	/* Check whether 32 and 64 bits versions are the same. */
	if (zigzag32(n) != z || de_zigzag32(z) != de_zigzag(z))
	    printf("%lld: 64 != 32\n", n);
    }
    return 0;
}
