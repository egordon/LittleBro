IMESCH =./mesch
IDIR = ./inc
CC=gcc
CFLAGS=-I$(IMESCH) -I$(IDIR)

ODIR=obj
LDIR =../lib

# Add all custom header files here
_DEPS = angleControl.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# Add all C files here.
_OBJ = LittleBro.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

LIBS=-lm

$(ODIR)/%.o: %.c
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)


LittleBro: $(OBJ) meschach.a
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

meschach.a:
	mkdir -p mesch
	wget http://homepage.divms.uiowa.edu/~dstewart/meschach/mesch12b.tar.gz
	tar -zxvf mesch12b.tar.gz -C mesch
	$(MAKE) -C mesch
	mv mesch/meschach.a .
	rm -f mesch12b.tar.gz


.PHONY: clean
clean:
	rm -f $(ODIR)/*.o *~ $(INCDIR)/*~
	rm -f LittleBro

purge: clean
	rm -f meschach.a
	rm -rf mesch
