#ifndef STATICMESH_H
#define STATICMESH_H

#include <TypedBuffer.h>
#include <Vertex.h>
#include <graphics.h>
#include <memory>
#include <vector>

#include "BoundingVolume.h"

namespace OM3D {

class SceneObjectInstance;

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<u32> indices;
};

class StaticMesh : NonCopyable {
    friend SceneObjectInstance;

public:
    StaticMesh() = default;
    StaticMesh(StaticMesh &&) = default;
    StaticMesh &operator=(StaticMesh &&) = default;

    StaticMesh(const MeshData &data);

    void draw() const;

    std::shared_ptr<BoundingVolume> getBoundingVolume() const;

private:
    TypedBuffer<Vertex> _vertex_buffer;
    TypedBuffer<u32> _index_buffer;
    std::shared_ptr<BoundingVolume> _bounding_volume;
};

} // namespace OM3D

#endif // STATICMESH_H
