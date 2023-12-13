#include "LoadSphere.h"

#include "Scene.h"
#include "StaticMesh.h"

namespace OM3D {

std::shared_ptr<StaticMesh> load_sphere() {
    auto res = Scene::from_gltf("../../data/sphere.glb");
    ALWAYS_ASSERT(res.is_ok, "Unable to load sphere.glb");
    SceneObject sphere_obj = res.value->objects()[0];
    return sphere_obj._mesh;
}

} // namespace OM3D
