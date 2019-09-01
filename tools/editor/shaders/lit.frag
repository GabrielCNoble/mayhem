#version 330

in vec4 pos;
in vec4 normal;
in vec4 tangent;
in vec2 tex_coords;

struct light_t
{
    vec4 pos_radius;
    vec4 col_energy;
    vec4 w_pos;
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
uniform vec4 r_NearPlane;
uniform int r_MaterialFlags;
uniform sampler2D r_TextureSampler0;
uniform sampler2D r_TextureSampler1;

void main()
{
    int i;

    vec3 color = vec3(0);
    vec4 pixel_color;
    vec4 pixel_normal;

    if(dot(pos.xyz, r_NearPlane.xyz) + r_NearPlane.w < 0.0)
    {
        discard;
    }

    if((r_MaterialFlags & 1) != 0)
    {
        pixel_color = texture2D(r_TextureSampler0, tex_coords);
    }
    else
    {
        pixel_color = r_BaseColor;
    }

    if((r_MaterialFlags & 2) != 0)
    {
        mat3 tbn;

        tbn[2] = normal.xyz;
        tbn[1] = tangent.xyz;
        tbn[0] = -cross(normal.xyz, tangent.xyz);

        pixel_normal.xyz = tbn * normalize(texture2D(r_TextureSampler1, tex_coords).xyz * 2.0 - vec3(1.0));

        //pixel_normal.x = -pixel_normal.x;
    }
    else
    {
        pixel_normal = normal;
    }

    for(i = 0; i < r_LightInterface.r_LightCount; i++)
    {
        light_t light = r_LightInterface.r_Lights[i];
        vec3 light_pos = light.pos_radius.xyz;
        vec3 light_vec = light_pos - pos.xyz;
        float distance = length(light_vec);
        light_vec /= distance;

        float attenuation = (1.0 / distance) * max(1.0 - (distance / light.pos_radius.w), 0.0) * light.col_energy.w;

        float l = clamp(dot(light_vec, pixel_normal.xyz), 0.0, 1.0) * 10.0;
        color += (light.col_energy.rgb * pixel_color.rgb * l) * attenuation;
    }

    gl_FragColor = vec4(color.xyz, 1.0);

//    gl_FragColor = vec4(tex_coords.xy, 0.0, 0.0);
//    gl_FragColor = base_color;

//    vec3 light_pos = vec3(6.0, 1.0, 0.0);
//    vec3 light_vec = light_pos - pos.xyz;
//    float falloff = length(light_vec);
//    light_vec /= falloff;
//
//    float l = clamp(dot(light_vec, normal.xyz), 0.0, 1.0) * 20.0;
//    gl_FragColor = ((r_BaseColor / 3.14159265) * l) / (falloff * falloff * 0.8);
}










