//
// $Id: pb.cc,v 1.1 2018/09/25 23:57:58 urs Exp $
//

#include <iostream>
#include <cstdlib>

#include <unistd.h>

#include "int.pb.h"

static void usage(const char *name)
{
    std::cerr << "Usage: " << name << " [-e|-d] ...\n";
}

int main(int argc, char **argv)
{
    enum { ENCODE, DECODE } mode = ENCODE;
    int errflag = 0;
    int opt;

    while ((opt = getopt(argc, argv, "ed")) != -1) {
	switch (opt) {
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

    if (errflag
	|| mode == ENCODE && argc - optind != 1
	|| mode == DECODE && argc - optind != 0) {
	usage(argv[0]);
	exit(1);
    }

    if (mode == ENCODE) {
	integers::ints msg;
	long long val = strtol(argv[optind], NULL, 0);
	msg.set_ui(val);
	msg.set_i(val);
	msg.set_si(val);
	msg.SerializeToOstream(&std::cout);
    } else {
	integers::ints msg;
	msg.ParseFromIstream(&std::cin);
	std::cout << std::hex
		  << msg.ui() << ' ' << msg.i() << ' ' << msg.si() << '\n';
    }

    return 0;
}
