#ifndef _PROCESSING_INCLUDE_SHADER_HPP_
#define _PROCESSING_INCLUDE_SHADER_HPP_

#include <processing/processing.hpp>

namespace processing
{
    class ShaderAssetHandler
    {
    public:
        Shader create(std::string_view vertexShaderSource, std::string_view fragmentShaderSource);

    private:
        std::vector<std::shared_ptr<PlatformShader>> m_assets;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_SHADER_HPP_
