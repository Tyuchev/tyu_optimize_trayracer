# Trayracer

My attempt at optimizing the below project;

Really really poorly written (by design) raytracer for use as a worst case scenario within programming courses.


## My Cmd Args

CmdArg Key:
	cmdArg[0] == imageWidth;
	cmdArg[1] == imageHeight;
	cmdArg[2] == raysPerPixel;
	cmdArg[3] == numSpheres;

Note!: If less than 4 args are provided values will be set to default.

Defaults:
		imageWidth = 1000;
		imageHeight = 500;
		raysPerPixel = 1;
		numSpheres = 12;

Note!: If more than 4 args are provided, anything more than the first 4 will be ignored


## Build instructions

Windows:

* You can either generate visual studio solution by using CMake, or just open the folder within VSCode and build/run.

Linux:

* Untested, but should run with CMake and possibly even via VSCode.

If you have problems with missing libraries, install (sudo apt-get install ...):
* libgl-dev
* xorg-dev
* libglu1-mesa-dev
* gdb
* freeglut3-dev

VSCode requires the C/C++ extension to be able to use the debugger.
