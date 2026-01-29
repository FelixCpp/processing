#ifndef _PROCESSING_INCLUDE_DEFAULT_RENDERER_HPP_
#define _PROCESSING_INCLUDE_DEFAULT_RENDERER_HPP_

#include <processing/processing.hpp>
#include <processing/shader.hpp>
#include <processing/texture.hpp>

#include <glad/gl.h>

namespace processing
{
    class DefaultRenderer : public Renderer
    {
    public:
        static std::unique_ptr<DefaultRenderer> create(ShaderAssetManager& shaderHandleManager, TextureAssetManager& textureAssetManager);
        ~DefaultRenderer();

        void activate(const RenderingDetails& renderingDetails) override;

        void beginDraw(const RenderingDetails& renderingDetails) override;
        void endDraw() override;

        void submit(const RenderingSubmission& submission) override;
        void flush() override;

    private:
        explicit DefaultRenderer(
            GLuint vertexArrayId,
            GLuint vertexBufferId,
            GLuint elementBufferId,
            Shader shaderProgram,
            Texture whiteTexture
        );

        void activate(const BlendMode& blendMode);

        GLuint m_vertexArrayId;
        GLuint m_vertexBufferId;
        GLuint m_elementBufferId;
        Shader m_defaultShaderProgram;
        Texture m_whiteTexture;
        matrix4x4 m_projectionMatrix;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_DEFAULT_RENDERER_HPP_
