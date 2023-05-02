#include "raytracer.h"
#include <random>

#include <chrono>
#include <iostream>
#include <memory>
#include "sphere.h"




//------------------------------------------------------------------------------
/**
*/
Raytracer::Raytracer(unsigned w, unsigned h, std::vector<Color>& frameBuffer, unsigned rpp, unsigned bounces, std::vector<Sphere> const &objects) :
    frameBuffer(frameBuffer),
    rpp(rpp),
    bounces(bounces),
    width(w),
    height(h),
    worldObjects(objects)

{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
Raytracer::Raytrace()
{
     //40 million bytes to hold my random num gen atlas
     static int leet = 1337;
     std::mt19937 generator (leet++);
     std::uniform_real_distribution<float> dis(0.0f, 1.0f);

     float widthInv = 1.0f / this->width;
     float heightInv = 1.0f / this->height;

    for (int x = 0; x < this->width; ++x)
    {
        for (int y = 0; y < this->height; ++y)
        {
            Color color;
            for (int i = 0; i < this->rpp; ++i)
            {
                float u = ((float(x + dis(generator)) * widthInv) * 2.0f) - 1.0f;
                float v = ((float(y + dis(generator)) * heightInv) * 2.0f) - 1.0f;

                Ray ray{ vec3{0, 10, 0}, vec3(u, v, 1.0f) };
                color += this->TracePath(ray, 0);
            }

            // divide by number of samples per pixel, to get the average of the distribution
            color.r /= this->rpp;
            color.g /= this->rpp;
            color.b /= this->rpp;

            this->frameBuffer[y * this->width + x] += color;
        }
    }

}

//------------------------------------------------------------------------------
/**
 * @parameter n - the current bounce level
*/
Color Raytracer::TracePath(Ray& ray, unsigned n)
{
    vec3 hitPoint;
    vec3 hitNormal;
    const Sphere* hitObject = nullptr;

    if (Raycast(ray, hitPoint, hitNormal, hitObject))
    {
        //BSDF overwrites ray's memory
        BSDF(hitObject->material, ray, hitNormal);
        if (n < this->bounces)
        {
            Color colorHandle;
            hitObject->GetColor(colorHandle);
            return colorHandle * this->TracePath(ray, n + 1);
        }

        if (n == this->bounces)
        {
            return {0,0,0};
        }
    }

    return this->Skybox(ray.rayDirection * ray.magnitude);
}

//------------------------------------------------------------------------------
/**
*/
bool
Raytracer::Raycast(Ray& ray, vec3& hitPoint, vec3& hitNormal, const Sphere*& hitObject)
{
    bool isHit = false;
    HitResult closestHit;
    int numHits = 0;
    float maxDistance = 10000;

    // Set max range for initial hit testing;
    closestHit.t = maxDistance;

    // Only thing maybe needs doing is reducing num of spheres we check against? are some behind us maybe?
    // I think behind culling is already implemented

    for (auto const& object : worldObjects)
    {
        // does raycast hit an object?
        if (object.Intersect(closestHit, ray))
        {
            isHit = true;

            hitPoint = closestHit.p;
            hitNormal = closestHit.normal;
            hitObject = closestHit.sphere;
        }
    }

    return isHit;
}


//------------------------------------------------------------------------------
/**
*/
void
Raytracer::Clear()
{
    for (auto& color : this->frameBuffer)
    {
        color.r = 0.0f;
        color.g = 0.0f;
        color.b = 0.0f;
    }
}


//------------------------------------------------------------------------------
/**
*/
Color
Raytracer::Skybox(vec3& direction) const
{
    float t = 0.5*(direction.y + 1.0);
    vec3 vec = vec3(1.0 - t, 1.0 - t, 1.0 - t) + vec3(0.5 * t, 0.7 * t, t);
    return {(float)vec.x, (float)vec.y, (float)vec.z};
}
