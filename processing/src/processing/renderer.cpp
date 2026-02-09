#include <processing/renderer.hpp>
#include <glad/gl.h>

namespace processing
{
    inline static constexpr usize MAX_VERTICES = 10'000;
    inline static constexpr usize MAX_INDICES = 20'000;
} // namespace processing

namespace processing
{
    inline static constexpr std::string_view VS_SOURCE = R"(
#version 410

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;
layout (location = 2) in vec4 a_Color;

out vec2 v_TexCoord;
out vec4 v_Color;

uniform mat4 u_ProjectionMatrix;

void main() {
    gl_Position = u_ProjectionMatrix * vec4(a_Position.xyz, 1.0);
    v_TexCoord = a_TexCoord;
    v_Color = a_Color;
}
)";

    inline static constexpr std::string_view FS_SOURCE = R"(
#version 410

layout (location = 0) out vec4 o_FragColor;

in vec2 v_TexCoord;
in vec4 v_Color;

uniform sampler2D u_TextureSampler;

void main() {
    o_FragColor = texture(u_TextureSampler, v_TexCoord) * v_Color;
}
)";
} // namespace processing

namespace processing
{
    void activate(const BlendMode mode)
    {
        switch (mode)
        {
            case BlendMode::opaque:
            {
                glDisable(GL_BLEND);
                return;
            }

            case BlendMode::alpha:
            {
                glEnable(GL_BLEND);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                glBlendFuncSeparate(
                    GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, // RGB
                    GL_ONE, GL_ONE_MINUS_SRC_ALPHA        // Alpha
                );
                break;
            }

            case BlendMode::premultiplied:
            {
                glEnable(GL_BLEND);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                glBlendFuncSeparate(
                    GL_ONE, GL_ONE_MINUS_SRC_ALPHA, // RGB
                    GL_ONE, GL_ONE_MINUS_SRC_ALPHA  // Alpha
                );
                break;
            }

            case BlendMode::additive:
            {
                glEnable(GL_BLEND);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                glBlendFuncSeparate(
                    GL_SRC_ALPHA, GL_ONE, // RGB
                    GL_ONE, GL_ONE        // Alpha
                );
                break;
            }

            case BlendMode::multiply:
            {
                glEnable(GL_BLEND);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                glBlendFuncSeparate(
                    GL_DST_COLOR, GL_ZERO, // RGB
                    GL_ONE, GL_ONE_MINUS_SRC_ALPHA
                );
                break;
            }

            case BlendMode::screen:
            {
                glEnable(GL_BLEND);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                glBlendFuncSeparate(
                    GL_ONE, GL_ONE_MINUS_SRC_COLOR, // RGB
                    GL_ONE, GL_ONE_MINUS_SRC_ALPHA
                );
                break;
            }

            case BlendMode::subtract:
            {
                glEnable(GL_BLEND);
                glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_ADD);
                glBlendFuncSeparate(
                    GL_SRC_ALPHA, GL_ONE, // RGB
                    GL_ONE, GL_ONE
                );
                break;
            }
        }
    }
} // namespace processing

namespace processing
{
    inline static constexpr GLenum vertexModeToGlId(const VertexMode mode)
    {
        switch (mode)
        {
                // clang-format off
            case VertexMode::points: return GL_POINTS;
            case VertexMode::lines: return GL_LINES;
            case VertexMode::lineStrip: return GL_LINE_STRIP;
            case VertexMode::lineLoop: return GL_LINE_LOOP;
            case VertexMode::triangles: return GL_TRIANGLES;
            case VertexMode::triangleStrip: return GL_TRIANGLE_STRIP;
            case VertexMode::triangleFan: return GL_TRIANGLE_FAN;
                // clang-format on
        }
    }
} // namespace processing

// namespace processing
// {
//     std::unique_ptr<DefaultRenderer> DefaultRenderer::create()
//     {
//         ResourceId vertexArrayId = {.value = 0};
//         glGenVertexArrays(1, &vertexArrayId.value);
//         glBindVertexArray(vertexArrayId.value);
//
//         ResourceId vertexBufferId = {.value = 0};
//         glGenBuffers(1, &vertexBufferId.value);
//         glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId.value);
//         glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
//         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, position));
//         glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, texcoord));
//         glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, color));
//         glEnableVertexAttribArray(0);
//         glEnableVertexAttribArray(1);
//         glEnableVertexAttribArray(2);
//
//         ResourceId elementBufferId = {.value = 0};
//         glGenBuffers(1, &elementBufferId.value);
//         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId.value);
//         glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_INDICES * sizeof(u32), nullptr, GL_DYNAMIC_DRAW);
//
//         glBindVertexArray(0);
//         glBindBuffer(GL_ARRAY_BUFFER, 0);
//         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//         const u8 pixel[] = {255, 255, 255, 255};
//         Image whiteImage = createImage(1, 1, pixel, FilterMode::linear, ExtendMode::clamp);
//         Shader defaultShader = createShader(VS_SOURCE, FS_SOURCE);
//
//         return std::unique_ptr<DefaultRenderer>(new DefaultRenderer(vertexArrayId, vertexBufferId, elementBufferId, std::move(whiteImage), std::move(defaultShader)));
//     }
//
//     void DefaultRenderer::beginDraw(const Framebuffer& framebuffer)
//     {
//         glViewport(0, 0, framebuffer.getSize().x, framebuffer.getSize().y);
//         glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.getResourceId().value);
//     }
//
//     void DefaultRenderer::endDraw()
//     {
//         // Nothing to do for now.
//     }
//
//     void DefaultRenderer::render(const Vertices& vertices, const RenderState& renderState)
//     {
//         const ResourceId shaderId = std::invoke(
//             [this, &renderState]()
//             {
//                 if (renderState.shader.has_value())
//                 {
//                     return renderState.shader->getResourceId();
//                 }
//
//                 return m_defaultShader.getResourceId();
//             }
//         );
//
//         const ResourceId imageId = std::invoke(
//             [this, &renderState]()
//             {
//                 if (renderState.image.has_value())
//                 {
//                     return renderState.image->getResourceId();
//                 }
//
//                 return m_whiteImage.getResourceId();
//             }
//         );
//
//         activate(renderState.blendMode);
//
//         glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId.value);
//         glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.vertices.size() * sizeof(Vertex), vertices.vertices.data());
//
//         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferId.value);
//         glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, vertices.indices.size() * sizeof(u32), vertices.indices.data());
//
//         glActiveTexture(GL_TEXTURE0);
//         glBindTexture(GL_TEXTURE_2D, imageId.value);
//
//         glUseProgram(shaderId.value);
//         glUniformMatrix4fv(glGetUniformLocation(shaderId.value, "u_ProjectionMatrix"), 1, GL_FALSE, renderState.transform.data.data());
//         glUniform1i(glGetUniformLocation(shaderId.value, "u_TextureSampler"), 0);
//
//         glBindVertexArray(m_vertexArrayId.value);
//         glDrawElements(vertexModeToGlId(vertices.mode), vertices.indices.size(), GL_UNSIGNED_INT, nullptr);
//     }
//
//     DefaultRenderer::DefaultRenderer(ResourceId vertexArrayId, ResourceId vertexBufferId, ResourceId elementBufferId, Image whiteImage, Shader defaultShader)
//         : m_vertexArrayId(vertexArrayId),
//           m_vertexBufferId(vertexBufferId),
//           m_elementBufferId(elementBufferId),
//           m_whiteImage(std::move(whiteImage)),
//           m_defaultShader(std::move(defaultShader))
//     {
//     }
// } // namespace processing

namespace processing
{
    bool BatchRenderer::BatchState::operator==(const BatchState& other) const
    {
        const auto shaderEqual = shader.getResourceId() == other.shader.getResourceId();
        const auto imageEqual = image.getResourceId() == other.image.getResourceId();
        return mode == other.mode and blendMode == other.blendMode and shaderEqual and imageEqual;
    }

    std::unique_ptr<BatchRenderer> BatchRenderer::create()
    {
        ResourceId vertexArrayId = {.value = 0};
        glGenVertexArrays(1, &vertexArrayId.value);
        glBindVertexArray(vertexArrayId.value);

        ResourceId vertexBufferId = {.value = 0};
        glGenBuffers(1, &vertexBufferId.value);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId.value);
        glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, position));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, texcoord));
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, color));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        ResourceId elementBufferId = {.value = 0};
        glGenBuffers(1, &elementBufferId.value);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId.value);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_INDICES * sizeof(u32), nullptr, GL_DYNAMIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        const u8 pixel[] = {255, 255, 255, 255};
        Image whiteImage = createImage(1, 1, pixel, FilterMode::linear, ExtendMode::clamp);
        Shader defaultShader = createShader(VS_SOURCE, FS_SOURCE);

        return std::unique_ptr<BatchRenderer>(new BatchRenderer(vertexArrayId, vertexBufferId, elementBufferId, std::move(whiteImage), std::move(defaultShader)));
    }

    void BatchRenderer::beginDraw(const Framebuffer& framebuffer)
    {
        glViewport(0, 0, framebuffer.getSize().x, framebuffer.getSize().y);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.getResourceId().value);

        m_projectionMatrix = matrix4x4::orthographic(0.0f, 0.0f, static_cast<f32>(framebuffer.getSize().x), static_cast<f32>(framebuffer.getSize().y), -1.0f, 1.0f);
    }

    void BatchRenderer::endDraw()
    {
        flush();
    }

    void BatchRenderer::render(const Vertices& vertices, const RenderState& state)
    {
        const u32 incomingVertices = static_cast<u32>(vertices.vertices.size());
        const u32 incomingIndices = vertices.indices.empty() ? incomingVertices : static_cast<u32>(vertices.indices.size());

        if (m_vertices.size() + incomingVertices > MAX_VERTICES or m_indices.size() + incomingIndices > MAX_INDICES)
        {
            flush();
        }

        BatchState newState = {
            .mode = vertices.mode,
            .blendMode = state.blendMode,
            .shader = state.shader.value_or(m_defaultShader),
            .image = state.image.value_or(m_whiteImage)
        };

        const bool needNewBatch = not m_currentBatch or not(m_currentBatch->state == newState);

        if (needNewBatch)
        {
            Batch batch = {
                .state = newState,
                .vertexOffset = static_cast<u32>(m_vertices.size()),
                .indexOffset = static_cast<u32>(m_indices.size()),
                .indexCount = 0,
            };

            m_batches.push_back(batch);
            m_currentBatch = &m_batches.back();
        }

        // Vertices transformieren & kopieren
        u32 baseVertex = static_cast<uint32_t>(m_vertices.size());
        m_vertices.append_range(vertices.vertices);

        // Indices remappen
        if (!vertices.indices.empty())
        {
            for (uint32_t idx : vertices.indices)
                m_indices.push_back(baseVertex + idx);

            m_currentBatch->indexCount += vertices.indices.size();
        }
        else
        {
            // non-indexed fallback
            for (uint32_t i = 0; i < vertices.vertices.size(); ++i)
                m_indices.push_back(baseVertex + i);

            m_currentBatch->indexCount += vertices.vertices.size();
        }
    }

    void BatchRenderer::flush()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId.value);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size() * sizeof(Vertex), m_vertices.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferId.value);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_indices.size() * sizeof(u32), m_indices.data());

        for (const Batch& batch : m_batches)
        {
            const ResourceId shaderId = batch.state.shader.getResourceId();
            const ResourceId imageId = batch.state.image.getResourceId();

            activate(batch.state.blendMode);

            glUseProgram(shaderId.value);
            glUniformMatrix4fv(glGetUniformLocation(shaderId.value, "u_ProjectionMatrix"), 1, GL_FALSE, m_projectionMatrix.data.data());
            glUniform1i(glGetUniformLocation(shaderId.value, "u_TextureSampler"), 0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, imageId.value);

            glBindVertexArray(m_vertexArrayId.value);
            glDrawElements(vertexModeToGlId(batch.state.mode), batch.indexCount, GL_UNSIGNED_INT, (const GLvoid*)(batch.indexOffset * sizeof(u32)));
        }

        m_vertices.clear();
        m_indices.clear();
        m_batches.clear();
        m_currentBatch = nullptr;
    }

    BatchRenderer::BatchRenderer(ResourceId vertexArrayId, ResourceId vertexBufferId, ResourceId elementBufferId, Image whiteImage, Shader defaultShader)
        : m_vertexArrayId(vertexArrayId),
          m_vertexBufferId(vertexBufferId),
          m_elementBufferId(elementBufferId),
          m_whiteImage(std::move(whiteImage)),
          m_defaultShader(std::move(defaultShader))
    {
    }
} // namespace processing
