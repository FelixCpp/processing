#include <processing/shader.hpp>
#include <glad/gl.h>

namespace processing
{
    class OpenGLPlatformShader : public PlatformShader
    {
    public:
        static std::unique_ptr<OpenGLPlatformShader> create(const std::string_view vertexShaderSource, const std::string_view fragmentShaderSource)
        {
            const char* vsSource = vertexShaderSource.data();
            const char* fsSource = fragmentShaderSource.data();

            ResourceId vsShader = {.value = glCreateShader(GL_VERTEX_SHADER)};
            glShaderSource(vsShader.value, 1, &vsSource, nullptr);
            glCompileShader(vsShader.value);

            ResourceId fsShader = {.value = glCreateShader(GL_FRAGMENT_SHADER)};
            glShaderSource(fsShader.value, 1, &fsSource, nullptr);
            glCompileShader(fsShader.value);

            ResourceId shaderProgramId = {.value = glCreateProgram()};
            glAttachShader(shaderProgramId.value, vsShader.value);
            glAttachShader(shaderProgramId.value, fsShader.value);
            glLinkProgram(shaderProgramId.value);
            glDetachShader(shaderProgramId.value, vsShader.value);
            glDetachShader(shaderProgramId.value, fsShader.value);
            glDeleteShader(vsShader.value);
            glDeleteShader(fsShader.value);

            return std::unique_ptr<OpenGLPlatformShader>(new OpenGLPlatformShader(shaderProgramId));
        }

        ResourceId getResourceId() const override
        {
            return m_shaderProgramId;
        }

    private:
        explicit OpenGLPlatformShader(const ResourceId shaderProgramId)
            : m_shaderProgramId(shaderProgramId)
        {
        }

        ResourceId m_shaderProgramId;
    };
} // namespace processing

namespace processing
{
    Shader ShaderAssetHandler::create(std::string_view vertexShaderSource, std::string_view fragmentShaderSource)
    {
        if (auto image = OpenGLPlatformShader::create(vertexShaderSource, fragmentShaderSource))
        {
            std::shared_ptr<PlatformShader>& ptr = m_assets.emplace_back(std::move(image));
            AssetId assetId = {.value = m_assets.size()};

            return Shader(assetId, ptr);
        }

        return Shader(AssetId{.value = 0}, nullptr);
    }
} // namespace processing

namespace processing
{
    Shader::Shader(const AssetId assetId, std::shared_ptr<PlatformShader> impl)
        : m_assetId{assetId},
          m_impl{std::move(impl)}
    {
    }

    ResourceId Shader::getResourceId() const
    {
        return m_impl->getResourceId();
    }

    AssetId Shader::getAssetId() const
    {
        return m_assetId;
    }
} // namespace processing
