
// Cmd ln args class header

#include <vector>



class CmdArgs
{
public:


	unsigned int windowWidth;
	unsigned int windowHeight;
	unsigned int imageWidth;
	unsigned int imageHeight;
	unsigned int raysPerPixel;
	unsigned int maxBounces;
	unsigned int numSpheres;


	CmdArgs();
	CmdArgs(unsigned int envVariabes[], unsigned int arrraySize);



};