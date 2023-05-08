
// cmd ln args implementation

#include "cmdargs.h"
#include <vector>
#include <cstdlib>
#include <iostream>




CmdArgs::CmdArgs()
	: windowWidth(1000), windowHeight(500), imageWidth(200), imageHeight(100), raysPerPixel(1), maxBounces(5), numSpheres(12)
{
	// blank
	// Number of spheres is actually multiplied by 3 - therefore initial test has 12 * 3 = 36 spheres
	// 720,000 approx intersection checks
	// Render time 0.00055
	// tests per second = 720,000 / rendertime = 1,309,090,909
}

void CmdArgs::updateEnvVariables(char* envVariables[])
{
	// atoi does not throw exeptions
	// program will UB/crash if atoi doesnt return a valid int
	int imagew = atoi(envVariables[0]);
	int imageh = atoi(envVariables[1]);
	int rays = atoi(envVariables[2]);
	int spheres = atoi(envVariables[3]);


	if (imagew <= 2560)
	{
		imageWidth = imagew;
	}
	else
	{
		imageWidth = 2560;
	}
	if (imageh <= 1440)
	{
		imageHeight = imageh;
	}
	else
	{
		imageHeight = 1440;
	}

	if (rays <= 20)
	{
		raysPerPixel = rays;
	}
	else
	{
		raysPerPixel = 50;
	}

	if (spheres <= 20000)
	{
		numSpheres = spheres;
	}
	else
	{
		numSpheres = 20000;
	}
}


// Debuging constructor - depracated
//CmdArgs::CmdArgs(int a) // a is discarded
//	: windowWidth(1000), windowHeight(500), imageWidth(1000), imageHeight(500), raysPerPixel(20), maxBounces(5), numSpheres(3334)
//{
//	// blank
//	// Number of spheres is actually multiplied by 3 - therefore initial test has 3334 * 3 = 10002 spheres
//
//	// 100,000,000,000 (100 billion)
//	// Render time 296
//	// tests per second = 100 billion / rendertime = 333,333,333
//}