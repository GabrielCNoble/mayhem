#version 330

in vec4 pos;
in vec4 normal;

struct light_t
{
    vec4 pos_radius;
    vec4 col_energy;
};

struct light_interface_t
{
    light_t r_Lights[64];
    int r_LightCount;
};

layout(std140) uniform r_LightsUniformBlock
{
    light_interface_t r_LightInterface;
};

uniform vec4 r_BaseColor;

void main()
{
    int i;

    vec3 color = vec3(0);

    for(i = 0; i < r_LightInterface.r_LightCount; i++)
    {
        light_t light = r_LightInterface.r_Lights[i];
        vec3 light_pos = light.pos_radius.xyz;
        vec3 light_vec = light_pos - pos.xyz;
        float distance = length(light_vec);
        light_vec /= distance;

        float attenuation = (1.0 / distance) * max(1.0 - (distance / light.pos_radius.w), 0.0) * light.col_energy.w;

        float l = clamp(dot(light_vec, normal.xyz), 0.0, 1.0) * 10.0;
        color += (light.col_energy.rgb * r_BaseColor.rgb * l) * attenuation;
    }

    gl_FragColor = vec4(color, 1.0);

//    vec3 light_pos = vec3(6.0, 1.0, 0.0);
//    vec3 light_vec = light_pos - pos.xyz;
//    float falloff = length(light_vec);
//    light_vec /= falloff;
//
//    float l = clamp(dot(light_vec, normal.xyz), 0.0, 1.0) * 20.0;
//    gl_FragColor = ((r_BaseColor / 3.14159265) * l) / (falloff * falloff * 0.8);
}










