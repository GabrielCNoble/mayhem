#version 330

in vec4 r_VertexPosition;


uniform mat4 r_ViewProjectionMatrix;

void main()
{
    gl_Position = r_ViewProjectionMatrix * r_VertexPosition;
}
