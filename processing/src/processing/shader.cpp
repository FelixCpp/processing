#include <processing/processing.hpp>
#include <processing/processing_data.hpp>
#include <processing/shader.hpp>

#include <string_view>
#include <optional>
#include <string>
#include <format>

#include <glad/gl.h>

namespace processing
{
    extern ProcessingData s_data;
}

namespace processing
{
    static std::optional<GLuint> createProgram(const std::string_view vertexShaderSource, const std::string_view fragmentShaderSource)
    {
        const char* vsSource = vertexShaderSource.data();
        GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShaderId, 1, &vsSource, nullptr);
        glCompileShader(vertexShaderId);

        {
            GLint succeeded = GL_FALSE;
            glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &succeeded);
            if (succeeded == GL_FALSE)
            {
                GLint infoLogLength = 0;
                glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
                std::string buffer(static_cast<size_t>(infoLogLength), '\0');
                glGetShaderInfoLog(vertexShaderId, buffer.length(), &infoLogLength, buffer.data());
                error(std::format("Failed to compile vertex shader: {}", buffer));
                return {};
            }
        }

        const char* fsSource = fragmentShaderSource.data();
        GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShaderId, 1, &fsSource, nullptr);
        glCompileShader(fragmentShaderId);

        {
            GLint succeeded = GL_FALSE;
            glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &succeeded);
            if (succeeded == GL_FALSE)
            {
                GLint infoLogLength = 0;
                glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
                std::string buffer(static_cast<size_t>(infoLogLength), '\0');
                glGetShaderInfoLog(fragmentShaderId, buffer.length(), &infoLogLength, buffer.data());
                error(std::format("Failed to compile fragment shader: {}", buffer));
                return {};
            }
        }

        GLuint shaderProgramId = glCreateProgram();
        glAttachShader(shaderProgramId, vertexShaderId);
        glAttachShader(shaderProgramId, fragmentShaderId);
        glLinkProgram(shaderProgramId);
        glDetachShader(shaderProgramId, vertexShaderId);
        glDetachShader(shaderProgramId, fragmentShaderId);
        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);

        {
            GLint succeeded = GL_FALSE;
            glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &succeeded);
            if (succeeded == GL_FALSE)
            {
                GLint infoLogLength = 0;
                glGetProgramiv(shaderProgramId, GL_INFO_LOG_LENGTH, &infoLogLength);
                std::string buffer(static_cast<size_t>(infoLogLength), '\0');
                glGetProgramInfoLog(shaderProgramId, buffer.length(), &infoLogLength, buffer.data());
                error(std::format("Failed to compile shader program: {}", buffer));
                glDeleteProgram(shaderProgramId);
                return {};
            }
        }

        return shaderProgramId;
    }

    Shader OpenGLShaderHandleManager::loadShader(const std::string_view vertexShaderSource, const std::string_view fragmentShaderSource)
    {
        if (const std::optional<GLuint> resourceId = createProgram(vertexShaderSource, fragmentShaderSource))
        {
            const auto clientId = Shader{++m_nextShaderProgramId};
            m_resourceIds.insert(std::make_pair(clientId, *resourceId));
            return clientId;
        }

        return INVALID_SHADER_HANDLE;
    }

    uint32_t OpenGLShaderHandleManager::getResourceId(const Shader shaderProgramId) const
    {
        const auto itr = m_resourceIds.find(shaderProgramId);
        if (itr != m_resourceIds.end())
        {
            return itr->second;
        }

        warning(std::format("Unknown ShaderProgramId: {}", shaderProgramId));
        return 0;
    }
} // namespace processing
