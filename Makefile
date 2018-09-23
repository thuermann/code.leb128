#
# $Id: Makefile,v 1.1 2018/09/23 08:50:37 urs Exp $
#

RM = rm -f

programs = leb128

.PHONY: all
all: $(programs)

leb128: leb128.o
	$(CC) $(LDFLAGS) -o $@ leb128.o $(LDLIBS)

.PHONY: clean
clean:
	$(RM) $(programs) *.o core
