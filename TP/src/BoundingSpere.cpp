#include "BoundingSpere.h"

#include <algorithm>
#include <memory>

#include "BoundingVolume.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/quaternion_float.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/gtx/matrix_decompose.hpp"

namespace OM3D {
BoundingSphere::BoundingSphere(glm::vec3 center, float radius)
    : _center(center)
    , _radius(radius) {
}

bool BoundingSphere::testPlane(glm::vec3 normal) {
    return glm::dot(normal, _center) > -_radius;
}

bool BoundingSphere::testPlane(glm::vec3 normal, glm::vec3 position) {
    return glm::dot(normal, _center) > glm::dot(normal, position) - _radius;
}

std::shared_ptr<BoundingVolume>
BoundingSphere::transform(glm::mat4 transformation) {
    glm::vec3 scale;
    glm::quat orientation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(transformation, scale, orientation, translation, skew,
                   perspective);

    float max_scale = std::max(scale.x, std::max(scale.y, scale.z));

    return std::make_shared<BoundingSphere>(
        glm::vec3(transformation * glm::vec4(_center, 1)), max_scale * _radius);
}

} // namespace OM3D
