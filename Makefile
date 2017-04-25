IMESCH =./mesch
IDIR = ./inc
CC=gcc
CFLAGS=-I$(IMESCH) -I$(IDIR) -I../PIGPIO -pthread

ODIR=obj
LDIR =../lib

# Add all custom header files here
_DEPS = angleControl.h pid.h motors.h sensors.h kalman.h control.h adafruit_distance.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# Add all C files here.
_OBJ = LittleBro.o pid.o motors.o kalman.o angleControl.o sensors.o control.o adafruit_distance.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

LIBS=-lm -lpigpiod_if2 -lrt

all: LittleBro

$(ODIR)/%.o: %.c mesch
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

LittleBro: $(OBJ) meschach.a
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

testkalman.out: $(ODIR)/testkalman.o $(ODIR)/kalman.o meschach.a
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

testi2c.out: $(ODIR)/testi2c.o
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

testDistance.out: $(ODIR)/testDistance.o sensors.o adafruit_distance.o
	cd obj
	rm testDistance.o
	cd ..
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

testpid.out: $(ODIR)/testpid.o $(ODIR)/pid.o
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
	rm -f *.out

purge: clean
	rm -f meschach.a
	rm -rf mesch/*.o
	rm -rf mesch

uploadLittlebro:
	scp -r ./LittleBro.c pi:/home/pi/code

uploadtesti2c:
	scp -r ./testi2c.c pi:/home/pi/code

uploadtestDistance:
	scp -r ./testDistance.c pi:/home/pi/code

upload: clean
	# Kill Previous Code
	ssh pi 'killall LittleBro || true'
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
