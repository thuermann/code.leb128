#
# $Id: Makefile,v 1.4 2018/09/30 02:50:27 urs Exp $
#

INSTALL = /usr/bin/install
RM      = rm -f
PROTOC  = protoc

programs = leb128 pb tst-zigzag
prefix   = /usr/local

.PHONY: all
all: $(programs)

leb128: leb128.o
	$(CC) $(LDFLAGS) -o $@ leb128.o $(LDLIBS)

int.pb.cc int.pb.h: int.proto
	$(PROTOC) --cpp_out=. int.proto

pb: pb.o int.pb.o
	$(CXX) $(LDFLAGS) -o $@ pb.o int.pb.o -lprotobuf $(LDLIBS)

pb.o intpb.o: int.pb.h

tst-zigzag: tst-zigzag.o
	$(CC) $(LDFLAGS) -o $@ tst-zigzag.o $(LDLIBS)

.PHONY: install
install:
	$(INSTALL) -m 755 leb128   $(prefix)/bin
	$(INSTALL) -m 644 leb128.1 $(prefix)/man/man1

.PHONY: clean
clean:
	$(RM) $(programs) int.pb.cc int.pb.h *.o core
