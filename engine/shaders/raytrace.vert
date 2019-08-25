#version 330

in vec4 r_VertexPosition;
in vec4 r_VertexNormal;

uniform mat4 r_ModelViewProjectionMatrix;

out vec4 world_position;
out vec4 world_normal;

void main()
{
    world_position = r_VertexPosition;
    world_normal = r_VertexNormal;

    gl_Position = gl_ModelViewProjectionMatrix * r_VertexPosition;
}
