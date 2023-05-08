
// Cmd ln args class header

#include <vector>
#include <cstdlib>
#include <iostream>



class CmdArgs
{
public:


	int windowWidth;
	int windowHeight;
	int imageWidth;
	int imageHeight;
	int raysPerPixel;
	int maxBounces;
	int numSpheres;


	CmdArgs();

	void updateEnvVariables(char* envVariables[]);



};