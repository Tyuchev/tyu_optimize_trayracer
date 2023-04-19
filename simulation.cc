#include <stdio.h>
#include "window.h"
#include "vec3.h"
#include "raytracer.h"
#include "sphere.h"
#include "cmdArgs.h"

#include <iostream>
#include <chrono>
#include <vector>
#include <array>

#define degtorad(angle) angle * MPI / 180

// Uni PC = Ryzen Threadripper PRO 3955wx 16 Cores

// Add average timer later?

// Add --switches: --initial & --FreTest switches to cmdlnargs to allow for different setup states
// --help to show options

// Add commands to README

// Remember number of spheres is multiplied by 3 ATM

int main()
{
    // Time the creation of objects
    std::chrono::high_resolution_clock wallClock{};
    auto setupBegin = wallClock.now();

    // Store cmdArgs in class (has default values if not provided with args)
    CmdArgs cmdArgs{};


    Display::Window wnd;
    wnd.SetSize(cmdArgs.windowWidth, cmdArgs.windowHeight);
    
    if (!wnd.Open())
        return 1;


    std::vector<Sphere> sphereHolder;
    sphereHolder.reserve(static_cast<size_t>((static_cast<int>(cmdArgs.numSpheres) * 3) + 1)); // current algorithm is num * 3 plus 1 for the floor


    std::vector<Color> framebuffer;
    // initial resolution is 200x100
    // is this actual image resolution?
    framebuffer.resize(static_cast<size_t>(cmdArgs.imageWidth) * static_cast<size_t>(cmdArgs.imageHeight));



    // Create Raytracer
    Raytracer rt = Raytracer(cmdArgs.imageWidth, cmdArgs.imageHeight, framebuffer, cmdArgs.raysPerPixel, cmdArgs.maxBounces);


    // Create some objects
    Material mat{};
    mat.type = "Lambertian";
    mat.color = { 0.5,0.5,0.5 };
    mat.roughness = 0.3;
    Sphere sphere{ 1000, vec3{ 0,-1000, -1 }, mat };
    sphereHolder.push_back(sphere);
    std::cout << std::endl;

    // Deterministic random sphere generation

    for (int it = 0; it < 12; it++)
    {
        {
            Material mat{};
                mat.type = "Lambertian";
                float r = RandomFloat();
                float g = RandomFloat();
                float b = RandomFloat();
                mat.color = { r,g,b };
                mat.roughness = RandomFloat();
                const double span = 10.0f;
                Sphere sphere{ RandomFloat() * 0.7f + 0.2f, vec3{RandomFloatNTP() * span, RandomFloat() * span + 0.2f, RandomFloatNTP() * span}, mat };
            sphereHolder.push_back(sphere);
        }
        {
            Material mat{};
            mat.type = "Conductor";
            float r = RandomFloat();
            float g = RandomFloat();
            float b = RandomFloat();
            mat.color = { r,g,b };
            mat.roughness = RandomFloat();
            const double span = 30.0f;
            Sphere sphere{ RandomFloat() * 0.7f + 0.2f,
                vec3{
                    RandomFloatNTP() * span,
                    RandomFloat() * span + 0.2f,
                    RandomFloatNTP() * span
                },
                mat };
            sphereHolder.push_back(sphere);
        }
        {
            Material mat{};
            mat.type = "Dielectric";
            float r = RandomFloat();
            float g = RandomFloat();
            float b = RandomFloat();
            mat.color = { r,g,b };
            mat.roughness = RandomFloat();
            mat.refractionIndex = 1.65;
            const double span = 25.0f;
            Sphere sphere{
                RandomFloat() * 0.7f + 0.2f,
                vec3{
                    RandomFloatNTP() * span,
                    RandomFloat() * span + 0.2f,
                    RandomFloatNTP() * span
                },
                mat};
            sphereHolder.push_back(sphere);
        }
    }
    
    bool exit = false;

    // camera
    bool resetFramebuffer = false;
    vec3 camPos = { 0,1.0f,10.0f };
    vec3 moveDir = { 0,0,0 };

    wnd.SetKeyPressFunction([&exit, &moveDir, &resetFramebuffer](int key, int scancode, int action, int mods)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            exit = true;
            break;
        case GLFW_KEY_W:
            moveDir.z -= 1.0f;
            resetFramebuffer |= true;
            break;
        case GLFW_KEY_S:
            moveDir.z += 1.0f;
            resetFramebuffer |= true;
            break;
        case GLFW_KEY_A:
            moveDir.x -= 1.0f;
            resetFramebuffer |= true;
            break;
        case GLFW_KEY_D:
            moveDir.x += 1.0f;
            resetFramebuffer |= true;
            break;
        case GLFW_KEY_SPACE:
            moveDir.y += 1.0f;
            resetFramebuffer |= true;
            break;
        case GLFW_KEY_LEFT_CONTROL:
            moveDir.y -= 1.0f;
            resetFramebuffer |= true;
            break;
        default:
            break;
        }
    });

    float pitch = 0;
    float yaw = 0;
    float oldx = 0;
    float oldy = 0;

    wnd.SetMouseMoveFunction([&pitch, &yaw, &oldx, &oldy, &resetFramebuffer](double x, double y)
    {
        x *= -0.1;
        y *= -0.1;
        yaw = x - oldx;
        pitch = y - oldy;
        resetFramebuffer |= true;
        oldx = x;
        oldy = y;
    });

    float rotx = 0;
    float roty = 0;

    // number of accumulated frames
    int frameIndex = 0;

    std::vector<Color> framebufferCopy;
    framebufferCopy.resize(static_cast<size_t>(cmdArgs.imageWidth)* static_cast<size_t>(cmdArgs.imageHeight));;


    // Num of Rays per frame
    float numberMegaRays = framebuffer.size() * (cmdArgs.raysPerPixel * 0.000001f);
    float megaRaysPerSec = 0.0f;


    // End of setup timer
    float setupTimer = (std::chrono::duration_cast<std::chrono::milliseconds>(wallClock.now() - setupBegin)).count() * 0.001;
    // Output setup time of objects
    std::cout << "Setup Time: " << setupTimer << " seconds" << std::endl;



    // Create time points for render loop
    std::chrono::high_resolution_clock::time_point renderBegin;
    std::chrono::high_resolution_clock::time_point renderEnd;
    double renderTimer{ 0.0 };

    // rendering loop
    while (wnd.IsOpen() && !exit)
    {
        renderBegin = wallClock.now();
        resetFramebuffer = false;
        
        //player movement
        moveDir = {0,0,0};
        pitch = 0;
        yaw = 0;
        // poll input
        wnd.Update();

        rotx -= pitch;
        roty -= yaw;
        moveDir = normalize(moveDir);

        // maybe dont recreate these every loop??
        mat4 xMat = (rotationx(rotx));
        mat4 yMat = (rotationy(roty));
        mat4 cameraTransform = multiply(yMat, xMat);

        camPos = camPos + transform(moveDir * 0.2f, cameraTransform);
        cameraTransform.m30 = camPos.x;
        cameraTransform.m31 = camPos.y;
        cameraTransform.m32 = camPos.z;

        rt.SetViewMatrix(cameraTransform);
        

        if (resetFramebuffer)
        {
            rt.Clear();
            frameIndex = 0;
        }


        // main raytracing starts here 
        rt.Raytrace();
        frameIndex++;

        // Get the average distribution of all samples
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

        glClearColor(0, 0, 0, 1.0);
        glClear( GL_COLOR_BUFFER_BIT );

        wnd.Blit((float*)&framebufferCopy[0], cmdArgs.imageWidth, cmdArgs.imageHeight);
        wnd.SwapBuffers();


        // End of rendering


        // Timers & Debug info
        renderEnd = wallClock.now();
        renderTimer = (std::chrono::duration_cast<std::chrono::milliseconds>(renderEnd - renderBegin)).count() * 0.001;

        // MraysPerSec
        megaRaysPerSec = numberMegaRays *  (1 / renderTimer);

        std::cout << "Render Time: " << renderTimer << " seconds" << std::endl;
        std::cout << "Mrays/sec: " << megaRaysPerSec << "\n" << std::endl;

    }


    // Cleanup
    if (wnd.IsOpen())
        wnd.Close();



    return 0;
} 