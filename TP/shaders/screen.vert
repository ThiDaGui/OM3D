#version 450

// vertex shader for a screen-space pass

layout(location = 0) out vec2 out_uv;

vec2 uvs[] = {
        vec2(0.0, 2.0),
        vec2(0.0, 0.0),
        vec2(2.0, 0.0),
    };

void main() {
    out_uv = uvs[gl_VertexID];
    gl_Position = vec4((uvs[gl_VertexID] * 2.0 - 1.0), 0.0, 1.0);
}
