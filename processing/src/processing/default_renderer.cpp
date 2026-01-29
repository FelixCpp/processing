#include <processing/default_renderer.hpp>

namespace processing
{
    inline static constexpr GLenum convertBlendFactor(const BlendMode::Factor factor)
    {
        switch (factor)
        {
            case BlendMode::Factor::zero:
                return GL_ZERO;
            case BlendMode::Factor::one:
                return GL_ONE;
            case BlendMode::Factor::srcColor:
                return GL_SRC_COLOR;
            case BlendMode::Factor::oneMinusSrcColor:
                return GL_ONE_MINUS_SRC_COLOR;
            case BlendMode::Factor::dstColor:
                return GL_DST_COLOR;
            case BlendMode::Factor::oneMinusDstColor:
                return GL_ONE_MINUS_DST_COLOR;
            case BlendMode::Factor::srcAlpha:
                return GL_SRC_ALPHA;
            case BlendMode::Factor::oneMinusSrcAlpha:
                return GL_ONE_MINUS_SRC_ALPHA;
            case BlendMode::Factor::dstAlpha:
                return GL_DST_ALPHA;
            case BlendMode::Factor::oneMinusDstAlpha:
                return GL_ONE_MINUS_DST_ALPHA;
        }
    }

    inline static constexpr GLenum convertEquation(const BlendMode::Equation equation)
    {
        switch (equation)
        {
            case BlendMode::Equation::add:
                return GL_FUNC_ADD;
            case BlendMode::Equation::subtract:
                return GL_FUNC_SUBTRACT;
            case BlendMode::Equation::reverseSubtract:
                return GL_FUNC_REVERSE_SUBTRACT;
            case BlendMode::Equation::min:
                return GL_MIN;
            case BlendMode::Equation::max:
                return GL_MAX;
        }
    }
} // namespace processing

namespace processing
{
    inline static constexpr size_t MAX_VERTICES = 10'000;
    inline static constexpr size_t MAX_INDICES = 30'000;

    inline static constexpr const char* VERTEX_SHADER = R"(
        #version 330 core

        layout (location = 0) in vec3 a_Position;
        layout (location = 1) in vec2 a_TexCoord;
        layout (location = 2) in vec4 a_Color;

        out vec2 v_TexCoord;
        out vec4 v_Color;

        uniform mat4 u_ProjectionMatrix;

        void main()
        {
            gl_Position = u_ProjectionMatrix * vec4(a_Position, 1.0);
            v_TexCoord = a_TexCoord;
            v_Color = a_Color;
        }
    )";

    inline static constexpr const char* FRAGMENT_SHADER = R"(
        #version 330 core

        layout (location = 0) out vec4 o_Color;

        in vec2 v_TexCoord;
        in vec4 v_Color;

        uniform sampler2D u_TextureSampler;

        void main()
        {
            o_Color = texture(u_TextureSampler, v_TexCoord) * v_Color;
        }
    )";

    std::unique_ptr<DefaultRenderer> DefaultRenderer::create(ShaderAssetManager& shaderHandleManager, TextureAssetManager& textureAssetManager)
    {
        GLuint vertexArrayId = 0;
        glGenVertexArrays(1, &vertexArrayId);
        glBindVertexArray(vertexArrayId);

        GLuint vertexBufferId = 0;
        glGenBuffers(1, &vertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

        GLuint elementBufferId = 0;
        glGenBuffers(1, &elementBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_INDICES * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

        glBindVertexArray(0);

        uint8_t pixel[] = {255, 255, 255, 255};
        Shader defaultShaderProgram = shaderHandleManager.loadShader(VERTEX_SHADER, FRAGMENT_SHADER);
        Texture whiteTexture = textureAssetManager.create(1, 1, pixel);

        return std::unique_ptr<DefaultRenderer>(new DefaultRenderer(vertexArrayId, vertexBufferId, elementBufferId, defaultShaderProgram, whiteTexture));
    }

    DefaultRenderer::~DefaultRenderer()
    {
        glDeleteVertexArrays(1, &m_vertexArrayId);
        glDeleteBuffers(1, &m_vertexBufferId);
        glDeleteBuffers(1, &m_elementBufferId);
    }

    void DefaultRenderer::activate(const RenderingDetails& renderingDetails)
    {
        flush();

        glViewport(renderingDetails.renderbufferViewport.left, renderingDetails.renderbufferViewport.top, renderingDetails.renderbufferViewport.width, renderingDetails.renderbufferViewport.height);
        glBindFramebuffer(GL_FRAMEBUFFER, renderingDetails.renderbufferResourceId.value);
        glClear(GL_DEPTH_BUFFER_BIT);

        m_projectionMatrix = renderingDetails.projectionMatrix;
    }

    void DefaultRenderer::beginDraw(const RenderingDetails& renderingDetails)
    {
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_MULTISAMPLE);

        glDepthFunc(GL_LEQUAL);

        activate(renderingDetails);
    }

    void DefaultRenderer::endDraw()
    {
        flush();
    }

    void DefaultRenderer::submit(const RenderingSubmission& submission)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, submission.textureResourceId.value_or(m_whiteTexture.getResourceId()).value);

        ResourceId shaderResourceId = submission.shaderResourceId.value_or(m_defaultShaderProgram.getResourceId());
        glUseProgram(shaderResourceId.value);
        glProgramUniformMatrix4fv(shaderResourceId.value, glGetUniformLocation(shaderResourceId.value, "u_ProjectionMatrix"), 1, GL_FALSE, m_projectionMatrix.data.data());
        glProgramUniform1i(shaderResourceId.value, glGetUniformLocation(shaderResourceId.value, "u_TextureSampler"), 0);

        activate(submission.blendMode);

        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
        glBufferSubData(GL_ARRAY_BUFFER, 0, submission.vertices.size() * sizeof(Vertex), submission.vertices.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferId);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, submission.indices.size() * sizeof(uint32_t), submission.indices.data());

        glBindVertexArray(m_vertexArrayId);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(submission.indices.size()), GL_UNSIGNED_INT, nullptr);
    }

    void DefaultRenderer::flush()
    {
    }

    void DefaultRenderer::activate(const BlendMode& blendMode)
    {
        glBlendFuncSeparate(convertBlendFactor(blendMode.colorSrcFactor), convertBlendFactor(blendMode.colorDstFactor), convertBlendFactor(blendMode.alphaSrcFactor), convertBlendFactor(blendMode.alphaDstFactor));
        glBlendEquationSeparate(convertEquation(blendMode.colorEquation), convertEquation(blendMode.alphaEquation));
    }

    DefaultRenderer::DefaultRenderer(GLuint vertexArrayId, GLuint vertexBufferId, GLuint elementBufferId, Shader shaderProgram, Texture whiteTexture)
        : m_vertexArrayId(vertexArrayId),
          m_vertexBufferId(vertexBufferId),
          m_elementBufferId(elementBufferId),
          m_defaultShaderProgram(shaderProgram),
          m_whiteTexture(whiteTexture),
          m_projectionMatrix(matrix4x4_identity())
    {
    }
} // namespace processing
