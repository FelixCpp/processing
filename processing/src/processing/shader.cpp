#include <processing/processing.hpp>
#include <processing/shader.hpp>

#include <string_view>
#include <optional>
#include <string>
#include <format>

#include <glad/gl.h>

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
                glGetShaderInfoLog(vertexShaderId, buffer.length(), nullptr, buffer.data());
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
                glGetShaderInfoLog(fragmentShaderId, buffer.length(), nullptr, buffer.data());
                error(std::format("Failed to compile fragment shader: {}", buffer));
                return {};
            }
        }

        GLuint shaderProgramId = 0;
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
                glGetProgramInfoLog(shaderProgramId, buffer.length(), nullptr, buffer.data());
                error(std::format("Failed to compile shader program: {}", buffer));
                glDeleteProgram(shaderProgramId);
                return {};
            }
        }

        return shaderProgramId;
    }

    ShaderHandleManager::ShaderHandleManager() : m_nextHandleId(0)
    {
    }

    ShaderHandleManager::~ShaderHandleManager()
    {
        for (const auto& [_, entry] : m_entries)
        {
            glDeleteProgram(entry.shaderProgramId);
        }
    }

    ShaderHandle ShaderHandleManager::loadShader(const std::string_view vertexShaderSource, const std::string_view fragmentShaderSource)
    {
        if (const auto shaderProgramId = createProgram(vertexShaderSource, fragmentShaderSource))
        {
            static const auto hasher = std::hash<std::string_view>{};

            ++m_nextHandleId;

            ShaderHandle handle = {m_nextHandleId};

            m_entries.emplace(
                handle,
                ShaderHandleEntry{
                    .shaderProgramId = *shaderProgramId,
                    .vertexShaderHash = hasher(vertexShaderSource),
                    .fragmentShaderHash = hasher(fragmentShaderSource),
                }
            );

            return handle;
        }

        return INVALID_SHADER_HANDLE;
    }

    GLuint ShaderHandleManager::getResourceId(const ShaderHandle handle) const
    {
        const auto itr = m_entries.find(handle);
        if (itr != m_entries.end())
        {
            return itr->second.shaderProgramId;
        }

        return 0;
    }
} // namespace processing
