#include "SceneObject.h"

#include <glm/gtc/matrix_transform.hpp>

namespace OM3D {

SceneObject::SceneObject(std::shared_ptr<StaticMesh> mesh, std::shared_ptr<Material> material) :
    _mesh(std::move(mesh)),
    _material(std::move(material)) {
}

void SceneObject::render() const {
    if(!_material || !_mesh) {
        return;
    }

    _material->set_uniform(HASH("model"), transform());
    _material->bind();
    _mesh->draw();
}

bool IsInPlane(glm::vec3 normal, glm::vec3 pos, float ray)
{
    return (glm::dot(normal, pos) + ray) > 0;
}
bool SceneObject::ObjInFrustrum(const Frustum frustum, glm::vec3 cam_pos) const
{
    glm::vec3 v = _mesh->BoundingSphereCenter - cam_pos;
    bool ret = IsInPlane(frustum._top_normal, v, _mesh->BoundingSphereRay) &&
               IsInPlane(frustum._bottom_normal, v, _mesh->BoundingSphereRay) &&
               IsInPlane(frustum._left_normal, v, _mesh->BoundingSphereRay) &&
               IsInPlane(frustum._right_normal, v, _mesh->BoundingSphereRay) &&
               IsInPlane(frustum._near_normal, v, _mesh->BoundingSphereRay);
    return ret;
}

void SceneObject::set_transform(const glm::mat4& tr) {
    _transform = tr;
}

const glm::mat4& SceneObject::transform() const {
    return _transform;
}

}
