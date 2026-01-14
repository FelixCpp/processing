#ifndef _PROCESSING_INCLUDE_LIBRARY_DATA_HPP_
#define _PROCESSING_INCLUDE_LIBRARY_DATA_HPP_

#include <processing/processing.hpp>
#include <processing/window.hpp>
#include <processing/graphics_context.hpp>
#include <processing/render_targets.hpp>

namespace processing
{
    struct ProcessingData
    {
        bool shouldAppClose;
        bool shouldRestart;
        int exitCode;

        bool isMainLoopPaused;
        bool userRequestedRedraw;

        std::unique_ptr<Window> window;
        std::unique_ptr<GraphicsContext> context;
        std::shared_ptr<Renderer> renderer;
        std::unique_ptr<Sketch> sketch;
        std::shared_ptr<MainRenderTarget> mainRenderTarget;
        std::unique_ptr<GraphicsImpl> graphics;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_LIBRARY_DATA_HPP_
