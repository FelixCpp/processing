#ifndef _PROCESSING_INCLUDE_GRAPHICSCONTEXT_HPP_
#define _PROCESSING_INCLUDE_GRAPHICSCONTEXT_HPP_

#include <memory>

namespace processing
{
    struct Window;

    struct GraphicsContext
    {
        virtual ~GraphicsContext() = default;
        virtual void setVerticalSyncEnabled(bool enabled) = 0;
        virtual bool isVerticalSyncEnabled() const = 0;
        virtual void flush() = 0;
    };

    std::unique_ptr<GraphicsContext> createContext(Window& window);
} // namespace processing

#endif // _PROCESSING_INCLUDE_GRAPHICSCONTEXT_HPP_
