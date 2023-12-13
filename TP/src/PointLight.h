#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <SceneObject.h>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <glm/vec3.hpp>
#include <math.h>
#include <memory>
#include <utility>
#include <utils.h>
#include <vector>

#include "Material.h"
#include "StaticMesh.h"
#include "Vertex.h"
#include "glm/geometric.hpp"

namespace OM3D {

class PointLight : public SceneObject {
public:
    PointLight()
        : SceneObject(light_sphere, light_matrial) {
    }

    void set_position(const glm::vec3 &pos) {
        _position = pos;
    }

    void set_color(const glm::vec3 &color) {
        _color = color;
    }

    void set_radius(float radius) {
        _radius = radius;
    }

    const glm::vec3 &position() const {
        return _position;
    }

    const glm::vec3 &color() const {
        return _color;
    }

    float radius() const {
        return _radius;
    }

    static void generate_statics(const std::size_t h_segments = 64,
                                 const size_t v_segments = 64) {
        const size_t vertex_count = (h_segments + 1) * (v_segments + 1);
        MeshData mesh_data = { std::vector<Vertex>(vertex_count),
                               std::vector<u32>() };

        size_t index = 0;

        for (size_t y_iter = 0; y_iter <= v_segments; y_iter++) {
            const double v = (1.0 / v_segments) * y_iter;
            const double theta = v * M_PI;

            for (size_t x_iter = 0; x_iter <= h_segments; x_iter++) {
                const double u = (1.0 / h_segments) * x_iter;
                Vertex vec = mesh_data.vertices[index++];
                vec.position = { cos(u * 2.0 * M_PI) * sin(theta), cos(theta),
                                 sin(u * 2.0 * M_PI) * sin(theta) };
                vec.normal = glm::normalize(vec.position);
                vec.uv = { u, v };
            }
        }

        for (size_t y = 0; y < v_segments; y++) {
            if (y % 2 == 0) {
                for (size_t x = 0; x <= h_segments; x++) {
                    mesh_data.indices.emplace_back(y * (h_segments + 1) + x);
                    mesh_data.indices.emplace_back((y + 1) * (h_segments + 1)
                                                   + x);
                }
            } else {
                for (int x = h_segments; x >= 0; x--) {
                    mesh_data.indices.emplace_back((y + 1) * (h_segments + 1)
                                                   + x);
                    mesh_data.indices.emplace_back(y * (h_segments + 1) + x);
                }
            }
        }
        light_sphere = std::make_shared<StaticMesh>(mesh_data);
        light_matrial =
            std::make_shared<Material>(Material::point_light_material());
    }

private:
    glm::vec3 _position = {};
    glm::vec3 _color = glm::vec3(1.0f);
    float _radius = 10.0f;

    inline static std::shared_ptr<StaticMesh> light_sphere = nullptr;
    inline static std::shared_ptr<Material> light_matrial = nullptr;
};

} // namespace OM3D

#endif // POINTLIGHT_H
