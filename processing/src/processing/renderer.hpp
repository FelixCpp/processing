#ifndef _PROCESSING_INCLUDE_RENDERER_HPP_
#define _PROCESSING_INCLUDE_RENDERER_HPP_

#include <processing/processing.hpp>

namespace processing
{
    struct RenderState
    {
    };

    class Renderer
    {
    public:
        static std::unique_ptr<Renderer> create();

        void draw();

    private:
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_RENDERER_HPP_
