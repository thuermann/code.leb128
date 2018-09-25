/*
 * $Id: leb128.c,v 1.6 2018/09/25 23:54:17 urs Exp $
 */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>

static void usage(const char *name)
{
    fprintf(stderr, "Usage: %s [-u|-z] [-e|-d] code\n", name);
}

typedef long long signed_int;
typedef unsigned long long unsigned_int;

static const int uint_bits = sizeof(unsigned_int) * CHAR_BIT;

static unsigned_int zigzag(signed_int n);
static signed_int de_zigzag(unsigned_int n);

static void unsigned_int_to_leb128(unsigned_int val, unsigned char *s);
static void signed_int_to_leb128(signed_int val, unsigned char *s);
static unsigned_int unsigned_leb128_to_int(const unsigned char *s);
static signed_int signed_leb128_to_int(const unsigned char *s);

static void print_leb128(const unsigned char *leb128);
static void read_leb128(int argc, char **argv, unsigned char *leb128);

int main(int argc, char **argv)
{
    enum { ENCODE, DECODE } mode = ENCODE;
    int errflag = 0, opt_unsigned = 0, opt_zigzag = 0;
    int opt;

    while ((opt = getopt(argc, argv, "uzed")) != -1) {
	switch (opt) {
	case 'u':
	    opt_unsigned = 1;
	    break;
	case 'z':
	    opt_zigzag = 1;
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
	if (opt_zigzag)
	    unsigned_int_to_leb128(zigzag(val), leb128);
	else
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
	if (opt_zigzag)
	    val = de_zigzag(unsigned_leb128_to_int(leb128));
	else
	    val = signed_leb128_to_int(leb128);
	printf("%llx\n", val);
    }

    return 0;
}

/* Google Protobuf encodes signed integers using zigzag encoding and then
 * encoding the result as unsigned LEB128.  Zigzag encoding ensures that
 * signed numbers with small absolute values are mapped to small unsigned
 * numbers.  Non-negative number are simply doubled while for negative
 * numbers the absolute values is doubled and then decremented by one.
 *
 * This is to prevent negative numbers to be always encoded in 10 bytes.
 * However, this is actually not needed since the usual signed LEB128
 * always encodes with the same number of bytes as zigzag encoding
 * following unsigned LEB128 encoding.  However, we implement zigzag
 * since it's in wide-spread use and related to LEB128.
 *
 * BTW, for encoding the Protobuf documentation provides the expression
 *
 *     (n << 1) ^ (n >> 63)
 *
 * which looks nice but has undefined and implementation-defined behavior.
 */
static unsigned_int zigzag(signed_int si)
{
    unsigned_int ui = si;
    return (ui << 1) ^ -(ui >> (uint_bits - 1));
}

static signed_int de_zigzag(unsigned_int ui)
{
    return (ui >> 1) ^ -(ui & 1);
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
