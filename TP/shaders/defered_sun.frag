#version 450

#include "utils.glsl"

layout(location = 0) out vec4 out_color;

layout(location = 0) in vec2 in_uv;

layout(binding = 0) uniform sampler2D in_albedo;
layout(binding = 1) uniform sampler2D in_normal;
layout(binding = 2) uniform sampler2D in_depth;

layout(binding = 0) uniform Data {
    FrameData frame;
};


vec3 unproject(vec2 uv, float depth, mat4 inv_viewproj) {
	const vec3 ndc = vec3(uv * 2.0 - vec2(1.0), depth);
	const vec4 p = inv_viewproj * vec4(ndc, 1.0);
	return p.xyz / p.w;
}

void main() {
    const ivec2 coord = ivec2(gl_FragCoord.xy);

	vec3 normal = (texelFetch(in_normal, coord, 0).rgb) * 2.0 - 1.0;
	vec3 albedo = texelFetch(in_albedo, coord, 0).rgb;
	float depth = texelFetch(in_depth, coord, 0).r;

	mat4 inv_viewproj = inverse(frame.camera.view_proj);

	vec3 world_pos = unproject(in_uv, depth, inv_viewproj);

    vec3 hdr = frame.sun_color * max(0.0, dot(frame.sun_dir, normal));

	hdr *= albedo;

    out_color = vec4(hdr, 1.0);
}
