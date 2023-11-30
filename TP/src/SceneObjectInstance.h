#ifndef SCENEOBJECTINSTANCE_H
#define SCENEOBJECTINSTANCE_H

#include <memory>
#include <vector>

#include "SceneObject.h"
#include "TypedBuffer.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "utils.h"

namespace OM3D {

class SceneObjectInstance : NonCopyable {
public:
    SceneObjectInstance() = default;
    ~SceneObjectInstance() = default;

    void push_back(const SceneObject obj);

    void init();

    void render() const;

private:
    SceneObject _instance;
    std::vector<glm::mat4> _model_matrices;
    TypedBuffer<glm::mat4> _ssbo;
};

} // namespace OM3D

#endif
