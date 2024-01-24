struct CameraData {
	mat4 view;
	mat4 proj;
    mat4 view_proj;
};

struct FrameData {
    CameraData camera;

    vec3 sun_dir;
    uint point_light_count;

    vec3 sun_color;
    float falloff;
};

struct PointLight {
    vec3 position;
    float radius;
    vec3 color;
    float padding_1;
};

