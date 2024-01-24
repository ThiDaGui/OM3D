#ifndef SCENE_H
#define SCENE_H

#include <Camera.h>
#include <SceneObject.h>
#include <memory>
#include <vector>

#include "Framebuffer.h"
#include "PointLight.h"
#include "Program.h"
#include "Texture.h"
#include "TypedBuffer.h"
#include "shader_structs.h"

namespace OM3D {

class Scene : NonMovable {
public:
    Scene();

    static Result<std::unique_ptr<Scene>>
    from_gltf(const std::string &file_name);

    void update();
    void render() const;
    void shadow_mapping(std::shared_ptr<Program> shadow_map_program) const;
    void deferred(std::shared_ptr<Program> deffered_sun_program) const;

    void add_object(SceneObject obj);
    void add_light(PointLight obj);

    Span<const SceneObject> objects() const;
    Span<const PointLight> point_lights() const;

    Camera &camera();
    const Camera &camera() const;

    void set_sun(glm::vec3 direction, glm::vec3 color = glm::vec3(1.0f));

    const glm::mat4 &get_sunLightSpaceMatrix() const;

private:
    std::vector<SceneObject> _objects;
    std::vector<PointLight> _point_lights;

    glm::vec3 _sun_direction = glm::vec3(0.2f, 1.0f, 0.1f);
    glm::vec3 _sun_color = glm::vec3(1.0f);
    glm::mat4 _sunLightSpaceMatrix = glm::mat4(1.0f);

    Camera _camera;

    TypedBuffer<shader::FrameData> _frame_data_buffer;
    TypedBuffer<shader::PointLight> _light_buffer;
};

} // namespace OM3D

#endif // SCENE_H
