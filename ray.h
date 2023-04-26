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
        direction(dir),
        magnitude(0)
    {
        direction = direction.normalize(magnitude);
    }

    ~Ray()
    {

    }

    vec3 PointAt(float t)
    {
        return {start + direction * t};
    }

    // beginning of ray
    vec3 start;
    // magnitude and direction of ray
    vec3 direction;

    float magnitude;
};