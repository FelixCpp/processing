#ifndef _PROCESSING_INCLUDE_LIBRARY_DATA_HPP_
#define _PROCESSING_INCLUDE_LIBRARY_DATA_HPP_

#include <processing/processing.hpp>
#include <processing/window.hpp>
#include <processing/graphics_context.hpp>
#include <processing/graphics.hpp>

#include <stack>

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
        std::unique_ptr<Graphics> graphics;
        std::unique_ptr<Sketch> sketch;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_LIBRARY_DATA_HPP_
