#ifndef MATERIAL_H
#define MATERIAL_H

#include <Program.h>
#include <Texture.h>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <vector>

namespace OM3D
{

enum class BlendMode
{
    None,
    Alpha,
    Additive,
};
enum class AlphaMode
{
    Opaque,
    Mask,
    Blend
};

enum class DepthTestMode
{
    Standard,
    Reversed,
    Equal,
    None
};

enum class CullMode
{
    Back,
    Front,
    None,
};

class Material
{
public:
    Material();

    void set_program(std::shared_ptr<Program> prog);
    void set_blend_mode(BlendMode blend);
    void set_depth_test_mode(DepthTestMode depth);
    void set_cull_mode(CullMode cull);
    void set_alpha_mode(std::string mode);
    void set_texture(u32 slot, std::shared_ptr<Texture> tex);

    template <typename... Args>
    void set_uniform(Args &&...args)
    {
        _program->set_uniform(FWD(args)...);
    }

    void bind() const;

    static std::shared_ptr<Material> empty_material();

    static Material colored_material();
    static Material textured_material();
    static Material textured_normal_mapped_material();

    static Material textured_transparent_material();
    static Material textured_normal_mapped_transparent_material();

    static Material textured_masked_material(float alpha_cutoff = 0.5);
    static Material
    textured_normal_mapped_masked_material(float alpha_cutoff = 0.5);
    // PointLight
    static Material point_light_material();

    BlendMode blend_mode() const
    {
        return _blend_mode;
    }
    DepthTestMode depth_test_mode() const
    {
        return _depth_test_mode;
    }
    CullMode cull_mode() const
    {
        return _cull_mode;
    }
    AlphaMode alpha_mode() const
    {
        return _alpha_mode;
    }

    void set_base_color(glm::vec4 base_color_)
    {
        _BaseColor = base_color_;
    }
    glm::vec4 base_color() const
    {
        return _BaseColor;
    }

private:
    std::shared_ptr<Program> _program;
    std::vector<std::pair<u32, std::shared_ptr<Texture>>> _textures;
    glm::vec4 _BaseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    bool use_base_color_as_texture = false;

    float alpha_cutoff = 0.5;

    BlendMode _blend_mode = BlendMode::None;
    DepthTestMode _depth_test_mode = DepthTestMode::Standard;
    CullMode _cull_mode = CullMode::Back;
    AlphaMode _alpha_mode = AlphaMode::Opaque;
};

} // namespace OM3D

#endif // MATERIAL_H
