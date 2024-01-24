#include "Scene.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <glad/glad.h>
#include <memory>
#include <unordered_map>
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

#include <iostream>

namespace OM3D {

Scene::Scene()
    : _frame_data_buffer(nullptr, size_t(1))
    , _light_buffer(nullptr, size_t(1)) {
}

void Scene::add_object(SceneObject obj) {
    _objects.emplace_back(std::move(obj));
}

void Scene::add_light(PointLight obj) {
    _point_lights.emplace_back(std::move(obj));

    _light_buffer = TypedBuffer<shader::PointLight>(
        nullptr, std::max(_point_lights.size(), size_t(1)));

    {
        auto mapping = _light_buffer.map(AccessType::WriteOnly);
        for (size_t i = 0; i != _point_lights.size(); ++i) {
            const auto &light = _point_lights[i];
            mapping[i] = { light.position(), light.radius(), light.color(),
                           0.0f };
        }
    }

    _light_buffer.bind(BufferUsage::Storage, 1);
}

Span<const SceneObject> Scene::objects() const {
    return _objects;
}

Span<const PointLight> Scene::point_lights() const {
    return _point_lights;
}

Camera &Scene::camera() {
    return _camera;
}

const Camera &Scene::camera() const {
    return _camera;
}

void Scene::set_sun(glm::vec3 direction, glm::vec3 color) {
    _sun_direction = direction;
    _sun_color = color;


    /*
    float near_plane = 0.0f, far_plane = 1000.0f;
    glm::mat4 lightProjection = glm::ortho(
            -500.0f, 500.0f, -500.0f, 500.0f,
            near_plane, far_plane);

    glm::vec3 sun_pos = -100.f * direction;
    glm::vec3 sun_target = glm::vec3(0.0);//sun_pos + _sun_direction;
    glm::mat4 lightViewMatrix = glm::lookAt(sun_pos, sun_target, glm::vec3(0.0f, 1.0f, 0.0f));*/
    glm::mat4 lightProjection = Camera::perspective(to_rad(60.0f), 16.0f / 9.0f, 0.001f);
    glm::mat4 lightViewMatrix = 
        glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    _sunLightSpaceMatrix = lightProjection * lightViewMatrix;
    _sunLightSpaceMatrix = _camera.view_proj_matrix();
    std::cout << "sun " << _sunLightSpaceMatrix[0][0] << "\n";
}

void Scene::update() {
    _frame_data_buffer = TypedBuffer<shader::FrameData>(nullptr, 1);

    {
        auto mapping = _frame_data_buffer.map(AccessType::WriteOnly);
        mapping[0].camera.view_proj = _camera.view_proj_matrix();
        mapping[0].point_light_count = u32(_point_lights.size());
        mapping[0].sun_color = _sun_color;
        mapping[0].sun_dir = glm::normalize(_sun_direction);
        mapping[0].light_proj.view_proj = get_sunLightSpaceMatrix();
    }

    _frame_data_buffer.bind(BufferUsage::Uniform, 0);
}

void Scene::render() const {
    const Frustum frustum = _camera.build_frustum();

    // TODO: Mode the creation of this map outside of render
    std::unordered_map<std::uintptr_t, SceneObjectInstance> instances;

    for (const SceneObject &obj : _objects) {
        if (obj.ObjInFrustrum(frustum, _camera.position())) {
            instances[obj.getMaterialAddr()].push_back(obj);
        }
    }

    // Render every object
    for (auto &pair : instances) {
        pair.second.init();
        pair.second.render();
    }
}

void Scene::shadow_mapping(std::shared_ptr<Program> shadow_map_program) const {
    /*
    TypedBuffer<shader::FrameData> buffer(nullptr, 1);
    {
        auto mapping = buffer.map(AccessType::WriteOnly);
        mapping[0].camera.view_proj = _camera.view_proj_matrix();
        mapping[0].point_light_count = u32(_point_lights.size());
        mapping[0].sun_color = _sun_color;
        mapping[0].sun_dir = glm::normalize(_sun_direction);
    }
    buffer.bind(BufferUsage::Uniform, 0);

    // Fill and bind lights buffer
    TypedBuffer<shader::PointLight> light_buffer(nullptr, std::max(_point_lights.size(), size_t(1)));
    {
        auto mapping = light_buffer.map(AccessType::WriteOnly);
        for(size_t i = 0; i != _point_lights.size(); ++i) {
            const auto& light = _point_lights[i];
            mapping[i] = {
                light.position(),
                light.radius(),
                light.color(),
                0.0f
            };
        }
    }
    light_buffer.bind(BufferUsage::Storage, 1);*/
    //shadow_map_program->bind();
    //glDrawBuffer(GL_NONE);
    //glReadBuffer(GL_NONE);
    //Sun
    //shadow_map_program->set_uniform(HASH("lightSpaceMatrix"), _sunLightSpaceMatrix);
    for (const SceneObject &obj : _objects)
    {
        std::cout << get_sunLightSpaceMatrix()[3][3] << " ??? \n";
        shadow_map_program->set_uniform(HASH("lightSpaceMatrix"), get_sunLightSpaceMatrix());
        obj.draw_mesh_shadow(shadow_map_program);
    }
}

void Scene::deferred(std::shared_ptr<Program> deffered_sun_program) const {
    deffered_sun_program->bind();
    deffered_sun_program->set_uniform(HASH("lightSpaceMatrix"), get_sunLightSpaceMatrix());
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glEnable(GL_DEPTH_TEST);

    for (const auto &light : _point_lights) {
        light.render();
    }
    glDepthMask(GL_TRUE);
}

const glm::mat4 &Scene::get_sunLightSpaceMatrix() const{
    return _sunLightSpaceMatrix;
}

} // namespace OM3D
