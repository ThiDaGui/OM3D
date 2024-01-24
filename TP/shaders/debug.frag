#version 450

#include "utils.glsl"

layout(location = 0) out vec4 out_color;

layout(location = 0) in vec2 in_uv;

layout(binding = 0) uniform sampler2D in_albedo;
layout(binding = 1) uniform sampler2D in_normal;
layout(binding = 2) uniform sampler2D in_depth;
layout(binding = 3) uniform sampler2D in_shadow_map;

uniform uint to_debug = 0;



void main() {
    const ivec2 coord = ivec2(gl_FragCoord.xy);

    vec3 hdr = vec3(0.0);
	float depth = texelFetch(in_depth, coord, 0).r;
	vec3 shadow_depth = texelFetch(in_shadow_map, coord, 0).rgb;
	vec3 albedo = texelFetch(in_albedo, coord, 0).rgb;
	vec3 normal = texelFetch(in_normal, coord, 0).rgb * 2.0 - vec3(1.0);
	switch (to_debug)
	{
		case 1:
			hdr = albedo;
			break;
		case 2:
			hdr = normal * 0.5 + vec3(0.5);
			break;
		case 3:
			depth = depth;
			hdr = vec3(depth);
			break;
        case 4:
			hdr = shadow_depth;
			break;
	}


    out_color = vec4(hdr, 1.0);
}


