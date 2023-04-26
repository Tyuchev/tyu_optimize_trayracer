#include "raytracer.h"
#include <random>

#include <chrono>
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
    // I thought leet code was banned xD
    static int leet = 1337;
    std::mt19937 generator (leet++);
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    for (int x = 0; x < this->width; ++x)
    {
        for (int y = 0; y < this->height; ++y)
        {
            Color color;
            for (int i = 0; i < this->rpp; ++i)
            {
                float u = ((float(x + dis(generator)) * (1.0f / this->width)) * 2.0f) - 1.0f;
                float v = ((float(y + dis(generator)) * (1.0f / this->height)) * 2.0f) - 1.0f;

                vec3 direction = vec3(u, v, -1.0f);
                direction = transform(direction, this->frustum);

                //Not sure about get pos
                Ray ray{ get_position(this->view), direction};
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
    float distance = FLT_MAX;

    if (Raycast(ray, hitPoint, hitNormal, hitObject, distance))
    {
        Ray scatteredRay{ hitObject->ScatterRay(ray, hitPoint, hitNormal) };
        if (n < this->bounces)
        {
            Color colorHandle;
            hitObject->GetColor(colorHandle);
            return colorHandle * this->TracePath(scatteredRay, n + 1);
        }

        if (n == this->bounces)
        {
            return {0,0,0};
        }
    }

    return this->Skybox(ray.direction * ray.magnitude);
}

//------------------------------------------------------------------------------
/**
*/
bool
Raytracer::Raycast(Ray& ray, vec3& hitPoint, vec3& hitNormal, const Sphere*& hitObject, float& distance)
{
    bool isHit = false;
    HitResult closestHit;
    int numHits = 0;
    HitResult hitResult;

    
    // Only thing maybe needs doing is reducing num of spheres we check against? are some behind us maybe?
    // I think behind culling is already implemented

    for (auto const& object : worldObjects)
    {
        // does raycast hit an object?
        HitResult rayTest = object.Intersect(ray, closestHit.t);

        // If yes:
        if (rayTest.sphere)
        {
            hitResult = rayTest;
            if (hitResult.t < closestHit.t)
            {
                closestHit = hitResult;
                closestHit.sphere = &object;
                isHit = true;

                //numHits++;

            }

        }

        hitPoint = closestHit.p;
        hitNormal = closestHit.normal;
        hitObject = closestHit.sphere;
        distance = closestHit.t;
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
void
Raytracer::UpdateMatrices()
{
    mat4 inverseView = inverse(this->view); 
    mat4 basis = transpose(inverseView);
    this->frustum = basis;
}

//------------------------------------------------------------------------------
/**
*/
Color
Raytracer::Skybox(vec3 direction)
{
    float t = 0.5*(direction.y + 1.0);
    vec3 vec = vec3(1.0 - t, 1.0 - t, 1.0 - t) + vec3(0.5 * t, 0.7 * t, t);
    return {(float)vec.x, (float)vec.y, (float)vec.z};
}
