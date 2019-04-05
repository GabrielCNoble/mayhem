#version 330

in vec4 r_VertexPosition;
in vec4 r_VertexNormal;

out vec4 pos;
out vec4 normal;

uniform mat4 r_ModelViewProjectionMatrix;

void main()
{
    pos = r_VertexPosition;
    normal = r_VertexNormal;
    gl_Position = r_ModelViewProjectionMatrix * r_VertexPosition;
}
