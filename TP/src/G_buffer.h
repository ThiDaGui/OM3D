#ifndef GBUFF
#define GBUFF

#include <Framebuffer.h>

struct G_buffer {
    static G_buffer create(glm::uvec2 size)
    {
        G_buffer buffer;
        Texture depth_texture = Texture(size, ImageFormat::Depth32_FLOAT);
        Texture albedo_texture = Texture(size, ImageFormat::RGBA8_sRGB);
        Texture normal_texture = Texture(size, ImageFormat::RGBA8_UNORM);

        buffer.fbuffer = Framebuffer(&depth_texture, 
                         std::array{&albedo_texture, &normal_texture}, 2);
    }
    Framebuffer fbuffer;
}

#endif
