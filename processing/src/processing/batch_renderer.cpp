#include <processing/batch_renderer.hpp>

namespace processing
{
    bool BatchKey::operator==(const BatchKey& other) const
    {
        return shaderProgramId == other.shaderProgramId and textureId == other.textureId and blendMode == other.blendMode;
    }

    size_t BatchKeyHash::operator()(const BatchKey& key) const
    {
        size_t h1 = std::hash<GLuint>{}(key.shaderProgramId.value);
        size_t h2 = std::hash<GLuint>{}(key.textureId.value);

        size_t h3 = std::hash<int>{}(static_cast<int>(key.blendMode.colorSrcFactor));
        size_t h4 = std::hash<int>{}(static_cast<int>(key.blendMode.colorDstFactor));
        size_t h5 = std::hash<int>{}(static_cast<int>(key.blendMode.colorEquation));
        size_t h6 = std::hash<int>{}(static_cast<int>(key.blendMode.alphaSrcFactor));
        size_t h7 = std::hash<int>{}(static_cast<int>(key.blendMode.alphaDstFactor));
        size_t h8 = std::hash<int>{}(static_cast<int>(key.blendMode.alphaEquation));

        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^
               (h6 << 5) ^ (h7 << 6) ^ (h8 << 7);
    }
} // namespace processing

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
        uniform mat4 u_ViewMatrix;

        void main()
        {
            gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(a_Position, 1.0);
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
            // float alpha = texture(u_TextureSampler, v_TexCoord).a;
            // o_Color = vec4(v_TexCoord.xy, 0.0, alpha);
            o_Color = texture(u_TextureSampler, v_TexCoord) * v_Color;
        }
    )";

    std::unique_ptr<Renderer> BatchRenderer::create()
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

        GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShaderId, 1, &VERTEX_SHADER, nullptr);
        glCompileShader(vertexShaderId);

        GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShaderId, 1, &FRAGMENT_SHADER, nullptr);
        glCompileShader(fragmentShaderId);

        GLuint shaderProgramId = glCreateProgram();
        glAttachShader(shaderProgramId, vertexShaderId);
        glAttachShader(shaderProgramId, fragmentShaderId);
        glLinkProgram(shaderProgramId);
        glValidateProgram(shaderProgramId);
        glDetachShader(shaderProgramId, vertexShaderId);
        glDetachShader(shaderProgramId, fragmentShaderId);
        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);

        GLuint textureId = 0;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        uint8_t pixel[] = {255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

        return std::unique_ptr<Renderer>(new BatchRenderer(vertexArrayId, vertexBufferId, elementBufferId, shaderProgramId, textureId));
    }

    BatchRenderer::~BatchRenderer()
    {
        glDeleteVertexArrays(1, &m_vertexArrayId);
        glDeleteBuffers(1, &m_vertexBufferId);
        glDeleteBuffers(1, &m_elementBufferId);
        glDeleteTextures(1, &m_whiteTextureId.value);
        glDeleteProgram(m_defaultShaderProgramId.value);
    }

    void BatchRenderer::beginDraw(const ProjectionDetails& details)
    {
        m_projectionDetails = details;

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glClear(GL_DEPTH_BUFFER_BIT);
    }

    void BatchRenderer::endDraw()
    {
        flush();
    }

    void BatchRenderer::submit(const RenderingSubmission& submission)
    {
        if (m_vertices.size() + submission.vertices.size() >= MAX_VERTICES or m_indices.size() + submission.indices.size() >= MAX_INDICES)
        {
            flush();
        }

        const BatchKey key{
            .shaderProgramId = submission.shaderProgramId.value_or(m_defaultShaderProgramId),
            .textureId = submission.textureId.value_or(m_whiteTextureId),
            .blendMode = submission.blendMode.value_or(BlendMode::alpha),
        };

        const size_t vertexOffset = m_vertices.size();
        const size_t indexStart = m_indices.size();

        m_vertices.append_range(submission.vertices);

        for (uint32_t index : submission.indices)
        {
            m_indices.push_back(index + static_cast<uint32_t>(vertexOffset));
        }

        if (not m_batches.empty())
        {
            Batch& lastBatch = m_batches.back();
            const bool hasSameKey = lastBatch.key == key;
            const bool isContinuous = lastBatch.indexStart + lastBatch.indexCount == indexStart;

            if (hasSameKey and isContinuous)
            {
                lastBatch.indexCount += submission.indices.size();
                return;
            }
        }

        m_batches.push_back(Batch{
            .key = key,
            .indexStart = indexStart,
            .indexCount = submission.indices.size(),
        });
    }

    void BatchRenderer::flush()
    {
        if (m_batches.empty())
        {
            return;
        }

        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE);
        glDisable(GL_DEPTH_TEST);

        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size() * sizeof(Vertex), m_vertices.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferId);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_indices.size() * sizeof(uint32_t), m_indices.data());

        glBindVertexArray(m_vertexArrayId);

        auto currentShader = ShaderProgramId{.value = 0};
        auto currentTexture = TextureId{.value = 0};
        auto currentBlendMode = BlendMode::alpha;
        bool isFirstRun = true;

        for (const Batch& batch : m_batches)
        {
            const BatchKey& key = batch.key;

            if (isFirstRun or key.shaderProgramId != currentShader)
            {
                currentShader = key.shaderProgramId;
                glUseProgram(currentShader.value);
                glUniformMatrix4fv(glGetUniformLocation(currentShader.value, "u_ProjectionMatrix"), 1, GL_FALSE, m_projectionDetails.projectionMatrix.data.data());
                glUniformMatrix4fv(glGetUniformLocation(currentShader.value, "u_ViewMatrix"), 1, GL_FALSE, m_projectionDetails.viewMatrix.data.data());
                glUniform1i(glGetUniformLocation(currentShader.value, "u_TextureSampler"), 0);
            }

            if (isFirstRun or key.textureId != currentTexture)
            {
                currentTexture = key.textureId;
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, currentTexture.value);
            }

            if (isFirstRun or key.blendMode != currentBlendMode)
            {
                currentBlendMode = key.blendMode;
                activate(currentBlendMode);
            }

            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(batch.indexCount), GL_UNSIGNED_INT, (void*)(batch.indexStart * sizeof(uint32_t)));
            isFirstRun = false;
        }

        m_batches.clear();
        m_vertices.clear();
        m_indices.clear();
    }

    void BatchRenderer::activate(const BlendMode& blendMode)
    {
        glBlendFuncSeparate(convertBlendFactor(blendMode.colorSrcFactor), convertBlendFactor(blendMode.colorDstFactor), convertBlendFactor(blendMode.alphaSrcFactor), convertBlendFactor(blendMode.alphaDstFactor));
        glBlendEquationSeparate(convertEquation(blendMode.colorEquation), convertEquation(blendMode.alphaEquation));
    }

    BatchRenderer::BatchRenderer(GLuint vertexArrayId, GLuint vertexBufferId, GLuint elementBufferId, GLuint defaultShaderProgramId, GLuint whiteTextureId)
        : m_vertexArrayId(vertexArrayId), m_vertexBufferId(vertexBufferId), m_elementBufferId(elementBufferId), m_defaultShaderProgramId(defaultShaderProgramId), m_whiteTextureId(whiteTextureId)
    {
        m_vertices.reserve(MAX_VERTICES);
        m_indices.reserve(MAX_INDICES);
    }
} // namespace processing
