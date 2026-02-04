#ifndef _PROCESSING_INCLUDE_RENDERER_HPP_
#define _PROCESSING_INCLUDE_RENDERER_HPP_

#include <processing/processing.hpp>

namespace processing
{
    class DefaultRenderer : public Renderer
    {
    public:
        static std::unique_ptr<Renderer> create();

        void render(const Vertices& vertices, const RenderState& state) override;

    private:
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_RENDERER_HPP_
