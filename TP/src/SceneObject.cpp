#include "SceneObject.h"

#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "BoundingVolume.h"
#include "Material.h"
#include "Scene.h"
namespace OM3D {

SceneObject::SceneObject(std::shared_ptr<StaticMesh> mesh,
                         std::shared_ptr<Material> material)
    : _mesh(std::move(mesh))
    , _material(std::move(material)) {
}

SceneObject::SceneObject(std::string path) {
    auto object_scene = std::make_unique<Scene>();
    auto result = Scene::from_gltf(path);
    ALWAYS_ASSERT(result.is_ok, "Unable to load sphere.glb");
    object_scene = std::move(result.value);
    auto objects = object_scene->objects();

    ALWAYS_ASSERT(objects.size() == 1, "object scene should have one object");
    *this = objects[0];
}

void SceneObject::render() const {
    if (!_material || !_mesh) {
        return;
    }

    _material->set_uniform(HASH("model"), transform());
    _material->bind();
    _mesh->draw();
}

void SceneObject::draw_mesh_shadow(std::shared_ptr<Program> shadow_map_program) const {
    shadow_map_program->set_uniform(HASH("model"), transform());
    _material->bind();
    shadow_map_program->bind();
    _mesh->draw_shadow();
}

bool IsInPlane(glm::vec3 normal, glm::vec3 pos, float ray) {
    return (glm::dot(normal, pos) + ray) > 0;
}
bool SceneObject::ObjInFrustrum(const Frustum frustum,
                                glm::vec3 cam_pos) const {
    std::shared_ptr<BoundingVolume> volume =
        _mesh->getBoundingVolume()->transform(_transform);

    bool ret = volume->testPlane(frustum._near_normal, cam_pos)
        && volume->testPlane(frustum._left_normal, cam_pos)
        && volume->testPlane(frustum._right_normal, cam_pos)
        && volume->testPlane(frustum._top_normal, cam_pos)
        && volume->testPlane(frustum._bottom_normal, cam_pos);

    return ret;
}

void SceneObject::set_transform(const glm::mat4 &tr) {
    _transform = tr;
}

const glm::mat4 &SceneObject::transform() const {
    return _transform;
}

std::uintptr_t SceneObject::getMaterialAddr() const {
    return reinterpret_cast<std::uintptr_t>(_material.get());
}

} // namespace OM3D
