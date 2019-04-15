#version 330

in vec4 pos;
uniform vec4 r_NearPlane;

void main()
{
    if(dot(pos.xyz, r_NearPlane.xyz) + r_NearPlane.w < 0.0)
    {
        discard;
    }


    gl_FragColor = vec4(0.0);
}
