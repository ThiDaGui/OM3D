#include "PointLight.h"

#include "LoadSphere.h"

namespace OM3D {
PointLight::PointLight()
    : SceneObject(
        load_sphere(),
        std::make_shared<Material>(Material::point_light_material())) {
}

void PointLight::set_position(const glm::vec3 &pos) {
    _position = pos;
}

void PointLight::set_color(const glm::vec3 &color) {
    _color = color;
}

void PointLight::set_radius(float radius) {
    _radius = radius;
}

const glm::vec3 &PointLight::position() const {
    return _position;
}

const glm::vec3 &PointLight::color() const {
    return _color;
}

float PointLight::radius() const {
    return _radius;
}
void PointLight::render() const {
    if (!_material || !_mesh) {
        return;
    }

    _material->set_uniform(HASH("model"), transform());
    _material->set_uniform(HASH("lightRadius"), radius());
    _material->set_uniform(HASH("lightPos"), position());
    _material->set_uniform(HASH("lightColor"), color());
    _material->bind();
    _mesh->draw();
}
} // namespace OM3D
