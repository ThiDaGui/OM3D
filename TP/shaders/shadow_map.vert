#version 450

#include "utils.glsl"

layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

layout(binding = 0) uniform Data {
    FrameData frame;
};

void main()
{
    gl_Position =  lightSpaceMatrix * model * vec4(aPos, 1.0);
}
