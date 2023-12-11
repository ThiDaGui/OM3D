#include "Scene.h"

#include <TypedBuffer.h>
#include <cstdint>
#include <memory>
#include <shader_structs.h>
#include <unordered_map>
#include <vector>

#include "Material.h"
#include "SceneObject.h"
#include "SceneObjectInstance.h"
#include "StaticMesh.h"

namespace OM3D {

Scene::Scene()
    : _frame_data_buffer(nullptr, 1)
    , _light_buffer(nullptr, size_t(1)) {
}

void Scene::add_object(SceneObject obj) {
    _objects.emplace_back(std::move(obj));
}

void Scene::add_light(PointLight obj) {
    _point_lights.emplace_back(std::move(obj));
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

void Scene::update() {
    {
        auto mapping = _frame_data_buffer.map(AccessType::WriteOnly);
        mapping[0].camera.view_proj = _camera.view_proj_matrix();
        mapping[0].point_light_count = u32(_point_lights.size());
        mapping[0].sun_color = _sun_color;
        mapping[0].sun_dir = glm::normalize(_sun_direction);
    }
    {
        auto mapping = _light_buffer.map(AccessType::WriteOnly);
        for (size_t i = 0; i != _point_lights.size(); ++i) {
            const auto &light = _point_lights[i];
            mapping[i] = { light.position(), light.radius(), light.color(),
                           0.0f };
        }
    }
}

void Scene::render() const {
    _frame_data_buffer.bind(BufferUsage::Uniform, 0);

    // Fill and bind lights buffer
    _light_buffer.bind(BufferUsage::Storage, 1);

    const Frustum frustum = _camera.build_frustum();

    // TODO: Mode the creation of this map outside of render
    std::unordered_map<std::uintptr_t, SceneObjectInstance> instances;

    for (const SceneObject &obj : _objects) {
        if (obj.ObjInFrustrum(frustum, _camera.position())) {
            instances[obj.getMaterialAddr()].push_back(obj);
        }
    }

    // Render every object
    for (auto &pair : instances) {
        pair.second.init();
        pair.second.render();
    }
}

} // namespace OM3D
