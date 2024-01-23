#include "Scene.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <glad/glad.h>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Material.h"
#include "PointLight.h"
#include "Program.h"
#include "SceneObject.h"
#include "SceneObjectInstance.h"
#include "StaticMesh.h"
#include "Texture.h"
#include "TypedBuffer.h"
#include "shader_structs.h"

namespace OM3D
{

    Scene::Scene()
        : _frame_data_buffer(nullptr, size_t(1)), _light_buffer(nullptr, size_t(1))
    {
    }

    void Scene::add_object(SceneObject obj)
    {
        _objects.emplace_back(std::move(obj));
    }

    void Scene::add_light(PointLight obj)
    {
        _point_lights.emplace_back(std::move(obj));

        _light_buffer = TypedBuffer<shader::PointLight>(
            nullptr, std::max(_point_lights.size(), size_t(1)));

        {
            auto mapping = _light_buffer.map(AccessType::WriteOnly);
            for (size_t i = 0; i != _point_lights.size(); ++i)
            {
                const auto &light = _point_lights[i];
                mapping[i] = {light.position(), light.radius(), light.color(),
                              0.0f};
            }
        }

        _light_buffer.bind(BufferUsage::Storage, 1);
    }

    Span<const SceneObject> Scene::objects() const
    {
        return _objects;
    }

    Span<const PointLight> Scene::point_lights() const
    {
        return _point_lights;
    }

    Camera &Scene::camera()
    {
        return _camera;
    }

    const Camera &Scene::camera() const
    {
        return _camera;
    }

    void Scene::set_sun(glm::vec3 direction, glm::vec3 color)
    {
        _sun_direction = direction;
        _sun_color = color;
    }

    float Scene::get_falloff() const
    {
        return _falloff;
    }

    void Scene::set_falloff(const float falloff)
    {
        _falloff = falloff;
    }

    void Scene::update()
    {
        _frame_data_buffer = TypedBuffer<shader::FrameData>(nullptr, 1);

        {
            auto mapping = _frame_data_buffer.map(AccessType::WriteOnly);
            mapping[0].camera.view_proj = _camera.view_proj_matrix();
            mapping[0].point_light_count = u32(_point_lights.size());
            mapping[0].sun_color = _sun_color;
            mapping[0].sun_dir = glm::normalize(_sun_direction);
            mapping[0].falloff = _falloff;
        }

        _frame_data_buffer.bind(BufferUsage::Uniform, 0);
    }

    void Scene::render() const
    {
        const Frustum frustum = _camera.build_frustum();
        // TODO: Mode the creation of this map outside of render
        std::unordered_map<std::uintptr_t,
                           std::unordered_map<std::uintptr_t, SceneObjectInstance>>
            instances;
        // sort objects by distance to camera for transparency*

        for (const SceneObject &obj : _objects)
        {
            if (obj.ObjInFrustrum(frustum, _camera.position()) && !obj.is_transparent())
            {
                instances[obj.getMaterialAddr()][obj.getMeshAddr()].push_back(obj);
            }
        }

        // Render every object
        for (auto &pair_ : instances)
        {
            for (auto &pair : pair_.second)
            {
                pair.second.init();
                pair.second.render();
            }
        }
    }

    void Scene::render_transparent() const
    {
        const Frustum frustum = _camera.build_frustum();

        std::vector<SceneObject> transparent_objects_sorted;

        for (const SceneObject &obj : _objects)
        {
            if (obj.is_transparent())
            {
                transparent_objects_sorted.push_back(obj);
            }
        }

        // sort transparent objects by distance to camera
        auto comp = [&](const SceneObject &a, const SceneObject &b)
        {
            glm::vec3 a_pos = a.transform()[3];
            glm::vec3 b_pos = b.transform()[3];
            return glm::length(camera().position() - a_pos) > glm::length(camera().position() - b_pos);
        };
        std::sort(transparent_objects_sorted.begin(), transparent_objects_sorted.end(), comp);

        //instance transparent objects
        std::unordered_map<std::uintptr_t, std::unordered_map<std::uintptr_t, SceneObjectInstance>> instances;
        
        for (std::reverse_iterator<std::vector<SceneObject>::iterator> it = transparent_objects_sorted.rbegin(); it != transparent_objects_sorted.rend(); ++it)
        {
            if (it->ObjInFrustrum(frustum, _camera.position()))
            {
                instances[it->getMaterialAddr()][it->getMeshAddr()].push_back(*it);
            }
        }
        for(auto &pair_ : instances)
        {
            for(auto &pair : pair_.second)
            {
                pair.second.init();
                pair.second.render();
            }
        }

    }

    void Scene::deferred(std::shared_ptr<Program> deffered_sun_program) const
    {
        deffered_sun_program->bind();
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glEnable(GL_DEPTH_TEST);

        for (const auto &light : _point_lights)
        {
            light.render();
        }
        glDepthMask(GL_TRUE);
    }

} // namespace OM3D
