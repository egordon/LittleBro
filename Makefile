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


.PHONY: clean purge upload
clean:
	rm -rf $(ODIR) *~ $(INCDIR)/*~ *.o
	rm -f LittleBro

purge: clean
	rm -f meschach.a
	rm -rf mesch/*.o
	rm -rf mesch

upload: clean
	# Kill Previous Code
	ssh pi 'killall LitleBro || true'
	ssh pi 'cd code; make clean'
	# Upload New Code
	scp -r ./*.c pi:/home/pi/code
	scp -r ./Makefile pi:/home/pi/code
	scp -r ./inc pi:/home/pi/code
	# Run New Code
	ssh pi 'cd code; make'
	ssh pi './code/LittleBro </dev/null >/home/pi/run.log 2>&1 &'

log:
	ssh pi 'tail -f run.log' || true
