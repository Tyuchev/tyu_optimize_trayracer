#include <stdio.h>
#include "window.h"
#include "vec3.h"
#include "raytracer.h"
#include "sphere.h"
#include "cmdArgs.h"
#include "random.h"

#include <iostream>
#include <chrono>
#include <vector>
#include <array>
#include <memory>
#include <cmath>
#include <random>
#include <algorithm>
#include <thread>
#include <atomic>


#define degtorad(angle) angle * MPI / 180

// Add commands to README

// Remember number of spheres is multiplied by 3 ATM


int main(int argc, char* argv[])
{
    // Time the creation of objects
    std::chrono::high_resolution_clock wallClock{};
    auto setupBegin = wallClock.now();
    
    //initialise with standard settings - then check for input
    CmdArgs cmdArgs{};
    
    if (argc == 5)
    {
        char* arg[] = { argv[1], argv[2], argv[3], argv[4] };

        // Store cmdArgs in class (has default values if not provided with args)
        cmdArgs.updateEnvVariables(arg);
    }
    else if (argc > 1) // checks for too little or too much input
    {

        std::cout << "This program will only accept 4 command line args" << std::endl;
        std::cout << "Executing with standard parameters: " << std::endl;
        std::cout << "imageWidth = 200, imageHeight = 100, raysPerPixel = 1, numSpheres = 12(* 3)" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(3));
    }


//#define DEBUG 1
#ifdef DEBUG

    Display::Window wnd;
    wnd.SetSize(cmdArgs.windowWidth, cmdArgs.windowHeight);

    if (!wnd.Open())
    {
        return 1;
    }

#endif // DEBUG Window


    // Simulation level Sphere holder to be passed by const ref
    std::vector<Sphere> sphereHolder;
    sphereHolder.reserve(static_cast<size_t>((static_cast<int>(cmdArgs.numSpheres) * 3) + 1)); // current algorithm is num * 3 plus 1 for the floor


    std::vector<Color> framebuffer;
    // initial resolution is 200x100
    framebuffer.resize(static_cast<size_t>(cmdArgs.imageWidth) * static_cast<size_t>(cmdArgs.imageHeight));


    // Create Raytracer
    Raytracer rt{ (unsigned int)cmdArgs.imageWidth, (unsigned int)cmdArgs.imageHeight, framebuffer, (unsigned int)cmdArgs.raysPerPixel, (unsigned int)cmdArgs.maxBounces, sphereHolder };
    RandomGen randomGen;


    // Create some objects
    Material mat{};
    mat.type = "Lambertian";
    mat.color = { 0.5,0.5,0.5 };
    mat.roughness = 0.3;
    Sphere sphere{ 1000, vec3{ 0,-1000, -1 }, mat };
    sphereHolder.push_back(sphere);
    std::cout << std::endl;

    // Deterministic random sphere generation

    for (int it = 0; it < cmdArgs.numSpheres; it++) // num Spheres * 3 
    {
        {
            Material mat{};
                mat.type = "Lambertian";
                float r = randomGen.RandomFloat();
                float g = randomGen.RandomFloat();
                float b = randomGen.RandomFloat();
                mat.color = { r,g,b };
                mat.roughness = randomGen.RandomFloat();
                const double span = 10.0f;
                Sphere sphere{ randomGen.RandomFloat() * 1.5f + 0.5f, vec3{randomGen.RandomFloatNTP() * span, randomGen.RandomFloat() * span + 0.2f, randomGen.RandomFloatNTP() * span}, mat };
            sphereHolder.push_back(sphere);
        }
        {
            Material mat{};
            mat.type = "Conductor";
            float r = randomGen.RandomFloat();
            float g = randomGen.RandomFloat();
            float b = randomGen.RandomFloat();
            mat.color = { r,g,b };
            mat.roughness = randomGen.RandomFloat();
            const double span = 30.0f;
            Sphere sphere{ randomGen.RandomFloat() * 1.5f + 0.5f,
                vec3{
                    randomGen.RandomFloatNTP() * span,
                    randomGen.RandomFloat() * span + 0.2f,
                    randomGen.RandomFloatNTP() * span
                },
                mat };
            sphereHolder.push_back(sphere);
        }
        {
            Material mat{};
            mat.type = "Dielectric";
            float r = randomGen.RandomFloat();
            float g = randomGen.RandomFloat();
            float b = randomGen.RandomFloat();
            mat.color = { r,g,b };
            mat.roughness = randomGen.RandomFloat();
            const double span = 25.0f;
            Sphere sphere{
                randomGen.RandomFloat() * 1.5f + 0.5f,
                vec3{
                    randomGen.RandomFloatNTP() * span,
                    randomGen.RandomFloat() * span + 0.2f,
                    randomGen.RandomFloatNTP() * span
                },
                mat};
            sphereHolder.push_back(sphere);
        }
    }

    // number of accumulated frames
    int frameIndex = 0;

    std::vector<Color> framebufferCopy;
    framebufferCopy.resize(static_cast<size_t>(cmdArgs.imageWidth) * static_cast<size_t>(cmdArgs.imageHeight));


    // Num of Rays per frame
    float numberMegaRays = framebuffer.size() * (cmdArgs.raysPerPixel * 0.000001f);
    float megaRaysPerSec = 0.0f;


    // End of setup timer
    float setupTimer = (std::chrono::duration_cast<std::chrono::microseconds>(wallClock.now() - setupBegin)).count() * 0.000001;
    // Output setup time of objects
    std::cout << "Setup Time: " << setupTimer << " seconds" << std::endl;


    // Create time points for render loop
    std::chrono::high_resolution_clock::time_point renderBegin;
    std::chrono::high_resolution_clock::time_point renderEnd;
    double renderTimer{ 0.0 };


    // rendering loop
    while (true)
    {
        renderBegin = wallClock.now();        

#ifdef DEBUG
        wnd.Update();
#endif
        // Raytracing starts here 
        rt.Raytrace();
        frameIndex++;

        // Get the average distribution of all samples
        // SIMD required
        {
            size_t p = 0;
            for (Color const& pixel : framebuffer)
            {
                framebufferCopy[p] = pixel;
                framebufferCopy[p].r /= frameIndex;
                framebufferCopy[p].g /= frameIndex;
                framebufferCopy[p].b /= frameIndex;
                p++;
            }
        }


#ifdef DEBUG
        glClearColor(0, 0, 0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        wnd.Blit((float*)&framebufferCopy[0], cmdArgs.imageWidth, cmdArgs.imageHeight);
        wnd.SwapBuffers();
#endif // DEBUG


        // Timers & Debug info
        renderEnd = wallClock.now();
        renderTimer = (std::chrono::duration_cast<std::chrono::microseconds>(renderEnd - renderBegin)).count() * 0.000001;

        // MraysPerSec
        megaRaysPerSec = numberMegaRays *  (1.0f / renderTimer);

        std::cout << "Render Time: " << renderTimer << " seconds" << std::endl;
        std::cout << "Mrays/sec: " << megaRaysPerSec << "\n" << std::endl;
    }


    // Cleanup
#ifdef DEBUG
    wnd.Close();
#endif // DEBUG


    return 0;
}

