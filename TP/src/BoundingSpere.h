#ifndef BOUNDINGSPHERE_H
#define BOUNDINGSPHERE_H

#include "BoundingVolume.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"

namespace OM3D {

class BoundingSphere : public BoundingVolume {
private:
    glm::vec3 _center;
    float _radius;

public:
    BoundingSphere(glm::vec3 center, float radius);
    virtual ~BoundingSphere() = default;

    virtual bool testPlane(glm::vec3 normal) override;
    virtual bool testPlane(glm::vec3 normal, glm::vec3 position) override;

    virtual std::shared_ptr<BoundingVolume>
    transform(glm::mat4 transformation) override;
};

} // namespace OM3D

#endif // BOUNDINGSPHERE_H
