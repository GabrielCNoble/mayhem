#version 330

in vec4 pos;
in vec4 normal;

struct light_t
{
    vec4 pos_radius;
    vec4 col_energy;
};

uniform vec4 r_BaseColor;

void main()
{
    vec3 light_pos = vec3(6.0, 1.0, 0.0);
    vec3 light_vec = light_pos - pos.xyz;
    float falloff = length(light_vec);
    light_vec /= falloff;

    float l = clamp(dot(light_vec, normal.xyz), 0.0, 1.0) * 20.0;
    gl_FragColor = ((r_BaseColor / 3.14159265) * l) / (falloff * falloff * 0.8);
}
