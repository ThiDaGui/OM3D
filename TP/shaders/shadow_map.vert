#version 450

#include "utils.glsl"

layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

layout(binding = 0) uniform Data {
    FrameData frame;
};
out vec4 test;

void main()
{
    lightSpaceMatrix[2][2] = 0.0f;
    mat4 test_m = mat4( 1.33364f, -0.707107f, 0.f, -0.57735f,
                      0.f, 1.41421f, 0.f, -0.57735f,
                      -1.33364f, -0.707107f, 0.f, -0.57735f,
                      0.f, 0.f, 0.001f, 3.46417f);
    //gl_Position = frame.light_proj.view_proj * model * vec4(aPos, 1.0);
    //gl_Position =  lightSpaceMatrix * model * vec4(aPos, 1.0);
    //gl_Position = frame.camera.view_proj * model * vec4(aPos, 1.0);
    gl_Position = test_m * model * vec4(aPos, 1.0);
    test = vec4(lightSpaceMatrix[2][0], lightSpaceMatrix[2][1], lightSpaceMatrix[2][2], 1.0f);
    //test = vec4(test_m[3][0], test_m[3][1], test_m[3][2], 1.0f);
}
