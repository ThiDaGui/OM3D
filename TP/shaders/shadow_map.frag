#version 450
layout(location = 0) out vec4 out_color;

void main()
{
    out_color = vec4(vec3(gl_FragCoord.z) * 10000.0f, 1.0);
}
