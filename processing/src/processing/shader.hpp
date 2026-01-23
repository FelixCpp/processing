#ifndef _PROCESSING_INCLUDE_SHADER_HPP_
#define _PROCESSING_INCLUDE_SHADER_HPP_

#include <processing/processing.hpp>

#include <string_view>
#include <unordered_map>

#include <glad/gl.h>

namespace processing
{
    struct ShaderHandleEntry
    {
        GLuint shaderProgramId;
        std::size_t vertexShaderHash;
        std::size_t fragmentShaderHash;
    };

    class ShaderHandleManager
    {
    public:
        ShaderHandleManager();
        ~ShaderHandleManager();

        ShaderHandleManager(const ShaderHandleManager&) = delete;
        ShaderHandleManager& operator=(const ShaderHandleManager&) = delete;
        ShaderHandleManager(ShaderHandleManager&&) = default;
        ShaderHandleManager& operator=(ShaderHandleManager&&) = default;

        ShaderHandle loadShader(std::string_view vertexShaderSource, std::string_view fragmentShaderHash);
        GLuint getResourceId(ShaderHandle handle) const;

    private:
        std::unordered_map<ShaderHandle, ShaderHandleEntry> m_entries;
        uint32_t m_nextHandleId;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_SHADER_HPP_
