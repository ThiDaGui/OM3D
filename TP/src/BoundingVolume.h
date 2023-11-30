#ifndef BOUNDINGVOLUME_H
#define BOUNDINGVOLUME_H

#include <memory>

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"

namespace OM3D {

enum BoundingType {
    Sphere,
};

class BoundingVolume {
public:
    BoundingVolume() = default;
    virtual ~BoundingVolume() = default;

    virtual bool testPlane(glm::vec3 normal) = 0;
    virtual bool testPlane(glm::vec3 normal, glm::vec3 position) = 0;

    virtual std::shared_ptr<BoundingVolume>
    transform(glm::mat4 transformation) = 0;
};

} // namespace OM3D

#endif // BOUNDINGVOLUME_H
