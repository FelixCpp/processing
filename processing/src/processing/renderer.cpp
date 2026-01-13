#include <processing/renderer.hpp>

namespace processing
{
    bool BatchKey::operator==(const BatchKey& other) const
    {
        return shaderProgramId == other.shaderProgramId and textureId == other.textureId;
    }

    size_t BatchKeyHash::operator()(const BatchKey& key) const
    {
        size_t h1 = std::hash<GLuint>{}(key.shaderProgramId);
        size_t h2 = std::hash<GLuint>{}(key.textureId);
        return h1 ^ (h2 << 1);
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

        layout (location = 0) out vec2 v_TexCoord;
        layout (location = 1) out vec4 v_Color;

        void main()
        {
            gl_Position = vec4(a_Position, 1.0);
            v_TexCoord = a_TexCoord;
            v_Color = a_Color;
        }
    )";

    inline static constexpr const char* FRAGMENT_SHADER = R"(
        #version 330 core

        layout (location = 0) out vec4 o_Color;

        layout (location = 0) in vec2 v_TexCoord;
        layout (location = 1) in vec4 v_Color;

        uniform sampler2D u_TextureSampler;

        void main()
        {
            o_Color = vec4(1.0, 0.0, 0.0, 1.0);
        }
    )";

    std::unique_ptr<Renderer> Renderer::create()
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

        return std::unique_ptr<Renderer>(new Renderer(vertexArrayId, vertexBufferId, elementBufferId, shaderProgramId, textureId));
    }

    Renderer::~Renderer()
    {
        glDeleteVertexArrays(1, &m_vertexArrayId);
        glDeleteBuffers(1, &m_vertexBufferId);
        glDeleteBuffers(1, &m_elementBufferId);
        glDeleteTextures(1, &m_whiteTextureId);
        glDeleteProgram(m_defaultShaderProgramId);
    }

    void Renderer::beginDraw()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::endDraw()
    {
        flush();
    }

    void Renderer::submit(const DrawSubmission& submission)
    {
        if (m_vertices.size() + submission.vertices.size() >= MAX_VERTICES or m_indices.size() + submission.indices.size() >= MAX_INDICES)
        {
            flush();
        }

        const BatchKey key{
            .shaderProgramId = submission.shaderProgramId.value_or(m_defaultShaderProgramId),
            .textureId = submission.textureId.value_or(m_whiteTextureId),
        };

        const size_t vertexOffset = m_vertices.size();
        const size_t indexStart = m_indices.size();

        m_vertices.append_range(submission.vertices);

        for (uint32_t index : submission.indices)
        {
            m_indices.push_back(index + static_cast<uint32_t>(vertexOffset));
        }

        const auto itr = m_batches.find(key);
        if (itr != m_batches.end())
        {
            Batch& batch = itr->second;
            size_t expectedStart = batch.indexStart + batch.indexCount;

            if (expectedStart == indexStart)
            {
                batch.indexCount += submission.indices.size();
            }
            else
            {
                batch.indexStart = indexStart;
                batch.indexCount = submission.indices.size();
            }
        }
        else
        {
            // Neuer Batch
            m_batches.emplace(std::make_pair(key, Batch{.indexStart = indexStart, .indexCount = submission.indices.size()}));
        }
    }

    void Renderer::flush()
    {
        if (m_batches.empty())
        {
            return;
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size() * sizeof(Vertex), m_vertices.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferId);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_indices.size() * sizeof(uint32_t), m_indices.data());

        glBindVertexArray(m_vertexArrayId);

        GLuint currentShader = 0;
        GLuint currentTexture = 0;

        for (const auto& [key, batch] : m_batches)
        {
            if (key.shaderProgramId != currentShader)
            {
                currentShader = key.shaderProgramId;
                glUseProgram(currentShader);
            }

            if (key.textureId != currentTexture)
            {
                currentTexture = key.textureId;
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, currentTexture);
            }

            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(batch.indexCount), GL_UNSIGNED_INT, (void*)(batch.indexStart * sizeof(uint32_t)));
        }

        m_batches.clear();
        m_vertices.clear();
        m_indices.clear();
    }

    Renderer::Renderer(GLuint vertexArrayId, GLuint vertexBufferId, GLuint elementBufferId, GLuint defaultShaderProgramId, GLuint whiteTextureId)
        : m_vertexArrayId(vertexArrayId), m_vertexBufferId(vertexBufferId), m_elementBufferId(elementBufferId), m_defaultShaderProgramId(defaultShaderProgramId), m_whiteTextureId(whiteTextureId)
    {
        m_vertices.reserve(MAX_VERTICES);
        m_indices.reserve(MAX_INDICES);
    }

} // namespace processing
