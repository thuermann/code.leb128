/*
 * $Id: leb128.c,v 1.5 2018/09/25 23:54:08 urs Exp $
 */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>

static void usage(const char *name)
{
    fprintf(stderr, "Usage: %s [-u] [-e|-d] code\n", name);
}

typedef long long signed_int;
typedef unsigned long long unsigned_int;

static const int uint_bits = sizeof(unsigned_int) * CHAR_BIT;

static void unsigned_int_to_leb128(unsigned_int val, unsigned char *s);
static void signed_int_to_leb128(signed_int val, unsigned char *s);
static unsigned_int unsigned_leb128_to_int(const unsigned char *s);
static signed_int signed_leb128_to_int(const unsigned char *s);

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
	unsigned_int val = strtoull(argv[optind], NULL, 0);
	unsigned_int_to_leb128(val, leb128);
	print_leb128(leb128);
    } else if (mode == ENCODE && !opt_unsigned) {
	unsigned char leb128[16];
	signed_int val = strtoll(argv[optind], NULL, 0);
	signed_int_to_leb128(val, leb128);
	print_leb128(leb128);
    } else if (mode == DECODE && opt_unsigned) {
	unsigned char leb128[16];
	unsigned_int val;
	read_leb128(argc - optind, argv + optind, leb128);
	val = unsigned_leb128_to_int(leb128);
	printf("%llx\n", val);
    } else if (mode == DECODE && !opt_unsigned) {
	unsigned char leb128[16];
	signed_int val;
	read_leb128(argc - optind, argv + optind, leb128);
	val = signed_leb128_to_int(leb128);
	printf("%llx\n", val);
    }

    return 0;
}

static void unsigned_int_to_leb128(unsigned_int val, unsigned char *s)
{
    unsigned char c;
    int more;

    do {
	c = val & 0x7f;
	val >>= 7;
	more = val != 0;
#ifdef DEBUG
	fprintf(stderr, "%.2x %llx\n", c, val);
#endif
	*s++ = c | (more ? 0x80 : 0);
    } while (more);
}

static void signed_int_to_leb128(signed_int val, unsigned char *s)
{
    unsigned char c;
    int more;

    do {
	c = val & 0x7f;
	val >>= 7;
	more = c & 0x40 ? val != -1 : val != 0;
#ifdef DEBUG
	fprintf(stderr, "%.2x %llx\n", c, val);
#endif
	*s++ = c | (more ? 0x80 : 0);
    } while (more);
}

static unsigned_int unsigned_leb128_to_int(const unsigned char *s)
{
    unsigned_int c, val = 0;
    int shift = 0;
    int more;

    do {
	c = *s++;
#ifdef DEBUG
	fprintf(stderr, "%.2llx %llx\n", c, val);
#endif
	val |= (c & 0x7f) << shift;
	shift += 7;
	more = c & 0x80;
    } while (more);

    return val;
}

static signed_int signed_leb128_to_int(const unsigned char *s)
{
    unsigned_int c, val = 0;
    int shift = 0;
    int more;

    do {
	c = *s++;
#ifdef DEBUG
	fprintf(stderr, "%.2llx %llx\n", c, val);
#endif
	val |= (c & 0x7f) << shift;
	shift += 7;
	more = c & 0x80;
    } while (more);

    if (c & 0x40 && shift < uint_bits)
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
