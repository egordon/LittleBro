# LittleBro
Code for the Brown Robotics Olympiad 2017

Installation
==

* Install [Pigpio](http://abyz.co.uk/rpi/pigpio/download.html)
* **IF ON OS X:**: run ```make mesch```, edit ```mesch/machine.h```, and comment out the line ```#define HAVE_MALLOC_H 1```
* Just run ```make```.
* Clean up old builds with ```make clean```.
* Purge Meschach with ```make purge```.

Add Files
==
* Add a new C file
* Put Headers in ./inc
* Edit Makefile: add header to DEPS, add C file as .o to OBJ
