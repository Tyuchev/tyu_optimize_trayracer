#include "material.h"
#include "pbr.h"
#include <time.h>
#include "mat4.h"
#include "sphere.h"
#include "random.h"

//------------------------------------------------------------------------------
/**
*/
Ray
BSDF(Material const &material, Ray& ray, vec3& point, vec3& normal)
{
    float cosTheta = -(ray.direction.dot(normal));

    if (material.type != "Dielectric")
    {
        float F0 = 0.04f;
        if (material.type == "Conductor")
        {
            F0 = 0.95f;
        }

        // probability that a ray will reflect on a microfacet
        float F = FresnelSchlick(cosTheta, F0, material.roughness);

        float r = RandomFloat();

        if (r < F)
        {
            mat4 basis = TBN(normal);
            // importance sample with brdf specular lobe
            vec3 H = ImportanceSampleGGX_VNDF(RandomFloat(), RandomFloat(), material.roughness, ray.direction, basis);
            vec3 reflected = ray.direction.reflect(H);
            return { point, reflected };
        }
        else
        {
            vec3 temp = normal + random_point_on_unit_sphere();
            return { point, temp };
        }
    }
    else
    {
        vec3 outwardNormal{1,1,1};
        float niOverNt;
        vec3 refracted{1,1,1};
        float reflect_prob;
        float cosine;
        vec3 rayDir = ray.direction;

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
            float F0 = powf(material.refractionIndex - 1, 2) / powf(material.refractionIndex + 1, 2);
            reflect_prob = FresnelSchlick(cosine, F0, material.roughness);
        }
        else
        {
            reflect_prob = 1.0;
        }
        if (RandomFloat() < reflect_prob)
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