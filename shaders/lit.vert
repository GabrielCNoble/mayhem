attribute vec4 r_VertexPosition;
attribute vec4 r_VertexNormal;

varying vec4 pos;
varying vec4 normal;

uniform mat4 r_ModelViewProjectionMatrix;

void main()
{
    pos = r_VertexPosition;
    normal = r_VertexNormal;
    gl_Position = r_ModelViewProjectionMatrix * r_VertexPosition;
}
