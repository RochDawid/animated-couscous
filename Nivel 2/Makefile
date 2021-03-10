CC=gcc
CFLAGS=-c -g -Wall -std=gnu99
SOURCES=mi_mkfs.c bloques.c leer_sf.c
LIBRARIES=bloques.o
INCLUDES=bloques.h
PROGRAMS=mi_mkfs leer_sf
OBJS=$(SOURCES:.c=.o)
all: $(OBJS) $(PROGRAMS)
$(PROGRAMS): $(LIBRARIES) $(INCLUDES)
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf *.o *~ $(PROGRAMS) disco* ext*