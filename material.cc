#include "material.h"
#include "pbr.h"
#include <time.h>
#include "mat4.h"
#include "sphere.h"
#include "random.h"

#include <iostream>

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
                mat4 basis = TBN(normal);
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
                mat4 basis = TBN(normal);
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


            if (cosTheta > 0.35f)
            {
                isReflected = true;
            }
            else if (material.roughness < 0.2f)
            {
                isReflected = true;
            }
            else
            {
                // Simplify Fresnel Schlick
                reflect_prob = F0 + (((1.0f - material.roughness) - F0) * pow(2, ((-5.55473f * cosine - 6.98316f) * cosine)));
                // roughness is already a random float, no need to test against another new randomFloat
                if (reflect_prob < material.roughness)
                {

                    isReflected = false;

                }
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