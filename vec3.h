#pragma once
#include <cmath>
#include <initializer_list>
#include <assert.h>

#define MPI 3.14159265358979323846

struct vec2
{
    float x, y;
};


class vec3
{
public:
    vec3()
        :x(1), y(1), z(1)
    {

    }

    vec3(vec2 vec, float z) : x(vec.x), y(vec.y), z(z)
    {

    }

    vec3(double x, double y, double z) : x(x), y(y), z(z)
    {

    }

    ~vec3()
    {
    }

    vec3(vec3 const& rhs)
    {
        this->x = rhs.x;
        this->y = rhs.y;
        this->z = rhs.z;
    }

    vec3 operator+(vec3 const& rhs) { return {x + rhs.x, y + rhs.y, z + rhs.z};}
    vec3 operator-(vec3 const& rhs) { return {x - rhs.x, y - rhs.y, z - rhs.z};}
    vec3 operator-() { return {-x, -y, -z};}
    vec3 operator*(float const c) { return {x * c, y * c, z * c};}

    double x, y, z;

    // Get length of 3D vector
    double len() const
    {
        double a = this->x * this->x;
        a = a + this->y * this->y;
        a = a + this->z * this->z;
        double l = sqrt(a);
        return l;
    }


    vec3 normalize() const
    {
        double l = this->len();

        vec3 ret{ this->x * 1/ l, this->y * 1/l, this->z * 1/l };
        return ret;
    }

    // sets storelength to the length of the original vector
    vec3 normalize(double& storeLength) const
    {
        storeLength = this->len();

        vec3 ret{ this->x / storeLength, this->y / storeLength, this->z / storeLength };
        return ret;
    }

    // piecewise multiplication between two vectors
    vec3 mul(vec3& rhs) const
    {
        return { this->x * rhs.x, this->y * rhs.y, this->z * rhs.z };
    }

    // piecewise add between two vectors
    vec3 add(vec3& rhs) const
    {
        return { this->x + rhs.x, this->y + rhs.y, this->z + rhs.z };
    }

    float dot(const vec3& rhs) const
    {
        return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z;
    }

    vec3 reflect(vec3& rhs) const
    {
        vec3 lhs = *this;
        return lhs - rhs * (2 * dot(rhs));
    }

    vec3 cross(vec3& rhs) const
    {
        return { this->y * rhs.z - this->z * rhs.y,
                 this->z * rhs.x - this->x * rhs.z,
                 this->x * rhs.y - this->y * rhs.x, };
    }

};

