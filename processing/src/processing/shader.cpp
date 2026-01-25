#include <processing/processing.hpp>
#include <processing/shader.hpp>

#include <string_view>
#include <string>
#include <format>

#include <glad/gl.h>

namespace processing
{
    Shader::Shader()
    {
    }

    Shader::Shader(const AssetId assetId, std::weak_ptr<ShaderImpl> impl)
        : m_assetId(assetId),
          m_impl(impl)
    {
    }

    ResourceId Shader::getResourceId() const
    {
        return m_impl.lock()->getResourceId();
    }

    AssetId Shader::getAssetId() const
    {
        return m_assetId;
    }

    void Shader::uploadUniform(const std::string_view name, const float x)
    {
        m_impl.lock()->uploadUniform(name, x);
    }

    void Shader::uploadUniform(const std::string_view name, const float x, const float y)
    {
        m_impl.lock()->uploadUniform(name, x, y);
    }

    void Shader::uploadUniform(const std::string_view name, const float x, const float y, const float z)
    {
        m_impl.lock()->uploadUniform(name, x, y, z);
    }

    void Shader::uploadUniform(const std::string_view name, const float x, const float y, const float z, const float w)
    {
        m_impl.lock()->uploadUniform(name, x, y, z, w);
    }
} // namespace processing

namespace processing
{
    std::unique_ptr<ShaderAssetImpl> ShaderAssetImpl::create(std::string_view vertexShaderSource, std::string_view fragmentShaderSource)
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

        return std::unique_ptr<ShaderAssetImpl>(new ShaderAssetImpl(shaderProgramId));
    }

    ShaderAssetImpl::~ShaderAssetImpl()
    {
        glDeleteProgram(m_shaderProgramId);
    }

    ResourceId ShaderAssetImpl::getResourceId() const
    {
        return ResourceId{.value = m_shaderProgramId};
    }

    void ShaderAssetImpl::uploadUniform(const std::string_view name, const float x)
    {
        GLint location = glGetUniformLocation(m_shaderProgramId, name.data());
        glProgramUniform1f(m_shaderProgramId, location, x);
    }

    void ShaderAssetImpl::uploadUniform(const std::string_view name, const float x, const float y)
    {
        GLint location = glGetUniformLocation(m_shaderProgramId, name.data());
        glProgramUniform2f(m_shaderProgramId, location, x, y);
    }

    void ShaderAssetImpl::uploadUniform(const std::string_view name, const float x, const float y, const float z)
    {
        GLint location = glGetUniformLocation(m_shaderProgramId, name.data());
        glProgramUniform3f(m_shaderProgramId, location, x, y, z);
    }

    void ShaderAssetImpl::uploadUniform(const std::string_view name, const float x, const float y, const float z, const float w)
    {
        GLint location = glGetUniformLocation(m_shaderProgramId, name.data());
        glProgramUniform4f(m_shaderProgramId, location, x, y, z, w);
    }

    ShaderAssetImpl::ShaderAssetImpl(GLuint resourceId)
        : m_shaderProgramId(resourceId)
    {
    }

} // namespace processing

namespace processing
{
    Shader ShaderAssetManager::loadShader(const std::string_view vertexShaderSource, const std::string_view fragmentShaderSource)
    {
        if (auto shader = ShaderAssetImpl::create(vertexShaderSource, fragmentShaderSource))
        {
            const auto insertion = m_assets.insert(std::make_pair(m_nextAssetId++, std::move(shader)));
            return Shader(AssetId{.value = insertion.first->first}, insertion.first->second);
        }

        return Shader();
    }

    ShaderImpl& ShaderAssetManager::getAsset(const AssetId assetId)
    {
        if (const auto itr = m_assets.find(assetId.value); itr != m_assets.end())
        {
            return *itr->second;
        }

        throw std::runtime_error("Unknown asset id");
    }
} // namespace processing
