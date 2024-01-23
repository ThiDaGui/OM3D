#version 450

#include "utils.glsl"

layout (location = 0) out vec4 out_color;

layout(location = 6) in vec3 in_clip_position;

layout(binding = 0) uniform sampler2D in_albedo;
layout(binding = 1) uniform sampler2D in_normal;
layout(binding = 2) uniform sampler2D in_depth;

layout(binding = 0) uniform Data {
    FrameData frame;
};

uniform float lightRadius;
uniform vec3 lightPos;
uniform vec3 lightColor;

vec3 unproject(vec2 uv, float depth, mat4 inv_viewproj) {
	const vec3 ndc = vec3(uv, depth);
	const vec4 p = inv_viewproj * vec4(ndc, 1.0);
	return p.xyz / p.w;
}

void main() {
    const ivec2 coord = ivec2(gl_FragCoord.xy);

	vec3 normal = (texelFetch(in_normal, coord, 0).rgb) * 2.0 - 1.0;
	vec3 albedo = texelFetch(in_albedo, coord, 0).rgb;
	float depth = texelFetch(in_depth, coord, 0).r;

	mat4 inv_viewproj = inverse(frame.camera.view_proj);

	vec3 worldSpacePos = unproject(in_clip_position.xy, depth, inv_viewproj);
	float dist = distance(worldSpacePos, lightPos);
	const vec3 to_light = (lightPos - worldSpacePos);
	const vec3 light_vec = to_light / dist;

	const float NoL = clamp(dot(light_vec, normal), 0.0, 1.0);
	const float att = attenuation(dist, lightRadius, frame.falloff);

	const vec4 tmp = frame.camera.view_proj * vec4(worldSpacePos, 1.0);
	const vec3 tmp3 = tmp.xyz / tmp.w;

	out_color = vec4(att * NoL * lightColor * albedo, 1.0);
}
