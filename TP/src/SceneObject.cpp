#include "SceneObject.h"

#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "BoundingVolume.h"
#include "Material.h"

namespace OM3D {

SceneObject::SceneObject(std::shared_ptr<StaticMesh> mesh,
                         std::shared_ptr<Material> material)
    : _mesh(std::move(mesh))
    , _material(std::move(material)) {
}

void SceneObject::render() const {
    if (!_material || !_mesh) {
        return;
    }

    _material->set_uniform(HASH("model"), transform());
    _material->bind();
    _mesh->draw();
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
