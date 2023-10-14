#pragma once
#include <cmath>

#define DECL_ALIGN(x) __declspec(align(x))
#define ALIGN16 DECL_ALIGN(16)
#define ALIGN16_POST DECL_ALIGN(16)

class SDKVec3
{
public:
    SDKVec3(void)
    {
        Invalidate();
    }
    SDKVec3(float X, float Y, float Z)
    {
        x = X;
        y = Y;
        z = Z;
    }
    SDKVec3(const float* clr)
    {
        x = clr[0];
        y = clr[1];
        z = clr[2];
    }

    void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
    {
        x = ix; y = iy; z = iz;
    }
    bool IsValid() const
    {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
    }
    void Invalidate()
    {
        x = y = z = std::numeric_limits<float>::infinity();
    }

    float& operator[](int i)
    {
        return ((float*)this)[i];
    }
    float operator[](int i) const
    {
        return ((float*)this)[i];
    }

    void Zero()
    {
        x = y = z = 0.0f;
    }

    bool operator==(const SDKVec3& src) const
    {
        return (src.x == x) && (src.y == y) && (src.z == z);
    }
    bool operator!=(const SDKVec3& src) const
    {
        return (src.x != x) || (src.y != y) || (src.z != z);
    }

    SDKVec3& operator+=(const SDKVec3& v)
    {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
    SDKVec3& operator-=(const SDKVec3& v)
    {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }
    SDKVec3& operator*=(float fl)
    {
        x *= fl;
        y *= fl;
        z *= fl;
        return *this;
    }
    SDKVec3& operator*=(const SDKVec3& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }
    SDKVec3& operator/=(const SDKVec3& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }
    SDKVec3& operator+=(float fl)
    {
        x += fl;
        y += fl;
        z += fl;
        return *this;
    }
    SDKVec3& operator/=(float fl)
    {
        x /= fl;
        y /= fl;
        z /= fl;
        return *this;
    }
    SDKVec3& operator-=(float fl)
    {
        x -= fl;
        y -= fl;
        z -= fl;
        return *this;
    }

    void NormalizeInPlace()
    {
        *this = Normalized();
    }
    SDKVec3 Normalized() const
    {
        SDKVec3 res = *this;
        float l = res.Length();
        if (l != 0.0f) {
            res /= l;
        }
        else {
            res.x = res.y = res.z = 0.0f;
        }
        return res;
    }

    float DistTo(const SDKVec3& vOther) const
    {
        SDKVec3 delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;
        delta.z = z - vOther.z;

        return delta.Length();
    }
    float DistToSqr(const SDKVec3& vOther) const
    {
        SDKVec3 delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;
        delta.z = z - vOther.z;

        return delta.LengthSqr();
    }
    float Dot(const SDKVec3& vOther) const
    {
        return (x * vOther.x + y * vOther.y + z * vOther.z);
    }
    float Length() const
    {
        return sqrt(x * x + y * y + z * z);
    }
    float LengthSqr(void) const
    {
        return (x * x + y * y + z * z);
    }
    float Length2D() const
    {
        return sqrt(x * x + y * y);
    }

    SDKVec3& operator=(const SDKVec3& vOther)
    {
        x = vOther.x; y = vOther.y; z = vOther.z;
        return *this;
    }

    SDKVec3 operator-(void) const
    {
        return SDKVec3(-x, -y, -z);
    }
    SDKVec3 operator+(const SDKVec3& v) const
    {
        return SDKVec3(x + v.x, y + v.y, z + v.z);
    }
    SDKVec3 operator-(const SDKVec3& v) const
    {
        return SDKVec3(x - v.x, y - v.y, z - v.z);
    }
    SDKVec3 operator*(float fl) const
    {
        return SDKVec3(x * fl, y * fl, z * fl);
    }
    SDKVec3 operator*(const SDKVec3& v) const
    {
        return SDKVec3(x * v.x, y * v.y, z * v.z);
    }
    SDKVec3 operator/(float fl) const
    {
        return SDKVec3(x / fl, y / fl, z / fl);
    }
    SDKVec3 operator/(const SDKVec3& v) const
    {
        return SDKVec3(x / v.x, y / v.y, z / v.z);
    }

    float x, y, z;
};

inline SDKVec3 operator*(float lhs, const SDKVec3& rhs)
{
    return rhs * lhs;
}
inline SDKVec3 operator/(float lhs, const SDKVec3& rhs)
{
    return rhs / lhs;
}

class ALIGN16 VectorAligned : public SDKVec3
{
public:
    inline VectorAligned(void) {};

    inline VectorAligned(SDKVec3 X, SDKVec3 Y, SDKVec3 Z)
    {
        //Init(X, Y, Z);
    }

    explicit VectorAligned(const SDKVec3& vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
    }

    VectorAligned& operator=(const SDKVec3& vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
        return *this;
    }

    float w;	// this space is used anyway
} ALIGN16_POST;