
// cmd ln args implementation

#include "cmdargs.h"




CmdArgs::CmdArgs()
	: windowWidth(1024), windowHeight(768), imageWidth(200), imageHeight(100), raysPerPixel(1), maxBounces(5), numSpheres(12)
{
	// blank
	// Number of spheres is actually multiplied by 3 - therefore initial test has 12 * 3 = 36 spheres
}

CmdArgs::CmdArgs(int a) // a is discarded
	: windowWidth(1000), windowHeight(500), imageWidth(1000), imageHeight(500), raysPerPixel(1), maxBounces(5), numSpheres(3334)
{
	// Number of spheres is actually multiplied by 3 - therefore initial test has 3334 * 3 = 10002 spheres
	// blank
}



CmdArgs::CmdArgs(unsigned int envVariables[], unsigned int arraySize)
{
	// NEEDS TO BE REWORKED - MAYBE PROFILES? CHECK SWITCHES, INCOMING DATA AND ARRAY SIZE!!!!!
	// REMEMBER THAT NR OF SPHERES IS ACTUALLY x3

	//CHECK IF BELOW 0

	// Command Line Arg Guards
	// If user overflows unsigned int behaviour is defined & values should wrap around

	//if (windoww <= 2560)
	//{
	//	windowWidth = windoww;
	//}
	//else
	//{
	//	windowWidth = 2560;
	//}
	//if (windowh <= 1440)
	//{
	//	windowHeight = windowh;
	//}
	//else
	//{
	//	windowHeight = 1440;
	//}


	//if (imagew <= 2560)
	//{
	//	imageWidth = imagew;
	//}
	//else
	//{
	//	imageWidth = 2560;
	//}
	//if (imageh <= 1440)
	//{
	//	imageHeight = imageh;
	//}
	//else
	//{
	//	imageHeight = 1440;
	//}

	//if (rays <= 50)
	//{
	//	raysPerPixel = rays;
	//}
	//else
	//{
	//	raysPerPixel = 50;
	//}

	//if (bounces <= 10)
	//{
	//	maxBounces = bounces;
	//}
	//else
	//{
	//	maxBounces = 10;
	//}

	//if (spheres <= 20000)
	//{
	//	numSpheres = spheres;
	//}
	//else
	//{
	//	numSpheres = 20000;
	//}


}