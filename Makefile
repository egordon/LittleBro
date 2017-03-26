IMESCH =./mesch
IDIR = ./inc
CC=gcc
CFLAGS=-I$(IMESCH) -I$(IDIR)

ODIR=obj
LDIR =../lib

# Add all custom header files here
_DEPS = angleControl.h pid.h motors.h sensor.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# Add all C files here.
_OBJ = LittleBro.o pid.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

LIBS=-lm

$(ODIR)/%.o: %.c mesch
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)


pid: $(ODIR)/pid.o
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

LittleBro: $(OBJ) meschach.a
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

meschach.a: mesch
	$(MAKE) -C mesch
	cp mesch/meschach.a .

mesch:
	mkdir -p mesch
	curl -OL http://homepage.divms.uiowa.edu/~dstewart/meschach/mesch12b.tar.gz
	tar -zxvf mesch12b.tar.gz -C mesch
	rm -f mesch12b.tar.gz


.PHONY: clean
clean:
	rm -f $(ODIR)/*.o *~ $(INCDIR)/*~ *.o
	rm -f LittleBro
	rm -f pid

purge: clean
	rm -f meschach.a
	rm -rf mesch
