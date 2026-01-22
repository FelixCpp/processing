#include <processing/processing.hpp>

#include <string_view>
#include <fstream>
#include <sstream>
#include <optional>
#include <string>
#include <filesystem>
#include <format>

#include <glad/gl.h>

namespace
{
    static std::optional<std::string> readFile(const std::filesystem::path& filepath)
    {
        const std::ifstream reader(filepath);
        if (not reader.is_open())
        {
            return std::nullopt;
        }

        std::ostringstream buffer;
        buffer << reader.rdbuf();
        return buffer.str();
    }
} // namespace

namespace processing
{
    class OpenGLShaderProgram : public ShaderProgramImpl
    {
    public:
        static std::unique_ptr<OpenGLShaderProgram> fromFile(const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath)
        {
            const auto vertexShaderSource = readFile(vertexShaderPath);
            if (not vertexShaderSource.has_value())
            {
                error(std::format("Could not read vertex shader: {}", vertexShaderPath.string()));
                return nullptr;
            }

            const auto fragmentShaderSource = readFile(fragmentShaderPath);
            if (not fragmentShaderSource.has_value())
            {
                error(std::format("Could not read fragment shader: {}", fragmentShaderPath.string()));
                return nullptr;
            }

            return fromMemory(*vertexShaderSource, *fragmentShaderSource);
        }

        static std::unique_ptr<OpenGLShaderProgram> fromMemory(const std::string_view vertexShaderSource, const std::string_view fragmentShaderSource)
        {
            // const char* VERTEX_SHADER = vertexShaderSource.data();
            // const char* FRAGMENT_SHADER = fragmentShaderSource.data();
            // GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
            // glShaderSource(vertexShaderId, 1, &VERTEX_SHADER, nullptr);
            // glCompileShader(vertexShaderId);
            //
            // GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
            // glShaderSource(fragmentShaderId, 1, &FRAGMENT_SHADER, nullptr);
            // glCompileShader(fragmentShaderId);
            //
            // ShaderProgramId shaderProgramId = {.value = glCreateProgram()};
            // glAttachShader(shaderProgramId.value, vertexShaderId);
            // glAttachShader(shaderProgramId.value, fragmentShaderId);
            // glLinkProgram(shaderProgramId.value);
            // glValidateProgram(shaderProgramId.value);
            // glDetachShader(shaderProgramId.value, vertexShaderId);
            // glDetachShader(shaderProgramId.value, fragmentShaderId);
            // glDeleteShader(vertexShaderId);
            // glDeleteShader(fragmentShaderId);

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
                    return nullptr;
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
                    return nullptr;
                }
            }

            ShaderProgramId shaderProgramId = {.value = glCreateProgram()};
            glAttachShader(shaderProgramId.value, vertexShaderId);
            glAttachShader(shaderProgramId.value, fragmentShaderId);
            glLinkProgram(shaderProgramId.value);
            // glValidateProgram(shaderProgramId.value);
            glDetachShader(shaderProgramId.value, vertexShaderId);
            glDetachShader(shaderProgramId.value, fragmentShaderId);
            glDeleteShader(vertexShaderId);
            glDeleteShader(fragmentShaderId);

            {
                GLint succeeded = GL_FALSE;
                glGetProgramiv(shaderProgramId.value, GL_LINK_STATUS, &succeeded);
                if (succeeded == GL_FALSE)
                {
                    GLint infoLogLength = 0;
                    glGetProgramiv(shaderProgramId.value, GL_INFO_LOG_LENGTH, &infoLogLength);
                    std::string buffer(static_cast<size_t>(infoLogLength), '\0');
                    glGetProgramInfoLog(shaderProgramId.value, buffer.length(), nullptr, buffer.data());
                    error(std::format("Failed to compile shader program: {}", buffer));
                    return nullptr;
                }
            }

            // {
            //     GLint succeeded = GL_FALSE;
            //     glGetProgramiv(shaderProgramId.value, GL_VALIDATE_STATUS, &succeeded);
            //     if (succeeded == GL_FALSE)
            //     {
            //         GLint infoLogLength = 0;
            //         glGetProgramiv(shaderProgramId.value, GL_INFO_LOG_LENGTH, &infoLogLength);
            //         std::string buffer(static_cast<size_t>(infoLogLength), '\0');
            //         glGetProgramInfoLog(shaderProgramId.value, buffer.length(), nullptr, buffer.data());
            //         error(std::format("Failed to compile shader program: {}", buffer));
            //         return nullptr;
            //     }
            // }

            return std::unique_ptr<OpenGLShaderProgram>(new OpenGLShaderProgram(shaderProgramId));
        }

        ~OpenGLShaderProgram() override
        {
            glDeleteProgram(m_shaderProgramId.value);
        }

        ShaderProgramId getResourceId() const override
        {
            return m_shaderProgramId;
        }

    private:
        explicit OpenGLShaderProgram(const ShaderProgramId shaderProgramId) : m_shaderProgramId(shaderProgramId)
        {
        }

        ShaderProgramId m_shaderProgramId;
    };
} // namespace processing

namespace processing
{
    ShaderProgram::ShaderProgram(std::unique_ptr<ShaderProgramImpl> impl) : m_impl(std::move(impl))
    {
    }

    ShaderProgramId ShaderProgram::getShaderProgramId() const
    {
        if (m_impl != nullptr)
        {
            return m_impl->getResourceId();
        }

        return ShaderProgramId{.value = 0};
    }

    ShaderProgram loadShaderFromMemory(std::string_view vertexShaderSource, std::string_view fragmentShaderSource)
    {
        return ShaderProgram(OpenGLShaderProgram::fromMemory(vertexShaderSource, fragmentShaderSource));
    }
} // namespace processing
