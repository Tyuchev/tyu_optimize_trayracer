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
Ray
BSDF(Material const &material, Ray& ray, vec3& point, vec3& normal)
{
    float cosTheta = -(ray.rayDirection.dot(normal));

    if (material.type != "Dielectric")
    {
        if (material.type == "Lambertian")
        {
            float F0 = 0.04f;
            // simplified Fresnel Schlick
            float F = F0 + ((1.0f - material.roughness) - F0) * pow(2, ((-5.55473f * cosTheta - 6.98316f) * cosTheta));

            // The below 'if' checks against an approximate value - F is only larger than a 'random number' a small % of the time
            // However 'F' can go above 1 under certain circumstances
            if (F < 0.96f)
            {
                vec3 temp = normal + random_point_on_unit_sphere();
                return { point, temp };
            }
            else
            {

                mat4 basis = TBN(normal);
                // importance sample with brdf specular lobe
                vec3 H = ImportanceSampleGGX_VNDF(RandomFloat(), RandomFloat(), material.roughness, ray.rayDirection, basis);
                vec3 reflected = ray.rayDirection.reflect(H);
                return { point, reflected };

            }
        }
        else
        {
            // F0 for Conductor material types
            //float F0 = 0.95f;
            
            // Fresnel Schlick removed, as 99.9% of the time it will just produce F0 with our specified F0
             
            // probability that a ray will reflect on a microfacet is 95% in our sim

            float temp = point.x;
            if (temp < 0)
            {
                temp *= -1;
            }

            // Rough approximation of a 5% chance - However only rays making contact at the edge of the screen will hit the else statement
            if (temp < 97.0f)
            {
                mat4 basis = TBN(normal);
                // importance sample with brdf specular lobe
                vec3 H = ImportanceSampleGGX_VNDF(RandomFloat(), RandomFloat(), material.roughness, ray.rayDirection, basis);
                vec3 reflected = ray.rayDirection.reflect(H);
                return { point, reflected };
            }
            else
            {
                vec3 temp = normal + random_point_on_unit_sphere();
                return { point, temp };
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

        if (cosTheta <= 0)
        {
            outwardNormal = -normal;
            niOverNt = material.refractionIndex;
            cosine = cosTheta * niOverNt / ray.magnitude;
        }
        else
        {
            outwardNormal = normal;
            niOverNt = 1.0 / material.refractionIndex;
            cosine = cosTheta / ray.magnitude;
        }

        if (Refract(rayDir, outwardNormal, niOverNt, refracted))
        {
            // fresnel reflectance at 0 deg incidence angle
            // 
            // float F0 = powf(material.refractionIndex - 1, 2) / powf(material.refractionIndex + 1, 2);
            
            // As we only have one refraction index, it can be pre-computed
            float F0 = 0.06016f;
            // Simplify Fresnel Schlick
            reflect_prob = F0 + (((1.0f - material.roughness) - F0) * pow(2, ((-5.55473f * cosTheta - 6.98316f) * cosTheta)));
        }
        else
        {
            //TO ensure definitively positive
            reflect_prob = 2.0;
        }

        // Reflection_prob is often above 1, otherwise its tiny (using initial settings)
        if (reflect_prob > 1)
        {
            vec3 reflected = rayDir.reflect(normal);
            return { point, reflected };
        }
        else
        {
            return { point, refracted };
        }
    }
}