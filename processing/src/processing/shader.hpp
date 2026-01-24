#ifndef _PROCESSING_INCLUDE_SHADER_HPP_
#define _PROCESSING_INCLUDE_SHADER_HPP_

#include <processing/processing.hpp>

#include <string_view>
#include <unordered_map>

#include <glad/gl.h>

namespace processing
{
    class OpenGLShaderHandleManager : public ShaderHandleManager
    {
    public:
        ~OpenGLShaderHandleManager() override;

        Shader loadShader(std::string_view vertexShaderSource, std::string_view fragmentShaderSource) override;
        uint32_t getResourceId(Shader shaderProgramId) const override;

        void uploadUniform(Shader id, std::string_view name, float x) override;
        void uploadUniform(Shader id, std::string_view name, float x, float y) override;
        void uploadUniform(Shader id, std::string_view name, float x, float y, float z) override;
        void uploadUniform(Shader id, std::string_view name, float x, float y, float z, float w) override;

    private:
        struct ShaderHasher
        {
            size_t operator()(Shader shader) const;
        };

        std::unordered_map<Shader, uint32_t, ShaderHasher> m_resourceIds;
        uint32_t m_nextShaderProgramId;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_SHADER_HPP_
