#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

class mat3_t;

class mat4_t
{
    public:

        union
        {
            float floats[4][4];

            struct
            {
                float x0, y0, z0, w0;
                float x1, y1, z1, w1;
                float x2, y2, z2, w2;
                float x3, y3, z3, w3;
            };

            vec4_t rows[4];
        };

        mat4_t();

        mat4_t(const vec4_t &r0, const vec4_t &r1, const vec4_t &r2, const vec4_t &r3);

        mat4_t operator * (const mat4_t &mat) const;

        mat4_t operator *= (const mat4_t &mat);

        vec4_t &operator[] (int index);

        void identity();

        void transpose();

        mat3_t rot_matrix();
};

//
//mat4_t identity();
//
//mat4_t transpose(const mat4_t &mat);

//mat4_t rotate_x(const mat4_t &mat, float angle);
//
//mat4_t rotate_y(const mat4_t &mat, float angle);
//
//mat4_t rotate_z(const mat4_t &mat, float angle);

mat4_t perspective(float fov_y, float aspect, float z_near, float z_far);

/*
=====================================================================
=====================================================================
=====================================================================
*/

class mat3_t
{
    public:

        union
        {
            float floats[3][3];

            struct
            {
                float x0, y0, z0;
                float x1, y1, z1;
                float x2, y2, z2;
            };

            vec3_t rows[3];
        };

        mat3_t();

        mat3_t(const vec3_t &r0, const vec3_t &r1, const vec3_t &r2);

        mat3_t operator * (const mat3_t &mat) const;

        mat3_t operator *= (const mat3_t &mat);

        vec3_t &operator[] (int index);

        void identity();

        void transpose();
};

//
//mat3_t identity();
//
//mat3_t transpose(const mat3_t &mat);

mat3_t rotate_x(const mat3_t &mat, float angle);

mat3_t rotate_y(const mat3_t &mat, float angle);

mat3_t rotate_z(const mat3_t &mat, float angle);



#endif // MATRIX_H





