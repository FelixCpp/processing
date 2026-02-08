#ifndef _PROCESSING_INCLUDE_RENDERER_HPP_
#define _PROCESSING_INCLUDE_RENDERER_HPP_

#include <processing/processing.hpp>

namespace processing
{
    struct RenderState
    {
        BlendMode blendMode;
        // Renderbuffer renderbuffer;
        std::optional<Shader> shader;
        std::optional<Image> image;
        matrix4x4 transform;
    };

    class DefaultRenderer
    {
    public:
        static std::unique_ptr<DefaultRenderer> create();

        void beginDraw(const Renderbuffer& buffer);
        void endDraw();

        void render(const Vertices& vertices, const RenderState& state);

    private:
        explicit DefaultRenderer(ResourceId vertexArrayId, ResourceId vertexBufferId, ResourceId elementBufferId, Image whiteImage, Shader defaultShader);

        ResourceId m_vertexArrayId;
        ResourceId m_vertexBufferId;
        ResourceId m_elementBufferId;

        Image m_whiteImage;
        Shader m_defaultShader;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_RENDERER_HPP_
