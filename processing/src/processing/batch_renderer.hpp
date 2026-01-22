#ifndef _PROCESSING_INCLUDE_BATCH_RENDERER_HPP_
#define _PROCESSING_INCLUDE_BATCH_RENDERER_HPP_

#include <processing/processing.hpp>

#include <vector>
#include <unordered_map>
#include <string>

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

    // class UniformUploader
    // {
    // private:
    //     using UniformBucketIndex = size_t;
    //     using UniformName = std::string;
    //     using UniformLocation = GLint;
    //
    // public:
    //
    //     UniformLocation getUniformLocation(ShaderProgramId shaderProgramId, UniformName)
    //
    // private:
    //     std::vector<std::unordered_map<UniformName, UniformLocation>> m_uniformCache;
    //     std::unordered_map<ShaderProgramId, UniformBucketIndex> m_bucketIndices;
    // };

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

        // UniformUploader m_uniformUploader;

        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;

        std::vector<Batch> m_batches;

        ProjectionDetails m_projectionDetails;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_BATCH_RENDERER_HPP_
