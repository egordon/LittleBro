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

Upload Code
==
* **USE A UNIX MACHINE!**
* Add the folowing to `~/.ssh/config`:
```
Host pi
        HostName 169.254.64.64
        User pi
        Port 22
        ForwardX11 yes
        IdentityFile ~/.ssh/littlebro.pem
```
* Get litlebro.pem from Ethan, add to your .ssh folder, and set permissions to 400:
```
chmod 400 ~/.ssh/littlebro.pem
```
* Plug in the Pi Zero, on Linux, make sure the connection is Link-Local, on Mac, this is done automatically.
* You should now be able to ssh into the pi with ```ssh pi```
* Run ```make upload``` to kill the existing process, upload new code, and start an new process.
* Run ```make log``` to follow the on-pi log file.
