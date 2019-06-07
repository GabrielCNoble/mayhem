#version 330

in vec4 r_VertexPosition;

out vec4 pos;

uniform mat4 r_ViewMatrix;
uniform mat4 r_ModelViewProjectionMatrix;

void main()
{
    pos = r_ViewMatrix * r_VertexPosition;
    gl_Position = r_ModelViewProjectionMatrix * r_VertexPosition;
    //gl_ClipDistance[0] = dot(pos.xyz, normalize(r_NearPlane.xyz)) + r_NearPlane.w;
}
