#ifndef _PROCESSING_INCLUDE_WINDOW_HPP_
#define _PROCESSING_INCLUDE_WINDOW_HPP_

#include <processing/processing.hpp>

#include <memory>
#include <string_view>
#include <optional>

namespace processing
{
    struct Window
    {
        virtual ~Window() = default;
        virtual void setSize(uint32_t width, uint32_t height) = 0;
        virtual void setTitle(std::string_view title) = 0;
        virtual std::optional<Event> pollEvent() = 0;
    };

    std::unique_ptr<Window> createWindow(uint32_t width, uint32_t height, std::string_view title);
} // namespace processing

#endif // _PROCESSING_INCLUDE_WINDOW_HPP_
