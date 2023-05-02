#pragma once
#include <stdlib.h>
#include <time.h>
#include "mat4.h"
#include "pbr.h"
#include "random.h"
#include "ray.h"
#include "material.h"


class Sphere;

struct HitResult
{
    // hit point
    vec3 p;
    // normal
    vec3 normal;
    // hit object, or nullptr
    const Sphere* sphere = nullptr;
    // intersection distance
    float t = FLT_MAX;
};


// a spherical object
class Sphere
{

public:
    float radius;
    vec3 center;
    Material const material;

    Sphere(float radius, vec3 center, Material const material) :
        radius(radius),
        center(center),
        material(material)
    {

    }

    ~Sphere()
    {
    
    }


    void GetColor(Color& inputColorBuffer) const 
    {
        inputColorBuffer = material.color;
    }

    bool Intersect(HitResult& storage, Ray& ray) const
    {
        vec3 oc = ray.start - this->center;
        float b = oc.x * ray.rayDirection.x + oc.y * ray.rayDirection.y + oc.z * ray.rayDirection.z;
        // early out if sphere is "behind" ray
        if (b > 0)
        {
            return false;
        }
        // Attempt to early out for spheres too far away
        float maxDistSqr = storage.t * storage.t;
        float ocLength = oc.x * oc.x + oc.y * oc.y + oc.z * oc.z;
        // This should be adjusted by sphere radius to check for spheres which collide
        if ((ocLength < maxDistSqr))
        {
            float a = ray.rayDirection.x * ray.rayDirection.x +
                ray.rayDirection.y * ray.rayDirection.y +
                ray.rayDirection.z * ray.rayDirection.z;

            float discriminant = b * b - a * (ocLength - this->radius * this->radius);
            if (discriminant <= 0)
            {
                return false;
            }
            else
            {
                float div = 1.0f / a;
                // removed min distance checks
                // constexpr float minDist = 0.001f;
                float sqrtDisc = sqrt(discriminant);
                float temp = (-b - sqrtDisc) * div;
                if (temp < storage.t)
                {
                    //vec3 p = ray.PointAt(temp);
                    storage.p = ray.start + ray.rayDirection * temp;
                    storage.normal = (storage.p - this->center) * (1.0f / this->radius);
                    storage.t = temp;
                    storage.sphere = this;
                    return true;
                }


                //// i think this checks if we are inside a sphere - not needed at small numbers
                //float temp2 = (-b + sqrtDisc) * div;
                //if (temp2 < storage.t)
                //{
                //    //vec3 p = ray.PointAt(temp2);
                //    storage.p = ray.start + ray.rayDirection * temp2;
                //    storage.normal = (storage.p - this->center) * (1.0f / this->radius);
                //    storage.t = temp2;
                //    storage.sphere = this;
                //    return true;
                //}
            }
        }

        return false;
    }

};



