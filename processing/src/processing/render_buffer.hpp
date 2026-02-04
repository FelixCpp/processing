#ifndef _PROCESSING_INCLUDE_RENDER_BUFFER_HPP_
#define _PROCESSING_INCLUDE_RENDER_BUFFER_HPP_

#include <processing/processing.hpp>

namespace processing
{
    class RenderbufferImpl : public Renderbuffer
    {
    public:
        static std::unique_ptr<RenderbufferImpl> create(u32 width, u32 height);
        ~RenderbufferImpl() override;

        ResourceId getResourceId() const override;
        uint2 getSize() const override;

    private:
        ResourceId m_resourceId;
        uint2 m_size;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_RENDER_BUFFER_HPP_
