SHELL = /bin/sh
CC = g++
CFLAGS = -Wall
AR = ar
LIBNAME = libgoodmalloc.a
OBJFILES = goodmalloc.o

mergesort: mergesort.o library
	$(CC) $(CFLAGS) -o mergesort mergesort.o -L. -lgoodmalloc

mergesort.o: mergesort.cpp
	$(CC) $(CFLAGS) -c mergesort.cpp

library: $(OBJFILES)
	$(AR) rcs $(LIBNAME) $(OBJFILES)

$(OBJFILES): goodmalloc.cpp
	$(CC) $(CFLAGS) -c goodmalloc.cpp

RM = rm -f

clean:
	-$(RM) *.o

distclean:
	-$(RM) *.o $(LIBNAME) mergesort