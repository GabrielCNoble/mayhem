varying vec4 pos;
varying vec4 normal;

struct light_t
{
    vec4 pos_radius;
    vec4 col_energy;
};

uniform vec4 r_BaseColor;

void main()
{
    vec3 light_pos = vec3(6.0, 1.0, 0.0);
    vec3 light_vec = normalize(light_pos - pos.xyz);
    float l = dot(light_vec, normal.xyz);
    gl_FragColor = (r_BaseColor) * l * l;
    //gl_FragColor = r_BaseColor;
}
