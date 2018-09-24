/*
 * $Id: leb128.c,v 1.3 2018/09/24 21:00:43 urs Exp $
 */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>

static void usage(const char *name)
{
    fprintf(stderr, "Usage: %s [-u] [-e|-d] code\n", name);
}

static void unsigned_long_to_leb128(unsigned long val, unsigned char *s);
static void signed_long_to_leb128(long val, unsigned char *s);
static unsigned long unsigned_leb128_to_long(const unsigned char *s);
static long signed_leb128_to_long(const unsigned char *s);

static void print_leb128(const unsigned char *leb128);
static void read_leb128(int argc, char **argv, unsigned char *leb128);

int main(int argc, char **argv)
{
    enum { ENCODE, DECODE } mode = ENCODE;
    int errflag = 0, opt_unsigned = 0;
    int opt;

    while ((opt = getopt(argc, argv, "ued")) != -1) {
	switch (opt) {
	case 'u':
	    opt_unsigned = 1;
	    break;
	case 'e':
	    mode = ENCODE;
	    break;
	case 'd':
	    mode = DECODE;
	    break;
	default:
	    errflag = 1;
	    break;
	}
    }
    if (errflag || optind == argc) {
	usage(argv[0]);
	exit(1);
    }

    if (mode == ENCODE && opt_unsigned) {
	unsigned char leb128[16];
	unsigned long val = strtoul(argv[optind], NULL, 0);
	unsigned_long_to_leb128(val, leb128);
	print_leb128(leb128);
    } else if (mode == ENCODE && !opt_unsigned) {
	unsigned char leb128[16];
	long val = strtol(argv[optind], NULL, 0);
	signed_long_to_leb128(val, leb128);
	print_leb128(leb128);
    } else if (mode == DECODE && opt_unsigned) {
	unsigned char leb128[16];
	unsigned long val;
	read_leb128(argc - optind, argv + optind, leb128);
	val = unsigned_leb128_to_long(leb128);
	printf("%lx\n", val);
    } else if (mode == DECODE && !opt_unsigned) {
	unsigned char leb128[16];
	long val;
	read_leb128(argc - optind, argv + optind, leb128);
	val = signed_leb128_to_long(leb128);
	printf("%lx\n", val);
    }

    return 0;
}

static void unsigned_long_to_leb128(unsigned long val, unsigned char *s)
{
    unsigned char c;
    int more;

    do {
	c = val & 0x7f;
	val >>= 7;
	more = val != 0;
#ifdef DEBUG
	fprintf(stderr, "%.2x %lx\n", c, val);
#endif
	*s++ = c | (more ? 0x80 : 0);
    } while (more);
}

static void signed_long_to_leb128(long val, unsigned char *s)
{
    unsigned char c;
    int more;

    do {
	c = val & 0x7f;
	val >>= 7;
	more = c & 0x40 ? val != -1 : val != 0;
#ifdef DEBUG
	fprintf(stderr, "%.2x %lx\n", c, val);
#endif
	*s++ = c | (more ? 0x80 : 0);
    } while (more);
}

static unsigned long unsigned_leb128_to_long(const unsigned char *s)
{
    unsigned long c, val = 0;
    int shift = 0;
    int more;

    do {
	c = *s++;
#ifdef DEBUG
	fprintf(stderr, "%.2lx %lx\n", c, val);
#endif
	val |= (c & 0x7f) << shift;
	shift += 7;
	more = c & 0x80;
    } while (more);

    return val;
}

static long signed_leb128_to_long(const unsigned char *s)
{
    unsigned long c, val = 0;
    int shift = 0;
    int more;

    do {
	c = *s++;
#ifdef DEBUG
	fprintf(stderr, "%.2lx %lx\n", c, val);
#endif
	val |= (c & 0x7f) << shift;
	shift += 7;
	more = c & 0x80;
    } while (more);

    if (c & 0x40 && shift < sizeof(unsigned long) * CHAR_BIT)
	val |= ~0UL << shift;

    return val;
}

static void print_leb128(const unsigned char *leb128)
{
    for (int i = 0, more = 1; more; i++) {
	more = leb128[i] & 0x80;
	printf(more ? "%.2x " : "%.2x\n", leb128[i]);
    }
}

static void read_leb128(int argc, char **argv, unsigned char *leb128)
{
    for (int i = 0; i < argc; i++) {
	unsigned long val = strtoul(argv[i], NULL, 0);
	if (val >= 0x100)
	    return;
	leb128[i] = val;
	if (val < 0x80)
	    break;
    }
}
