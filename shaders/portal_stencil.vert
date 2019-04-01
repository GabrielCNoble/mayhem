attribute vec4 r_VertexPosition;

uniform mat4 r_ModelViewProjectionMatrix;

void main()
{
    gl_Position = r_ModelViewProjectionMatrix * r_VertexPosition;
}
