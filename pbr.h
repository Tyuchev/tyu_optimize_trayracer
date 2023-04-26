#pragma once
#include "vec3.h"
#include "mat4.h"
#include <math.h>

//------------------------------------------------------------------------------
/**
*/
inline float
FresnelSchlick(float cosTheta, float F0, float roughness)
{
    return F0 + (fmax(1.0f - roughness, F0) - F0) * pow(2, ((-5.55473f*cosTheta - 6.98316f) * cosTheta));
}

//------------------------------------------------------------------------------
/**
*/
inline vec3
ImportanceSampleGGX_VNDF(float u1, float u2, float roughness, vec3 const& V, mat4 const& basis)
{
    float alpha = roughness * roughness;

    vec3 Ve = -vec3{ V.dot(get_row0(basis)), V.dot(get_row2(basis)), V.dot(get_row1(basis)) };

    vec3 Vh = vec3(alpha * Ve.x, alpha * Ve.y, Ve.z);
    Vh = Vh.normalize();

    // Why? we just normalised length
    // float lensq = Vh.x * Vh.x + Vh.y * Vh.y; // Also what happened to z value?

    vec3 T1{ -Vh.y, Vh.x, 0.0f };
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
    vec3 Ne{ alpha * Nh.x, fmaxf(0.0f, Nh.z), alpha * Nh.y };

    // World space H
    vec3 temp{ transform(Ne, basis) };
    return temp;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
Refract(vec3 v, vec3 n, float niOverNt, vec3& refracted)
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