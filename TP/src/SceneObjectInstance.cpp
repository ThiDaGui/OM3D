#include "SceneObjectInstance.h"

#include <glad/glad.h>

#include "SceneObject.h"
#include "TypedBuffer.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "graphics.h"
#include "utils.h"

namespace OM3D {

extern bool audit_bindings_before_draw;

void SceneObjectInstance::push_back(const SceneObject obj) {
    if (_instance._mesh == nullptr)
        _instance = obj;
    _model_matrices.push_back(obj.transform());
}

void SceneObjectInstance::init() {
    Span<glm::mat4> model_matrices(_model_matrices);
    _ssbo =
        TypedBuffer<glm::mat4>(_model_matrices.data(), _model_matrices.size());
}

void SceneObjectInstance::render() const {
    if (!_instance._material || !_instance._mesh)
        return;

    _instance._material->bind();
    _instance._mesh->_vertex_buffer.bind(BufferUsage::Attribute);
    _instance._mesh->_index_buffer.bind(BufferUsage::Attribute);

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

    _ssbo.bind(BufferUsage::Storage, 5);

    if (audit_bindings_before_draw) {
        audit_bindings();
    }

    glDrawElementsInstanced(GL_TRIANGLES,
                            int(_instance._mesh->_index_buffer.element_count()),
                            GL_UNSIGNED_INT, nullptr, _model_matrices.size());
}

} // namespace OM3D
