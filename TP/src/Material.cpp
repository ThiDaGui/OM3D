#include "Material.h"

#include <algorithm>
#include <glad/glad.h>
#include <memory>

namespace OM3D
{

    Material::Material()
    {
    }

    void Material::set_program(std::shared_ptr<Program> prog)
    {
        _program = std::move(prog);
    }

    void Material::set_blend_mode(BlendMode blend)
    {
        _blend_mode = blend;
    }

    void Material::set_depth_test_mode(DepthTestMode depth)
    {
        _depth_test_mode = depth;
    }

    void Material::set_cull_mode(CullMode cull)
    {
        _cull_mode = cull;
    }

    void Material::set_alpha_mode(std::string mode)
    {
        if (mode == "OPAQUE")
        {
            _alpha_mode = AlphaMode::Opaque;
        }
        else if (mode == "MASK")
        {
            _alpha_mode = AlphaMode::Mask; // should not be happens in our case
        }
        else if (mode == "BLEND")
        {
            _alpha_mode = AlphaMode::Blend;
        }
    }

    void Material::set_texture(u32 slot, std::shared_ptr<Texture> tex)
    {
        if (const auto it =
                std::find_if(_textures.begin(), _textures.end(),
                             [&](const auto &t)
                             { return t.second == tex; });
            it != _textures.end())
        {
            it->second = std::move(tex);
        }
        else
        {
            _textures.emplace_back(slot, std::move(tex));
        }
    }

    void Material::bind() const
    {
        switch (_blend_mode)
        {
        case BlendMode::None:
            glDisable(GL_BLEND);
            break;

        case BlendMode::Alpha:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;

        case BlendMode::Additive:
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        }

        switch (_depth_test_mode)
        {
        case DepthTestMode::None:
            glDisable(GL_DEPTH_TEST);
            break;

        case DepthTestMode::Equal:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_EQUAL);
            break;

        case DepthTestMode::Standard:
            glEnable(GL_DEPTH_TEST);
            // We are using reverse-Z
            glDepthFunc(GL_GEQUAL);
            break;

        case DepthTestMode::Reversed:
            glEnable(GL_DEPTH_TEST);
            // We are using reverse-Z
            glDepthFunc(GL_LEQUAL);
            break;
        }

        switch (_cull_mode)
        {
        case CullMode::Back:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            break;

        case CullMode::Front:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            break;

        case CullMode::None:
            glDisable(GL_CULL_FACE);
            break;
        }

        for (const auto &texture : _textures)
        {
            texture.second->bind(texture.first);
        }
        _program->bind();
    }

    std::shared_ptr<Material> Material::empty_material()
    {
        static std::weak_ptr<Material> weak_material;
        auto material = weak_material.lock();
        if (!material)
        {
            material = std::make_shared<Material>();
            // material->_program = Program::from_files("lit.frag", "basic.vert");
            material->_program =
                Program::from_files("g_buffer.frag", "instanced.vert");
            weak_material = material;
        }
        return material;
    }

    Material Material::textured_material()
    {
        Material material;
        material._program =
            // Program::from_files("lit.frag", "basic.vert", { "TEXTURED" });
            Program::from_files("g_buffer.frag", "instanced.vert", {"TEXTURED"});
        return material;
    }

    Material Material::textured_transparent_material()
    {
        Material material;
        material._program =
        Program::from_files("lit.frag", "instanced.vert", std::array<std::string, 2>{"TEXTURED", "TRANSPARENT"});
        return material;
    }

    Material Material::textured_normal_mapped_transparent_material()
    {
        Material material;
        material._program = Program::from_files("lit.frag", "instanced.vert", std::array<std::string, 3>{"TEXTURED", "NORMAL_MAPPED", "TRANSPARENT"});
        material.set_blend_mode(BlendMode::Alpha);
        return material;
    }

    Material Material::textured_normal_mapped_material()
    {
        Material material;
        material._program = Program::from_files(
            /*"lit.frag", "basic.vert"*/ "g_buffer.frag", "instanced.vert",
            std::array<std::string, 2>{"TEXTURED", "NORMAL_MAPPED"});
        return material;
    }

    Material Material::point_light_material()
    {
        Material material;
        material._program =
            Program::from_files("defered_light.frag", "defered_light.vert");
        material.set_cull_mode(CullMode::Front);
        material.set_depth_test_mode(DepthTestMode::Reversed);
        material.set_blend_mode(BlendMode::Additive);
        return material;
    }

} // namespace OM3D
