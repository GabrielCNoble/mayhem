#version 330

in vec4 r_VertexPosition;
in vec4 r_VertexNormal;

out vec4 pos;
out vec4 normal;

uniform mat4 r_ModelViewProjectionMatrix;
uniform mat4 r_ViewMatrix;

void main()
{
    pos = r_ViewMatrix * r_VertexPosition;
    normal = r_ViewMatrix * vec4(r_VertexNormal.xyz, 0.0);
    gl_Position = r_ModelViewProjectionMatrix * r_VertexPosition;
}
