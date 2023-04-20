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
                
                Ray* ray = new Ray(get_position(this->view), direction);
                color += this->TracePath(*ray, 0);
                delete ray;
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
Color
Raytracer::TracePath(Ray ray, unsigned n)
{
    vec3 hitPoint;
    vec3 hitNormal;
    const Object* hitObject = nullptr;
    float distance = FLT_MAX;

    if (Raycast(ray, hitPoint, hitNormal, hitObject, distance))
    {
        Ray* scatteredRay = new Ray(hitObject->ScatterRay(ray, hitPoint, hitNormal));
        if (n < this->bounces)
        {
            return hitObject->GetColor() * this->TracePath(*scatteredRay, n + 1);
        }
        delete scatteredRay;

        if (n == this->bounces)
        {
            return {0,0,0};
        }
    }

    return this->Skybox(ray.m);
}

//------------------------------------------------------------------------------
/**
*/
bool
Raytracer::Raycast(Ray ray, vec3& hitPoint, vec3& hitNormal, const Object*& hitObject, float& distance)
{
    bool isHit = false;
    HitResult closestHit;
    int numHits = 0;
    HitResult hitResult;

    // Are we recreating the vectors/storage? in which case this memory wont stick around?? There arent any duplicates in the 'world' vector?
    
    // Only thing maybe needs doing is reducing num of spheres we check against? are some behind us maybe?
    // I think behind culling is already implemented

    for (auto const& object : worldObjects)
    {
        // does raycast hit an object?
        auto optional = object.Intersect(ray, closestHit.t);

        // If yes:
        if (optional.HasValue())
        {
            hitResult = optional.Get();
            if (hitResult.t < closestHit.t)
            {
                closestHit = hitResult;
                closestHit.object = &object;
                isHit = true;

                //numHits++;
            }

        }
    }

    hitPoint = closestHit.p;
    hitNormal = closestHit.normal;
    hitObject = closestHit.object;
    distance = closestHit.t;
    
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
    vec3 vec = vec3(1.0, 1.0, 1.0) * (1.0 - t) + vec3(0.5, 0.7, 1.0) * t;
    return {(float)vec.x, (float)vec.y, (float)vec.z};
}
