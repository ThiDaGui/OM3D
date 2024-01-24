#include "Scene.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <glad/glad.h>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Material.h"
#include "PointLight.h"
#include "Program.h"
#include "SceneObject.h"
#include "SceneObjectInstance.h"
#include "StaticMesh.h"
#include "Texture.h"
#include "TypedBuffer.h"
#include "shader_structs.h"

namespace OM3D {

Scene::Scene()
    : _frame_data_buffer(nullptr, size_t(1))
    , _light_buffer(nullptr, size_t(1)) {
}

void Scene::add_object(SceneObject obj) {
    _objects.emplace_back(std::move(obj));
}

void Scene::add_light(PointLight obj) {
    _point_lights.emplace_back(std::move(obj));

    _light_buffer = TypedBuffer<shader::PointLight>(
        nullptr, std::max(_point_lights.size(), size_t(1)));

    {
        auto mapping = _light_buffer.map(AccessType::WriteOnly);
        for (size_t i = 0; i != _point_lights.size(); ++i) {
            const auto &light = _point_lights[i];
            mapping[i] = { light.position(), light.radius(), light.color(),
                           0.0f };
        }
    }

    _light_buffer.bind(BufferUsage::Storage, 1);
}

Span<const SceneObject> Scene::objects() const {
    return _objects;
}

Span<const PointLight> Scene::point_lights() const {
    return _point_lights;
}

Camera &Scene::camera() {
    return _camera;
}

const Camera &Scene::camera() const {
    return _camera;
}

void Scene::set_sun(glm::vec3 direction, glm::vec3 color) {
    _sun_direction = direction;
    _sun_color = color;
}

float Scene::get_falloff() const {
    return _falloff;
}

void Scene::set_falloff(const float falloff) {
    _falloff = falloff;
}

void Scene::update() {
    _frame_data_buffer = TypedBuffer<shader::FrameData>(nullptr, 1);

    {
        auto mapping = _frame_data_buffer.map(AccessType::WriteOnly);
        mapping[0].camera.view_proj = _camera.view_proj_matrix();
        mapping[0].camera.proj = _camera.projection_matrix();
        mapping[0].camera.view = _camera.view_matrix();
        mapping[0].point_light_count = u32(_point_lights.size());
        mapping[0].sun_color = _sun_color;
        mapping[0].sun_dir = glm::normalize(_sun_direction);
        mapping[0].falloff = _falloff;
    }

    _frame_data_buffer.bind(BufferUsage::Uniform, 0);
}

void Scene::render() const {
    const Frustum frustum = _camera.build_frustum();

    for (const SceneObject &obj : _objects) {
        if (obj.ObjInFrustrum(frustum, _camera.position())) {
            obj.render();
        }
    }
}

void Scene::deferred(std::shared_ptr<Program> deffered_sun_program) const {
    deffered_sun_program->bind();
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glEnable(GL_DEPTH_TEST);

    for (const auto &light : _point_lights) {
        light.render();
    }
    glDepthMask(GL_TRUE);
}

} // namespace OM3D
