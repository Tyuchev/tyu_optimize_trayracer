#pragma once
#include "vec3.h"

//------------------------------------------------------------------------------
/**
*/
class Ray
{
public:
    Ray(vec3 startpoint, vec3 dir) :
        start(startpoint),
        rayDirection(dir),
        magnitude(0)
    {
        rayDirection = rayDirection.normalize(magnitude);
    }

    ~Ray()
    {

    }

    vec3 PointAt(float t)
    {
        return {start + rayDirection * t};
    }

    // beginning of ray
    vec3 start;
    // magnitude and direction of ray
    vec3 rayDirection;

    double magnitude;
};