



struct Cmdargs
{
	unsigned int windowHeight;
	unsigned int windowWidth;
	unsigned int imageHeight;
	unsigned int imageWidth;
	unsigned int raysPerPixel;
	unsigned int maxBounces;
	unsigned int numSpheres;


	Cmdargs(unsigned int windoww = 1024, unsigned int windowh = 768, unsigned int imagew = 200, unsigned int imageh = 100, unsigned int rays = 1, unsigned int bounces = 5, unsigned int spheres = 12)
	{
		// Command Line Arg Guards
		// If user overflows unsigned int behaviour is defined & values should wrap around

		if (windoww <= 2560)
		{
			windowWidth = windoww;
		}
		else
		{
			windowWidth = 2560;
		}
		if (windowh <= 1440)
		{
			windowHeight = windowh;
		}
		else
		{
			windowHeight = 1440;
		}


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

		if (rays <= 50)
		{
			raysPerPixel = rays;
		}
		else
		{
			raysPerPixel = 50;
		}

		if (bounces <= 10)
		{
			maxBounces = bounces;
		}
		else
		{
			maxBounces = 10;
		}

		if (spheres <= 100000)
		{
			numSpheres = spheres;
		}
		else
		{
			numSpheres = 100000;
		}


	}



};