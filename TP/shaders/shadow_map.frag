#version 450
layout(location = 0) out vec4 out_color;

in vec4 test;

void main()
{
    //gl_FragDepth = gl_FragCoord.z;
    //out_color = vec4(vec3(clamp(gl_FragCoord.z, 0.0, 1.0)), 1.0);
    out_color = test;
    //out_color = vec4(1.0);
}
