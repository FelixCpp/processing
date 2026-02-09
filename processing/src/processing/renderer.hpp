#ifndef _PROCESSING_INCLUDE_RENDERER_HPP_
#define _PROCESSING_INCLUDE_RENDERER_HPP_

#include <processing/processing.hpp>
#include <processing/framebuffer.hpp>

namespace processing
{
    struct RenderState
    {
        BlendMode blendMode;
        std::optional<Shader> shader;
        std::optional<Image> image;
        matrix4x4 transform;
    };

    // class DefaultRenderer
    // {
    // public:
    //     static std::unique_ptr<DefaultRenderer> create();
    //
    //     void beginDraw(const Framebuffer& buffer);
    //     void endDraw();
    //
    //     void render(const Vertices& vertices, const RenderState& state);
    //
    // private:
    //     explicit DefaultRenderer(ResourceId vertexArrayId, ResourceId vertexBufferId, ResourceId elementBufferId, Image whiteImage, Shader defaultShader);
    //
    //     ResourceId m_vertexArrayId;
    //     ResourceId m_vertexBufferId;
    //     ResourceId m_elementBufferId;
    //
    //     Image m_whiteImage;
    //     Shader m_defaultShader;
    // };

    class BatchRenderer
    {
    public:
        struct BatchState
        {
            VertexMode mode;
            BlendMode blendMode;
            Shader shader;
            Image image;

            bool operator==(const BatchState& other) const;
        };

        struct Batch
        {
            BatchState state;
            u32 vertexOffset;
            u32 indexOffset;
            u32 indexCount;
        };

    public:
        static std::unique_ptr<BatchRenderer> create();

        void beginDraw(const Framebuffer& buffer);
        void endDraw();

        void render(const Vertices& vertices, const RenderState& state);

    private:
        void flush();

        explicit BatchRenderer(ResourceId vertexArrayId, ResourceId vertexBufferId, ResourceId elementBufferId, Image whiteImage, Shader defaultShader);

        ResourceId m_vertexArrayId;
        ResourceId m_vertexBufferId;
        ResourceId m_elementBufferId;

        std::vector<Vertex> m_vertices;
        std::vector<u32> m_indices;
        std::vector<Batch> m_batches;
        Batch* m_currentBatch;

        matrix4x4 m_projectionMatrix;

        Image m_whiteImage;
        Shader m_defaultShader;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_RENDERER_HPP_
