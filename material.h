#pragma once
#include "color.h"
#include "ray.h"
#include "vec3.h"
#include <string>
#include "mat4.h"

//------------------------------------------------------------------------------
/**
*/
struct Material
{
    
    /*
        type can be "Lambertian", "Dielectric" or "Conductor".
        Obviously, "lambertian" materials are dielectric, but we separate them here
        just because figuring out a good IOR for ex. plastics is too much work
    */
    std::string type = "Lambertian";
    Color color = {0.5f,0.5f,0.5f};
    float roughness = 0.75;

    // this is only needed for dielectric materials.
    float refractionIndex = 1.65;
};

//------------------------------------------------------------------------------
/**
    Scatter ray against material
*/
bool BSDF(Material const &material, Ray& ray, vec3& normal);

// returns a random point on the surface of a unit sphere
vec3 random_point_on_unit_sphere();

//------------------------------------------------------------------------------
/**
*/

//Simplified and used elswhere
// 
//inline float
//FresnelSchlick(float cosTheta, float F0, float roughness);

//------------------------------------------------------------------------------
/**
*/
vec3 ImportanceSampleGGX_VNDF(float u1, float u2, float roughness, vec3 const& V, mat4 const& basis);

//------------------------------------------------------------------------------
/**
*/
bool Refract(vec3& v, vec3& n, float niOverNt, vec3& refracted);
