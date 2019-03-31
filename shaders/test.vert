attribute vec4 r_VertexPosition;
attribute vec4 r_VertexNormal;

varying vec4 color;

uniform mat4 r_ModelViewProjectionMatrix;

void main()
{
    color = gl_Color;
    gl_Position = r_ModelViewProjectionMatrix * r_VertexPosition;
}
