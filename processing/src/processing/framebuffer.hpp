#ifndef _PROCESSING_INCLUDE_FRAMEBUFFER_HPP_
#define _PROCESSING_INCLUDE_FRAMEBUFFER_HPP_

#include <processing/processing.hpp>

namespace processing
{
    struct PlatformFramebuffer
    {
        virtual ~PlatformFramebuffer() = default;
        virtual Image& getImage() = 0;
        virtual uint2 getSize() const = 0;
        virtual ResourceId getResourceId() const = 0;
    };

    class Framebuffer
    {
    public:
        explicit Framebuffer(std::shared_ptr<PlatformFramebuffer> impl);

        Image& getImage();
        uint2 getSize() const;
        ResourceId getResourceId() const;

    private:
        std::shared_ptr<PlatformFramebuffer> m_impl;
    };

    Framebuffer createFramebuffer(u32 width, u32 height, FilterMode filterMode, ExtendMode extendMode);
} // namespace processing

#endif // _PROCESSING_INCLUDE_FRAMEBUFFER_HPP_
