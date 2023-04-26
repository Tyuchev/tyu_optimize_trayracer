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


// returns a random point on the surface of a unit sphere
inline vec3 random_point_on_unit_sphere()
{
    float x = RandomFloatNTP();
    float y = RandomFloatNTP();
    float z = RandomFloatNTP();
    vec3 v(x, y, z);
    return v ;
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

    HitResult Intersect(Ray ray, float maxDist) const
    {
        HitResult hit;
        vec3 oc = ray.start - this->center;
        vec3 dir = ray.direction * ray.magnitude;
        float b = oc.dot(dir); 

        // early out if sphere is "behind" ray
        if (b > 0)
        {
            return hit;
        }

        float a = dir.dot(dir);
        float c = oc.dot(oc) - this->radius * this->radius;
        float discriminant = b * b - a * c;

        if (discriminant > 0)
        {
            constexpr float minDist = 0.001f;
            float div = 1.0f / a;
            float sqrtDisc = sqrt(discriminant);
            float temp = (-b - sqrtDisc) * div;
            float temp2 = (-b + sqrtDisc) * div;

            if (temp < maxDist && temp > minDist)
            {
                vec3 p = ray.PointAt(temp);
                hit.p = p;
                hit.normal = (p - this->center) * (1.0f / this->radius);
                hit.t = temp;
                hit.sphere = this;
                return hit;
            }
            if (temp2 < maxDist && temp2 > minDist)
            {
                vec3 p = ray.PointAt(temp2);
                hit.p = p;
                hit.normal = (p - this->center) * (1.0f / this->radius);
                hit.t = temp2;
                hit.sphere = this;
                return hit;
            }
        }
        return hit;
    }

    Ray ScatterRay(Ray& ray, vec3& point, vec3& normal) const  
    {
        return BSDF(material, ray, point, normal);
    }


};



