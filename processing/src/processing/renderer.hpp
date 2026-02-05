#ifndef _PROCESSING_INCLUDE_RENDERER_HPP_
#define _PROCESSING_INCLUDE_RENDERER_HPP_

#include <processing/processing.hpp>

namespace processing
{
    class DefaultRenderer : public Renderer
    {
    public:
        static std::unique_ptr<DefaultRenderer> create();

        void render(const Vertices& vertices, const RenderState& state) override;

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
