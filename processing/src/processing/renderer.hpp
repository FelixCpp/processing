#ifndef _PROCESSING_INCLUDE_RENDERER_HPP_
#define _PROCESSING_INCLUDE_RENDERER_HPP_

#include <processing/processing.hpp>

#include <vector>
#include <span>
#include <optional>
#include <unordered_map>

#include <glad/gl.h>

namespace processing
{
    struct Vertex
    {
        float3 position;
        float2 texcoord;
        float4 color;
    };

    struct DrawSubmission
    {
        std::span<Vertex> vertices;
        std::span<uint32_t> indices;
        std::optional<GLuint> shaderProgramId;
        std::optional<GLuint> textureId;
    };

    struct BatchKey
    {
        GLuint shaderProgramId;
        GLuint textureId;

        bool operator==(const BatchKey& other) const;
    };

    struct BatchKeyHash
    {
        size_t operator()(const BatchKey& key) const;
    };

    struct Batch
    {
        size_t indexStart;
        size_t indexCount;
    };

    class Renderer
    {
    public:
        static std::unique_ptr<Renderer> create();
        ~Renderer();

        void beginDraw();
        void endDraw();

        void submit(const DrawSubmission& submission);

    private:
        explicit Renderer(GLuint vertexArrayId, GLuint vertexBufferId, GLuint elementBufferId, GLuint defaultShaderProgramId, GLuint whiteTextureId);

        void flush();

        GLuint m_vertexArrayId;
        GLuint m_vertexBufferId;
        GLuint m_elementBufferId;
        GLuint m_defaultShaderProgramId;
        GLuint m_whiteTextureId;

        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;

        std::unordered_map<BatchKey, Batch, BatchKeyHash> m_batches;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_RENDERER_HPP_
