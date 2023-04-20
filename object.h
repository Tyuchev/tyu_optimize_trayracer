#pragma once
#include "ray.h"
#include "color.h"
#include <float.h>
#include <string>
#include <memory>

class Object;

//------------------------------------------------------------------------------
/**
*/
struct HitResult
{
    // hit point
    vec3 p;
    // normal
    vec3 normal;
    // hit object, or nullptr
    const Object* object = nullptr;
    // intersection distance
    float t = FLT_MAX;
};

template<class TYPE>
class Optional
{
public:
    Optional() {}
    Optional(HitResult hit) : hasValue(true)
    {
        this->value = std::make_shared<HitResult>(hit);
    }

    bool HasValue()
    {
        // check if this object has a value.
        if (hasValue)
        {
            return true;
        }
        else
        {
            return false;
        }

    }
    HitResult Get()
    {
        assert(this->HasValue());
        return *value;
    }

private:
    bool hasValue = false;
    std::shared_ptr<HitResult> value = nullptr;
};

//------------------------------------------------------------------------------
/**
*/
class Object
{
public:
    Object() 
    {
        // blank
    }

    virtual ~Object()
    {

    }

    virtual Optional<HitResult> Intersect(Ray ray, float maxDist) const { return {}; };
    virtual Color GetColor() const = 0;
    virtual Ray ScatterRay(Ray ray, vec3 point, vec3 normal) const { return Ray({ 0,0,0 }, {1,1,1}); };


};