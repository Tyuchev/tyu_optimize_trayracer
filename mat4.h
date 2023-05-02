#pragma once
#include "vec3.h"

//------------------------------------------------------------------------------
/**
    @struct mat4

    4x4 matrix
*/
class mat4
{
public:

    float m00, m01, m02, m03;
    float m10, m11, m12, m13;
    float m20, m21, m22, m23;
    float m30, m31, m32, m33;


    //------------------------------------------------------------------------------
/**
*/
    vec3 get_row0() const
    {
        return { m00, m01, m02 };
    }

    //------------------------------------------------------------------------------
    /**
    */
    vec3 get_row1() const
    {
        return { m10, m11, m12 };
    }

    //------------------------------------------------------------------------------
    /**
    */
    vec3
        get_row2() const
    {
        return { m20, m21, m22 };
    }

    //------------------------------------------------------------------------------
    /**
    */
    vec3
        get_position() const
    {
        return { m30, m31, m32 };
    }


    //------------------------------------------------------------------------------
    /**
        Create a cartesian space transform from a single normal
    */
    mat4 TBN(vec3 normal)
    {
        mat4 ret;
        ret.m10 = normal.x;
        ret.m11 = normal.y;
        ret.m12 = normal.z;
        ret.m13 = 0.0f;

        if (normal.z < -0.999805696f)
        {
            ret.m00 = 0.0f;
            ret.m01 = -1.0f;
            ret.m02 = 0.0f;
            ret.m03 = 0.0f;

            ret.m20 = -1.0f;
            ret.m21 = 0.0f;
            ret.m22 = 0.0f;
            ret.m23 = 0.0f;
        }
        else
        {
            float a = 1.0f / (1.0f + normal.z);
            float b = -normal.x * normal.y * a;

            ret.m00 = 1.0f - normal.x * normal.x * a;
            ret.m01 = b;
            ret.m02 = -normal.x;
            ret.m03 = 0.0f;

            ret.m20 = b;
            ret.m21 = 1.0f - normal.y * normal.y * a;
            ret.m22 = normal.y;
            ret.m23 = 0.0f;
        }

        ret.m30 = 0.0f;
        ret.m31 = 0.0f;
        ret.m32 = 0.0f;
        ret.m33 = 0.0f;
        return ret;
    }

    //------------------------------------------------------------------------------
    /**
        Calculate determinant
    */
    float det()
    {
        return
            (m00 * m11 - m01 * m10) * (m22 * m33 - m23 * m32)
            - (m00 * m12 - m02 * m10) * (m21 * m33 - m23 * m31)
            + (m00 * m13 - m03 * m10) * (m21 * m32 - m22 * m31)
            + (m01 * m12 - m02 * m11) * (m20 * m33 - m23 * m30)
            - (m01 * m13 - m03 * m11) * (m20 * m32 - m22 * m30)
            + (m02 * m13 - m03 * m12) * (m20 * m31 - m21 * m30);
    }

    //------------------------------------------------------------------------------
    /**
        Calculate inverse of matrix
    */
    mat4 inverse()
    {
        float s = det();

        if (s == 0.0)
            return { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 }; // cannot inverse, make it identity matrix

        s = 1.0f / s;

        return { s * (m11 * (m22 * m33 - m23 * m32) + m12 * (m23 * m31 - m21 * m33) + m13 * (m21 * m32 - m22 * m31)),
                 s * (m21 * (m02 * m33 - m03 * m32) + m22 * (m03 * m31 - m01 * m33) + m23 * (m01 * m32 - m02 * m31)),
                 s * (m31 * (m02 * m13 - m03 * m12) + m32 * (m03 * m11 - m01 * m13) + m33 * (m01 * m12 - m02 * m11)),
                 s * (m01 * (m13 * m22 - m12 * m23) + m02 * (m11 * m23 - m13 * m21) + m03 * (m12 * m21 - m11 * m22)),
                 s * (m12 * (m20 * m33 - m23 * m30) + m13 * (m22 * m30 - m20 * m32) + m10 * (m23 * m32 - m22 * m33)),
                 s * (m22 * (m00 * m33 - m03 * m30) + m23 * (m02 * m30 - m00 * m32) + m20 * (m03 * m32 - m02 * m33)),
                 s * (m32 * (m00 * m13 - m03 * m10) + m33 * (m02 * m10 - m00 * m12) + m30 * (m03 * m12 - m02 * m13)),
                 s * (m02 * (m13 * m20 - m10 * m23) + m03 * (m10 * m22 - m12 * m20) + m00 * (m12 * m23 - m13 * m22)),
                 s * (m13 * (m20 * m31 - m21 * m30) + m10 * (m21 * m33 - m23 * m31) + m11 * (m23 * m30 - m20 * m33)),
                 s * (m23 * (m00 * m31 - m01 * m30) + m20 * (m01 * m33 - m03 * m31) + m21 * (m03 * m30 - m00 * m33)),
                 s * (m33 * (m00 * m11 - m01 * m10) + m30 * (m01 * m13 - m03 * m11) + m31 * (m03 * m10 - m00 * m13)),
                 s * (m03 * (m11 * m20 - m10 * m21) + m00 * (m13 * m21 - m11 * m23) + m01 * (m10 * m23 - m13 * m20)),
                 s * (m10 * (m22 * m31 - m21 * m32) + m11 * (m20 * m32 - m22 * m30) + m12 * (m21 * m30 - m20 * m31)),
                 s * (m20 * (m02 * m31 - m01 * m32) + m21 * (m00 * m32 - m02 * m30) + m22 * (m01 * m30 - m00 * m31)),
                 s * (m30 * (m02 * m11 - m01 * m12) + m31 * (m00 * m12 - m02 * m10) + m32 * (m01 * m10 - m00 * m11)),
                 s * (m00 * (m11 * m22 - m12 * m21) + m01 * (m12 * m20 - m10 * m22) + m02 * (m10 * m21 - m11 * m20)) };
    }

    //------------------------------------------------------------------------------
    /**
        Flip it!
    */
    mat4 transpose()
    {
        return {
            m00, m10, m20, m30,
            m01, m11, m21, m31,
            m02, m12, m22, m32,
            m03, m13, m23, m33
        };
    }

    //------------------------------------------------------------------------------
    /**
    */
    mat4 multiply(mat4 a)
    {
        return { m00 * a.m00 + m10 * a.m01 + m20 * a.m02 + m30 * a.m03,
                 m01 * a.m00 + m11 * a.m01 + m21 * a.m02 + m31 * a.m03,
                 m02 * a.m00 + m12 * a.m01 + m22 * a.m02 + m32 * a.m03,
                 m03 * a.m00 + m13 * a.m01 + m23 * a.m02 + m33 * a.m03,

                 m00 * a.m10 + m10 * a.m11 + m20 * a.m12 + m30 * a.m13,
                 m01 * a.m10 + m11 * a.m11 + m21 * a.m12 + m31 * a.m13,
                 m02 * a.m10 + m12 * a.m11 + m22 * a.m12 + m32 * a.m13,
                 m03 * a.m10 + m13 * a.m11 + m23 * a.m12 + m33 * a.m13,

                 m00 * a.m20 + m10 * a.m21 + m20 * a.m22 + m30 * a.m23,
                 m01 * a.m20 + m11 * a.m21 + m21 * a.m22 + m31 * a.m23,
                 m02 * a.m20 + m12 * a.m21 + m22 * a.m22 + m32 * a.m23,
                 m03 * a.m20 + m13 * a.m21 + m23 * a.m22 + m33 * a.m23,

                 m00 * a.m30 + m10 * a.m31 + m20 * a.m32 + m30 * a.m33,
                 m01 * a.m30 + m11 * a.m31 + m21 * a.m32 + m31 * a.m33,
                 m02 * a.m30 + m12 * a.m31 + m22 * a.m32 + m32 * a.m33,
                 m03 * a.m30 + m13 * a.m31 + m23 * a.m32 + m33 * a.m33 };
    }

    //------------------------------------------------------------------------------
    /**
    */
    mat4 rotationx(float angle)
    {
        float result;
        float c;
        float s;

        if (angle == 180.0f) {
            result = MPI;
            s = 0;
        }
        else {
            result = (angle * MPI / 180.0f);
            s = sinf(result);
        }

        c = std::cosf(result);

        return { 1,  0,  0,  0,
                 0,  c, -s,  0,
                 0,  s,  c,  0,
                 0,  0,  0,  1 };
    }

    //------------------------------------------------------------------------------
    /**
    */
    mat4 rotationy(float angle)
    {
        float result;
        float c;
        float s;

        if (angle == 180.0f) {
            result = MPI;
            s = 0;
        }
        else {
            result = (angle * MPI / 180.0f);
            s = sinf(result);
        }

        c = std::cosf(result);

        return { c, 0, s, 0,
                  0, 1, 0, 0,
                 -s, 0, c, 0,
                  0, 0, 0, 1 };
    }

};

