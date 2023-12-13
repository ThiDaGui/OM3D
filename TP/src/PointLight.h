#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <SceneObject.h>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <glm/vec3.hpp>
#include <math.h>
#include <memory>
#include <string>
#include <utility>
#include <utils.h>
#include <vector>

#include "LoadSphere.h"
#include "Material.h"
#include "StaticMesh.h"
#include "Vertex.h"
#include "glm/geometric.hpp"

namespace OM3D {

class PointLight : public SceneObject {
public:
    PointLight();

    void set_position(const glm::vec3 &pos);

    void set_color(const glm::vec3 &color);

    void set_radius(float radius);

    const glm::vec3 &position() const;

    const glm::vec3 &color() const;

    float radius() const;

private:
    glm::vec3 _position = {};
    glm::vec3 _color = glm::vec3(1.0f);
    float _radius = 10.0f;
};

} // namespace OM3D

#endif // POINTLIGHT_H
