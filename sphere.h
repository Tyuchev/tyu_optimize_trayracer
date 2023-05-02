#pragma once
#include <stdlib.h>
#include <time.h>
#include "mat4.h"
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
        vec3 rayVector = ray.rayDirection;
        vec3 rayOrigin = ray.start;
        vec3 oc = rayOrigin - this->center;
        float b = oc.x * rayVector.x + oc.y * rayVector.y + oc.z * rayVector.z;
        // early out if sphere is "behind" ray
        if (b < 0)
        {
            float prevMaxDist = storage.t;
            // Attempt to early out for spheres too far away
            float maxDistSqr = prevMaxDist * prevMaxDist;
            float ocLength = oc.x * oc.x + oc.y * oc.y + oc.z * oc.z;
            // This should be adjusted by sphere radius to check for spheres which collide
            if ((ocLength < maxDistSqr))
            {
                float a = rayVector.x * rayVector.x +
                    rayVector.y * rayVector.y +
                    rayVector.z * rayVector.z;

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
                    if (temp < prevMaxDist)
                    {
                        vec3 p = rayOrigin + rayVector * temp;
                        storage.p = p;
                        storage.normal = (p - this->center) * (1.0f / this->radius);
                        storage.t = temp;
                        storage.sphere = this;
                        return true;
                    }


                    //// i think this checks if we are inside a sphere - not needed at small numbers
                    //float temp2 = (-b + sqrtDisc) * div;
                    //if (temp2 < prevMaxDist)
                    //{
                    //    //vec3 p = ray.PointAt(temp2);
                    //    storage.p = ray.start + rayVector * temp2;
                    //    storage.normal = (storage.p - this->center) * (1.0f / this->radius);
                    //    storage.t = temp2;
                    //    storage.sphere = this;
                    //    return true;
                    //}
                }
            }
       
        }


        return false;
    }

};



