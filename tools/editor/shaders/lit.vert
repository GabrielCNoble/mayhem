#version 330

in vec4 r_VertexPosition;
in vec4 r_VertexNormal;
in vec4 r_VertexTangent;
in vec2 r_VertexTexCoords;

out vec4 pos;
out vec4 normal;
out vec4 tangent;
out vec2 tex_coords;

uniform mat4 r_ModelViewProjectionMatrix;
uniform mat4 r_ViewMatrix;

void main()
{
    pos = r_ViewMatrix * r_VertexPosition;
    normal = r_ViewMatrix * vec4(r_VertexNormal.xyz, 0.0);
    tangent = r_ViewMatrix * vec4(r_VertexTangent.xyz, 0.0);
    tex_coords = r_VertexTexCoords;
    gl_Position = r_ModelViewProjectionMatrix * r_VertexPosition;
    //gl_ClipDistance[0] = dot(pos.xyz, r_NearPlane.xyz) + r_NearPlane.w;
}
