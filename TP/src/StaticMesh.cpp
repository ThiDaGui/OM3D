#include "StaticMesh.h"

#include <glad/glad.h>
#include <iostream>
#include <memory>

#include "BoundingSpere.h"
#include "BoundingVolume.h"
#include "glm/ext/vector_float3.hpp"

namespace OM3D {

extern bool audit_bindings_before_draw;

StaticMesh::StaticMesh(const MeshData &data)
    : _vertex_buffer(data.vertices)
    , _index_buffer(data.indices) {
    glm::vec3 sum = glm::vec3(0.0);
    for (auto i : data.vertices) {
        sum += i.position;
    }
    glm::vec3 bounding_sphere_center = sum / (float)data.vertices.size();
    float max_dist = 0.0;
    for (auto i : data.vertices) {
        float n_dist = glm::distance(i.position, bounding_sphere_center);
        max_dist = std::max(n_dist, max_dist);
    }
    float bounding_sphere_radius = max_dist;
    _bounding_volume = std::make_shared<BoundingSphere>(bounding_sphere_center,
                                                        bounding_sphere_radius);
}

void StaticMesh::draw() const {
    _vertex_buffer.bind(BufferUsage::Attribute);
    _index_buffer.bind(BufferUsage::Index);

    // Vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), nullptr);
    // Vertex normal
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex),
                          reinterpret_cast<void *>(3 * sizeof(float)));
    // Vertex uv
    glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex),
                          reinterpret_cast<void *>(6 * sizeof(float)));
    // Tangent / bitangent sign
    glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(Vertex),
                          reinterpret_cast<void *>(8 * sizeof(float)));
    // Vertex color
    glVertexAttribPointer(4, 3, GL_FLOAT, false, sizeof(Vertex),
                          reinterpret_cast<void *>(12 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    if (audit_bindings_before_draw) {
        audit_bindings();
    }

    glDrawElements(GL_TRIANGLES, int(_index_buffer.element_count()),
                   GL_UNSIGNED_INT, nullptr);
}

std::shared_ptr<BoundingVolume> StaticMesh::getBoundingVolume() const {
    return _bounding_volume;
}

} // namespace OM3D
