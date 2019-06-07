#ifndef VECTOR_H
#define VECTOR_H




class mat4_t;
class vec3_t;

class vec4_t
{
    public:

        union
        {
            float floats[4];

            struct
            {
                float x;
                float y;
                float z;
                float w;
            };
        };

        vec4_t();
        vec4_t(float x, float y, float z, float w);
        vec4_t(const vec3_t& vec3, float w);

        vec4_t operator + (const vec4_t &vec);
        vec4_t operator += (const vec4_t &vec);

        vec4_t operator - (const vec4_t &vec);
        vec4_t operator - ();
        vec4_t operator -= (const vec4_t &vec);

        vec4_t operator * (float s) const;
        vec4_t operator * (const mat4_t &mat) const;
        vec4_t operator *= (float s);

        vec4_t operator / (float s);
        vec4_t operator /= (float s);

        float& operator [] (int index);

        float length() const;
};

vec4_t operator * (float s, const vec4_t &vec);

vec4_t operator / (float s, const vec4_t &vec);

float dot(const vec4_t &a, const vec4_t &b);



/*
=====================================================================
=====================================================================
=====================================================================
*/



class mat3_t;

class vec3_t
{
    public:

        union
        {
            float floats[3];

            struct
            {
                float x;
                float y;
                float z;
            };
        };

        vec3_t();
        vec3_t(float x, float y, float z);

        vec3_t operator + (const vec3_t &vec);
        vec3_t operator += (const vec3_t &vec);

        vec3_t operator - (const vec3_t &vec);
        vec3_t operator - ();
        vec3_t operator -= (const vec3_t &vec);

        vec3_t operator * (float s) const;
        vec3_t operator * (const mat3_t &mat) const;
        vec3_t operator *= (float s);

        vec3_t operator / (float s);
        vec3_t operator /= (float s);

        float& operator [] (int index);

        float length() const;
};

vec3_t operator * (float s, const vec3_t &vec);

vec3_t operator / (float s, const vec3_t &vec);

vec3_t normalize(const vec3_t &vec);

float length(const vec3_t &vec);

float dot(const vec3_t &a, const vec3_t &b);

vec3_t cross(const vec3_t &a, const vec3_t &b);

vec3_t vfabs(const vec3_t& vec);


/*
=====================================================================
=====================================================================
=====================================================================
*/



class vec2_t
{
    public:

        union
        {
            float floats[2];

            struct
            {
                float x;
                float y;
            };
        };

        vec2_t();
        vec2_t(float x, float y);

        vec2_t operator + (const vec2_t &vec);
        vec2_t operator += (const vec2_t &vec);

        vec2_t operator - (const vec2_t &vec);
        vec2_t operator - ();
        vec2_t operator -= (const vec2_t &vec);

        vec2_t operator * (float s) const;
        vec2_t operator *= (float s);

        vec2_t operator / (float s);
        vec2_t operator /= (float s);

        float& operator [] (int index);

        float length() const;
};

vec2_t operator * (float s, const vec2_t &vec);

vec2_t operator / (float s, const vec2_t &vec);

vec2_t normalize(const vec2_t &vec);

float dot(const vec2_t &a, const vec2_t &b);


#endif // VECTOR_H










