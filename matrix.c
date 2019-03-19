#include "matrix.h"
#include <math.h>



mat4_t::mat4_t()
{

}

mat4_t::mat4_t(const vec4_t &r0, const vec4_t &r1, const vec4_t &r2, const vec4_t &r3)
{
    this->rows[0] = r0;
    this->rows[1] = r1;
    this->rows[2] = r2;
    this->rows[3] = r3;
}

mat4_t mat4_t::operator * (const mat4_t &mat) const
{
    mat4_t result;

    result.rows[0] = mat.rows[0] * this->rows[0].x +
                     mat.rows[1] * this->rows[0].y +
                     mat.rows[2] * this->rows[0].z +
                     mat.rows[3] * this->rows[0].w;

    result.rows[1] = mat.rows[0] * this->rows[1].x +
                     mat.rows[1] * this->rows[1].y +
                     mat.rows[2] * this->rows[1].z +
                     mat.rows[3] * this->rows[1].w;

    result.rows[2] = mat.rows[0] * this->rows[2].x +
                     mat.rows[1] * this->rows[2].y +
                     mat.rows[2] * this->rows[2].z +
                     mat.rows[3] * this->rows[2].w;

    result.rows[3] = mat.rows[0] * this->rows[3].x +
                     mat.rows[1] * this->rows[3].y +
                     mat.rows[2] * this->rows[3].z +
                     mat.rows[3] * this->rows[3].w;

    return result;
}

mat4_t mat4_t::operator *= (const mat4_t &mat)
{
    this->rows[0] = mat.rows[0] * this->rows[0].x +
                    mat.rows[1] * this->rows[0].y +
                    mat.rows[2] * this->rows[0].z +
                    mat.rows[3] * this->rows[0].w;

    this->rows[1] = mat.rows[0] * this->rows[1].x +
                    mat.rows[1] * this->rows[1].y +
                    mat.rows[2] * this->rows[1].z +
                    mat.rows[3] * this->rows[1].w;

    this->rows[2] = mat.rows[0] * this->rows[2].x +
                    mat.rows[1] * this->rows[2].y +
                    mat.rows[2] * this->rows[2].z +
                    mat.rows[3] * this->rows[2].w;

    this->rows[3] = mat.rows[0] * this->rows[3].x +
                    mat.rows[1] * this->rows[3].y +
                    mat.rows[2] * this->rows[3].z +
                    mat.rows[3] * this->rows[3].w;

    return *this;
}

vec4_t &mat4_t::operator[] (int index)
{
    return this->rows[index];
}

void mat4_t::identity()
{
    this->rows[0] = vec4_t(1.0, 0.0, 0.0, 0.0);
    this->rows[1] = vec4_t(0.0, 1.0, 0.0, 0.0);
    this->rows[2] = vec4_t(0.0, 0.0, 1.0, 0.0);
    this->rows[3] = vec4_t(0.0, 0.0, 0.0, 1.0);
}

mat4_t identity()
{
    return mat4_t(vec4_t(1.0, 0.0, 0.0, 0.0),
                  vec4_t(0.0, 1.0, 0.0, 0.0),
                  vec4_t(0.0, 0.0, 1.0, 0.0),
                  vec4_t(0.0, 0.0, 0.0, 1.0));
}

void mat4_t::transpose()
{
//    float temp;
//
//    temp = this->floats[0][1];
//    this->floats[0][1] = this->floats[1][0];
//    this->floats[1][0] = temp;
//
//    temp = this->floats[0][2];
//    this->floats[0][2] = this->floats[2][0];
//    this->floats[2][0] = temp;
//
//    temp = this->floats[2][1];
//    this->floats[2][1] = this->floats[1][2];
//    this->floats[1][2] = temp;
}

mat3_t mat4_t::rot_matrix()
{
    mat3_t rot;

    rot.floats[0][0] = this->floats[0][0];
    rot.floats[0][1] = this->floats[0][1];
    rot.floats[0][2] = this->floats[0][2];

    rot.floats[1][0] = this->floats[1][0];
    rot.floats[1][1] = this->floats[1][1];
    rot.floats[1][2] = this->floats[1][2];

    rot.floats[2][0] = this->floats[2][0];
    rot.floats[2][1] = this->floats[2][1];
    rot.floats[2][2] = this->floats[2][2];

    return rot;
}

mat4_t transpose(const mat4_t &mat)
{
//    mat4_t result;
//
//    result.floats[0][0] = mat.floats[0][0];
//    result.floats[0][1] = mat.floats[1][0];
//    result.floats[1][0] = mat.floats[0][1];
//    result.floats[0][2] = mat.floats[2][0];
//    result.floats[2][0] = mat.floats[0][2];
//    result.floats[1][1] = mat.floats[1][1];
//    result.floats[2][1] = mat.floats[1][2];
//    result.floats[1][2] = mat.floats[2][1];
//    result.floats[2][2] = mat.floats[2][2];
//
//    return result;
}


mat4_t perspective(float fov_y, float aspect, float z_near, float z_far)
{

}

/*
=====================================================================
=====================================================================
=====================================================================
*/

mat3_t::mat3_t()
{

}

mat3_t::mat3_t(const vec3_t &r0, const vec3_t &r1, const vec3_t &r2)
{
    this->rows[0] = r0;
    this->rows[1] = r1;
    this->rows[2] = r2;
}

mat3_t mat3_t::operator * (const mat3_t &mat) const
{
    mat3_t result;

    result.rows[0] = mat.rows[0] * this->rows[0].x +
                     mat.rows[1] * this->rows[0].y +
                     mat.rows[2] * this->rows[0].z;

    result.rows[1] = mat.rows[0] * this->rows[1].x +
                     mat.rows[1] * this->rows[1].y +
                     mat.rows[2] * this->rows[1].z;

    result.rows[2] = mat.rows[0] * this->rows[2].x +
                     mat.rows[1] * this->rows[2].y +
                     mat.rows[2] * this->rows[2].z;

    return result;
}

mat3_t mat3_t::operator *= (const mat3_t &mat)
{
    mat3_t result;

    this->rows[0] = mat.rows[0] * this->rows[0].x +
                    mat.rows[1] * this->rows[0].y +
                    mat.rows[2] * this->rows[0].z;

    this->rows[1] = mat.rows[0] * this->rows[1].x +
                    mat.rows[1] * this->rows[1].y +
                    mat.rows[2] * this->rows[1].z;

    this->rows[2] = mat.rows[0] * this->rows[2].x +
                    mat.rows[1] * this->rows[2].y +
                    mat.rows[2] * this->rows[2].z;

    return *this;
}

vec3_t &mat3_t::operator[] (int index)
{
    return this->rows[index];
}

void mat3_t::identity()
{
    this->rows[0] = vec3_t(1.0, 0.0, 0.0);
    this->rows[1] = vec3_t(0.0, 1.0, 0.0);
    this->rows[2] = vec3_t(0.0, 0.0, 1.0);
}

//mat3_t identity()
//{
//    return mat3_t(vec3_t(1.0, 0.0, 0.0),
//                  vec3_t(0.0, 1.0, 0.0),
//                  vec3_t(0.0, 0.0, 1.0));
//}

void mat3_t::transpose()
{
    float temp;

    temp = this->floats[0][1];
    this->floats[0][1] = this->floats[1][0];
    this->floats[1][0] = temp;

    temp = this->floats[0][2];
    this->floats[0][2] = this->floats[2][0];
    this->floats[2][0] = temp;

    temp = this->floats[2][1];
    this->floats[2][1] = this->floats[1][2];
    this->floats[1][2] = temp;
}

//mat3_t transpose(const mat3_t &mat)
//{
//    mat3_t result;
//
//    result.floats[0][0] = mat.floats[0][0];
//    result.floats[0][1] = mat.floats[1][0];
//    result.floats[1][0] = mat.floats[0][1];
//    result.floats[0][2] = mat.floats[2][0];
//    result.floats[2][0] = mat.floats[0][2];
//    result.floats[1][1] = mat.floats[1][1];
//    result.floats[2][1] = mat.floats[1][2];
//    result.floats[1][2] = mat.floats[2][1];
//    result.floats[2][2] = mat.floats[2][2];
//
//    return result;
//}



mat3_t rotate_x(const mat3_t &mat, float angle)
{
    float s;
    float c;

    s = sin(angle * M_PI);
    c = cos(angle * M_PI);

    return mat3_t(vec3_t(1.0, 0.0, 0.0),
                  vec3_t(0.0,   c,   s),
                  vec3_t(0.0,  -s,   c));
}

mat3_t rotate_y(const mat3_t &mat, float angle)
{
    float s;
    float c;

    s = sin(angle * M_PI);
    c = cos(angle * M_PI);

    return mat3_t(vec3_t(c  , 0.0,  -s),
                  vec3_t(0.0, 1.0, 0.0),
                  vec3_t(s  , 0.0,   c));
}

mat3_t rotate_z(const mat3_t &mat, float angle)
{
    float s;
    float c;

    s = sin(angle * M_PI);
    c = cos(angle * M_PI);

    return mat3_t(vec3_t(c  ,   s, 0.0),
                  vec3_t(-s ,   c, 0.0),
                  vec3_t(0.0, 0.0, 1.0));
}











