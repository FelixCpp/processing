#ifndef _PROCESSING_INCLUDE_BATCH_RENDERER_HPP_
#define _PROCESSING_INCLUDE_BATCH_RENDERER_HPP_

#include <processing/processing.hpp>

#include <vector>

#include <glad/gl.h>

namespace processing
{
    struct BatchKey
    {
        ShaderProgramId shaderProgramId;
        TextureId textureId;
        BlendMode blendMode;

        bool operator==(const BatchKey& other) const;
    };

    struct BatchKeyHash
    {
        size_t operator()(const BatchKey& key) const;
    };

    struct Batch
    {
        BatchKey key;
        size_t indexStart;
        size_t indexCount;
    };

    class BatchRenderer : public Renderer
    {
    public:
        static std::unique_ptr<Renderer> create();
        ~BatchRenderer();

        void beginDraw(const ProjectionDetails& details) override;
        void endDraw() override;

        void submit(const RenderingSubmission& submission) override;
        void flush() override;

    private:
        explicit BatchRenderer(GLuint vertexArrayId, GLuint vertexBufferId, GLuint elementBufferId, GLuint defaultShaderProgramId, GLuint whiteTextureId);

        void activate(const BlendMode& blendMode);

        GLuint m_vertexArrayId;
        GLuint m_vertexBufferId;
        GLuint m_elementBufferId;
        ShaderProgramId m_defaultShaderProgramId;
        TextureId m_whiteTextureId;

        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;

        std::vector<Batch> m_batches;

        ProjectionDetails m_projectionDetails;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_BATCH_RENDERER_HPP_
