#
# $Id: Makefile,v 1.2 2018/09/25 23:57:58 urs Exp $
#

RM     = rm -f
PROTOC = protoc

programs = leb128 pb

.PHONY: all
all: $(programs)

leb128: leb128.o
	$(CC) $(LDFLAGS) -o $@ leb128.o $(LDLIBS)

int.pb.cc int.pb.h: int.proto
	$(PROTOC) --cpp_out=. int.proto

pb: pb.o int.pb.o
	$(CXX) $(LDFLAGS) -o $@ pb.o int.pb.o -lprotobuf $(LDLIBS)

pb.o intpb.o: int.pb.h

.PHONY: clean
clean:
	$(RM) $(programs) int.pb.cc int.pb.h *.o core
