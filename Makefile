#
# $Id: Makefile,v 1.3 2018/09/27 20:47:40 urs Exp $
#

RM     = rm -f
PROTOC = protoc

programs = leb128 pb tst-zigzag

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

.PHONY: clean
clean:
	$(RM) $(programs) int.pb.cc int.pb.h *.o core
