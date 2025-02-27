#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <Material.h>
#include <StaticMesh.h>
#include <cstdint>
#include <glm/matrix.hpp>
#include <memory>

#include "BoundingSpere.h"
#include "BoundingVolume.h"
#include "Camera.h"

namespace OM3D {

class SceneObjectInstance;

class SceneObject {
    friend SceneObjectInstance;

public:
    SceneObject(std::shared_ptr<StaticMesh> mesh = nullptr,
                std::shared_ptr<Material> material = nullptr);

    void render() const;

    void set_transform(const glm::mat4 &tr);
    const glm::mat4 &transform() const;
    bool ObjInFrustrum(Frustum frustum, glm::vec3 cam_pos) const;

    std::uintptr_t getMaterialAddr() const;

private:
    glm::mat4 _transform = glm::mat4(1.0f);

    std::shared_ptr<StaticMesh> _mesh;
    std::shared_ptr<Material> _material;
};

} // namespace OM3D

#endif // SCENEOBJECT_H
