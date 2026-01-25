#ifndef _PROCESSING_INCLUDE_SHADER_HPP_
#define _PROCESSING_INCLUDE_SHADER_HPP_

#include <processing/processing.hpp>

#include <string_view>
#include <unordered_map>

#include <glad/gl.h>

namespace processing
{
    class ShaderAssetImpl : public ShaderImpl
    {
    public:
        static std::unique_ptr<ShaderAssetImpl> create(std::string_view vertexShaderSource, std::string_view fragmentShaderSource);
        ~ShaderAssetImpl() override;
        ResourceId getResourceId() const override;

        void uploadUniform(std::string_view name, float x) override;
        void uploadUniform(std::string_view name, float x, float y) override;
        void uploadUniform(std::string_view name, float x, float y, float z) override;
        void uploadUniform(std::string_view name, float x, float y, float z, float w) override;

    private:
        explicit ShaderAssetImpl(GLuint shaderProgramId);

        GLuint m_shaderProgramId;
    };

    class ShaderAssetManager
    {
    public:
        Shader loadShader(std::string_view vertexShaderSource, std::string_view fragmentShaderSource);
        ShaderImpl& getAsset(AssetId assetId);

    private:
        std::unordered_map<size_t, std::shared_ptr<ShaderImpl>> m_assets;
        size_t m_nextAssetId;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_SHADER_HPP_
