#pragma once
#include <vector>
#include "vec3.h"
#include "mat4.h"
#include "color.h"
#include "ray.h"
#include <float.h>

#include <chrono>
#include <memory>
#include <algorithm>
#include <thread>
#include <execution>
#include <vector>

#include "sphere.h"
#include "random.h"



//------------------------------------------------------------------------------
/**
*/
class Raytracer
{
public:
    Raytracer(unsigned w, unsigned h, std::vector<Color>& frameBuffer, unsigned rpp, unsigned bounces, std::vector<Sphere> const &objects);
    ~Raytracer() { }

    // start raytracing!
    void Raytrace(std::vector<float>& randoms);

    // single raycast, find object
    bool Raycast(Ray& ray, vec3& hitPoint, vec3& hitNormal, const Sphere*& hitObject);

    // clear screen
    void Clear();

    // trace a path and return intersection color
    // n is bounce depth
    Color TracePath(Ray& ray, unsigned n, float random);

    // get the color of the skybox in a direction
    Color Skybox(float rayY) const;

    std::vector<Color>& frameBuffer;
    
    // rays per pixel
    unsigned rpp;
    // max number of bounces before termination
    unsigned bounces = 5;

    // width of framebuffer
    const unsigned width;
    // height of framebuffer
    const unsigned height;
    



private:
    std::vector<Sphere> const &worldObjects;
    std::vector<int> frameBufferWidthIt;
    std::vector<int> frameBufferHeightIt;
    std::vector<int> frameBufferRppIt;

    // Scatter ray against material
    bool BSDF(Material const& material, Ray& ray, vec3& normal, float random);

    // returns a random point on the surface of a unit sphere
    vec3 random_point_on_unit_sphere(float random);
    
    // To refer to precalculated random values - deprecated
    // std::shared_ptr<std::vector<vec2>> randomNums;

};


