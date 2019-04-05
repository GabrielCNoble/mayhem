#include "vector.h"
#include "matrix.h"
#include <math.h>

vec4_t::vec4_t()
{

}

vec4_t::vec4_t(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

vec4_t::vec4_t(const vec3_t& vec3, float w)
{
    this->x = vec3.x;
    this->y = vec3.y;
    this->z = vec3.z;
    this->w = w;
}



vec4_t vec4_t::operator+(const vec4_t &vec)
{
    return vec4_t(this->x + vec.x, this->y + vec.y, this->z + vec.z, this->w + vec.w);
}

vec4_t vec4_t::operator += (const vec4_t &vec)
{
    this->x += vec.x;
    this->y += vec.y;
    this->z += vec.z;
    this->w += vec.w;
    return *this;
}



vec4_t vec4_t::operator - (const vec4_t &vec)
{
    return vec4_t(this->x - vec.x, this->y - vec.y, this->z - vec.z, this->w - vec.w);
}

vec4_t vec4_t::operator - ()
{
    return vec4_t(-this->x, -this->y, -this->z, -this->w);
}

vec4_t vec4_t::operator -= (const vec4_t &vec)
{
    this->x -= vec.x;
    this->y -= vec.y;
    this->z -= vec.z;
    this->w -= vec.w;
    return *this;
}



vec4_t vec4_t::operator * (float s) const
{
    return vec4_t(this->x * s, this->y * s, this->z * s, this->w * s);
}

vec4_t vec4_t::operator * (const mat4_t &mat) const
{
    return vec4_t(this->x * mat.rows[0] + this->y * mat.rows[1] + this->z * mat.rows[2] + this->w * mat.rows[3]);
}

vec4_t operator * (float s, const vec4_t &vec)
{
    return vec4_t(vec.x * s, vec.y * s, vec.z * s, vec.w * s);
}

vec4_t vec4_t::operator *= (float s)
{
    this->x *= s;
    this->y *= s;
    this->z *= s;
    this->w *= s;
    return *this;
}



vec4_t vec4_t::operator / (float s)
{
    return vec4_t(this->x / s, this->y / s, this->z / s, this->w / s);
}

vec4_t operator / (float s, const vec4_t &vec)
{
    return vec4_t(vec.x / s, vec.y / s, vec.z / s, vec.w / s);
}

vec4_t vec4_t::operator /= (float s)
{
    this->x /= s;
    this->y /= s;
    this->z /= s;
    this->w /= s;
    return *this;
}

float & vec4_t::operator [] (int index)
{
    return this->floats[index];
}

float vec4_t::length() const
{
    float len;

    len = this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w;

    if(len > 0.0)
    {
        return sqrt(len);
    }

    return 0.0;
}

vec4_t normalize(const vec4_t &vec)
{
    float len = vec.length();
    return vec4_t(vec.x / len, vec.y / len, vec.z / len, vec.w / len);
}

float dot(const vec4_t &a, const vec4_t &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}






/*
=====================================================================
=====================================================================
=====================================================================
*/

vec3_t::vec3_t()
{

}

vec3_t::vec3_t(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}



vec3_t vec3_t::operator+(const vec3_t &vec)
{
    return vec3_t(this->x + vec.x, this->y + vec.y, this->z + vec.z);
}

vec3_t vec3_t::operator += (const vec3_t &vec)
{
    this->x += vec.x;
    this->y += vec.y;
    this->z += vec.z;
    return *this;
}



vec3_t vec3_t::operator - (const vec3_t &vec)
{
    return vec3_t(this->x - vec.x, this->y - vec.y, this->z - vec.z);
}

vec3_t vec3_t::operator - ()
{
    return vec3_t(-this->x, -this->y, -this->z);
}

vec3_t vec3_t::operator -= (const vec3_t &vec)
{
    this->x -= vec.x;
    this->y -= vec.y;
    this->z -= vec.z;
    return *this;
}



vec3_t vec3_t::operator * (float s) const
{
    return vec3_t(this->x * s, this->y * s, this->z * s);
}

vec3_t vec3_t::operator * (const mat3_t &mat) const
{
    return vec3_t(this->x * mat.rows[0] + this->y * mat.rows[1] + this->z * mat.rows[2]);
}

vec3_t operator * (float s, const vec3_t &vec)
{
    return vec3_t(vec.x * s, vec.y * s, vec.z * s);
}

vec3_t vec3_t::operator *= (float s)
{
    this->x *= s;
    this->y *= s;
    this->z *= s;
    return *this;
}



vec3_t vec3_t::operator / (float s)
{
    return vec3_t(this->x / s, this->y / s, this->z / s);
}

vec3_t operator / (float s, const vec3_t &vec)
{
    return vec3_t(vec.x / s, vec.y / s, vec.z / s);
}

vec3_t vec3_t::operator /= (float s)
{
    this->x /= s;
    this->y /= s;
    this->z /= s;
    return *this;
}

float & vec3_t::operator [] (int index)
{
    return this->floats[index];
}

float vec3_t::length() const
{
    float len;

    len = this->x * this->x + this->y * this->y + this->z * this->z;

    if(len > 0.0)
    {
        return sqrt(len);
    }

    return 0.0;
}

vec3_t normalize(const vec3_t &vec)
{
    float len = vec.length();

    if(len)
    {
        return vec3_t(vec.x / len, vec.y / len, vec.z / len);
    }

    return vec3_t(0.0, 0.0, 0.0);
}

float length(const vec3_t &vec)
{
    return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

float dot(const vec3_t &a, const vec3_t &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3_t cross(const vec3_t &a, const vec3_t &b)
{
    return vec3_t(a.z * b.y - a.y * b.z, a.x * b.z - a.z * b.x, a.y * b.x - a.x * b.y);
}

vec3_t vfabs(const vec3_t& vec)
{
    return vec3_t(fabs(vec.x), fabs(vec.y), fabs(vec.z));
}

/*
=====================================================================
=====================================================================
=====================================================================
*/



vec2_t::vec2_t()
{

}

vec2_t::vec2_t(float x, float y)
{
    this->x = x;
    this->y = y;
}



vec2_t vec2_t::operator+(const vec2_t &vec)
{
    return vec2_t(this->x + vec.x, this->y + vec.y);
}

vec2_t vec2_t::operator += (const vec2_t &vec)
{
    this->x += vec.x;
    this->y += vec.y;
    return *this;
}



vec2_t vec2_t::operator - (const vec2_t &vec)
{
    return vec2_t(this->x - vec.x, this->y - vec.y);
}

vec2_t vec2_t::operator - ()
{
    return vec2_t(-this->x, -this->y);
}

vec2_t vec2_t::operator -= (const vec2_t &vec)
{
    this->x -= vec.x;
    this->y -= vec.y;
    return *this;
}



vec2_t vec2_t::operator * (float s) const
{
    return vec2_t(this->x * s, this->y * s);
}

vec2_t operator * (float s, const vec2_t &vec)
{
    return vec2_t(vec.x * s, vec.y * s);
}

vec2_t vec2_t::operator *= (float s)
{
    this->x *= s;
    this->y *= s;
    return *this;
}



vec2_t vec2_t::operator / (float s)
{
    return vec2_t(this->x / s, this->y / s);
}

vec2_t operator / (float s, const vec2_t &vec)
{
    return vec2_t(vec.x / s, vec.y / s);
}

vec2_t vec2_t::operator /= (float s)
{
    this->x /= s;
    this->y /= s;
    return *this;
}

float & vec2_t::operator [] (int index)
{
    return this->floats[index];
}

float vec2_t::length() const
{
    float len;

    len = this->x * this->x + this->y * this->y;

    if(len > 0.0)
    {
        return sqrt(len);
    }

    return 0.0;
}

vec2_t normalize(const vec2_t &vec)
{
    float len = vec.length();
    return vec2_t(vec.x / len, vec.y / len);
}

float dot(const vec2_t &a, const vec2_t &b)
{
    return a.x * b.x + a.y * b.y;
}





