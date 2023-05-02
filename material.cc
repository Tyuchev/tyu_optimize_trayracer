#include "material.h"
#include "pbr.h"
#include <time.h>
#include "mat4.h"
#include "sphere.h"
#include "random.h"
#include "mat4.h"

#include <iostream>



// returns a random point on the surface of a unit sphere
vec3 random_point_on_unit_sphere()
{
    float x = RandomFloatNTP();
    float y = RandomFloatNTP();
    float z = RandomFloatNTP();
    vec3 v(x, y, z);
    return v;
};

//------------------------------------------------------------------------------
/**
*/

bool
BSDF(Material const &material, Ray& ray, vec3& normal)
{
    //these should be normalized... output will be between 1 & -1
    float cosTheta = -(ray.rayDirection.dot(normal));

    // If dotpr is 0 or less, then vectors go in opposites
    // cosTheta is *= -1, so, we if RAY and NORMAL are OPPOSITES, cosTheta INCREASES
    // 
    // IF RAY & NORMAL go in SAME direction, THETA will be NEGATIVE


    if (material.type != "Dielectric")
    {
        if (material.type == "Lambertian")
        {

            float F0 = 0.04f;
            bool isReflected = false;

            // Simplify Fresnel Schlick
            float F = F0 + ((1.0f - material.roughness) - F0) * pow(2, ((-5.55473f * cosTheta - 6.98316f) * cosTheta));

            float r = RandomFloat();
            if (F < r)
            {
                isReflected = false;
            }


            if (isReflected)
            {
                mat4 basis = basis.TBN(normal);
                // importance sample with brdf specular lobe
                vec3 H = ImportanceSampleGGX_VNDF(RandomFloat(), RandomFloat(), material.roughness, ray.rayDirection, basis);
                ray.rayDirection = ray.rayDirection.reflect(H);
                return true;
            }
            else
            {
                ray.rayDirection = normal + random_point_on_unit_sphere();
                return true;
            }
        }
        else
        {
            // F0 for Conductor material types
            //float F0 = 0.95f;
            // 
            // Fresnel Schlick removed, as 99.9% of the time it will just produce F0 with our specified F0
             
            // Probability that a ray will reflect on a microfacet is 95% in our sim

            // Approximation of a 95% chance of reflection
            if (material.roughness < 0.95f)
            {
                mat4 basis = basis.TBN(normal);
                // importance sample with brdf specular lobe
                vec3 H = ImportanceSampleGGX_VNDF(RandomFloat(), RandomFloat(), material.roughness, ray.rayDirection, basis);
                ray.rayDirection = ray.rayDirection.reflect(H);
                return true;
            }
            else
            {
                ray.rayDirection = normal + random_point_on_unit_sphere();
                return true;
            }
        }

    }
    else
    {
        vec3 outwardNormal;
        float niOverNt;
        vec3 refracted;
        float reflect_prob;
        float cosine;
        vec3 rayDir = ray.rayDirection;
        bool isReflected = true;

        if (cosTheta <= 0)
        {
            outwardNormal = -normal;
            niOverNt = material.refractionIndex;
            cosine = cosTheta * niOverNt * (1 /ray.magnitude);
        }
        else
        {
            outwardNormal = normal;
            niOverNt = 1.0 / material.refractionIndex;
            cosine = cosTheta * (1 / ray.magnitude);
        }

        if (Refract(rayDir, outwardNormal, niOverNt, refracted))
        {
            // fresnel reflectance at 0 deg incidence angle
            // 
            // float F0 = powf(material.refractionIndex - 1, 2) / powf(material.refractionIndex + 1, 2);
            // As we only have one refraction index, F0 can be pre-computed
            float F0 = 0.06016f;
            
            // Simplify Fresnel Schlick
            reflect_prob = F0 + (((1.0f - material.roughness) - F0) * pow(2, ((-5.55473f * cosine - 6.98316f) * cosine)));
            // roughness is already a random float, no need to test against another new randomFloat
            if (reflect_prob < material.roughness)
            {

                isReflected = false;

            }
        }

        if (isReflected)
        {
            ray.rayDirection = rayDir.reflect(normal);
            return true;
        }
        else
        {
            ray.rayDirection = refracted;
            return true;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/

//Simplified and used elswhere

//inline float
//FresnelSchlick(float cosTheta, float F0, float roughness)
//{
//    return F0 + (fmax(1.0f - roughness, F0) - F0) * pow(2, ((-5.55473f*cosTheta - 6.98316f) * cosTheta));
//}


//------------------------------------------------------------------------------
/**
*/
vec3
ImportanceSampleGGX_VNDF(float u1, float u2, float roughness, vec3 const& V, mat4 const& basis)
{
    float alpha = roughness * roughness;

    vec3 Ve = -vec3{ V.dot(basis.get_row0()), V.dot(basis.get_row2()), V.dot(basis.get_row1()) };

    vec3 Vh{ alpha * Ve.x, alpha * Ve.y, Ve.z };
    Vh = Vh.normalize();

    vec3 T1{ -Vh.y, Vh.x, 0.0f };
    T1 = T1.normalize();

    vec3 T2 = Vh.cross(T1);

    float r = sqrt(u1);
    float phi = 2.0f * MPI * u2;
    float t1 = r * cos(phi);
    float t2 = r * sin(phi);
    float s = 0.5 * (1.0 + Vh.z);
    float t1sq = (t1 * t1);
    t2 = (1.0 - s) * sqrtf(1.0 - t1sq) + s * t2;


    vec3 Nh = T1 * t1 + T2 * t2 + Vh * sqrtf(fmaxf(0.0f, 1.0f - t1sq - (t2 * t2)));

    // Tangent space H
    // This differs from other implementations - why do we swap the y & z co-ords??
    vec3 Ne{ alpha * Nh.x, fmaxf(0.0f, Nh.z), alpha * Nh.y };

    return Ne;
}

//------------------------------------------------------------------------------
/**
*/
bool
Refract(vec3& v, vec3& n, float niOverNt, vec3& refracted)
{
    float dt = v.dot(n);
    float discriminant = 1.0f - niOverNt * niOverNt * (1.0f - dt * dt);
    if (discriminant > 0)
    {
        refracted = ((v - n * dt) * niOverNt) - (n * sqrt(discriminant));
        return true;
    }

    return false;
}