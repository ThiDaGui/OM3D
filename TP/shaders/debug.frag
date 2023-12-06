#version 450

#include "utils.glsl"

layout(location = 0) out vec4 out_color;

layout(location = 0) in vec2 in_uv;

layout(binding = 0) uniform sampler2D in_albedo;
layout(binding = 1) uniform sampler2D in_normal;
layout(binding = 2) uniform sampler2D in_depth;

uniform uint to_debug = 0;



void main() {
    const ivec2 coord = ivec2(gl_FragCoord.xy);

    vec3 hdr;
	switch (to_debug)
	{
		case 1:
			hdr = texelFetch(in_albedo, coord, 0).rgb;
			break;
		case 2:
			hdr = texelFetch(in_normal, coord, 0).rgb;
			break;
		case 3:
			float depth = texelFetch(in_depth, coord, 0).r;
			depth = pow(depth, 0.35);
			hdr = vec3(depth);
			break;
	}


    out_color = vec4(hdr, 1.0);
}


