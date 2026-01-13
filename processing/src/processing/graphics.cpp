#include <processing/render_targets.hpp>
#include <processing/processing.hpp>
#include <processing/renderer.hpp>

namespace processing
{
    color_t color(int32_t red, int32_t green, int32_t blue, int32_t alpha)
    {
        return color_t{
            .value = (uint32_t)(red << 24) | (uint32_t)(green << 16) | (uint32_t)(blue << 8) | (uint32_t)alpha
        };
    }

    color_t color(int32_t grey, int32_t alpha)
    {
        return color(grey, grey, grey, alpha);
    }

    int32_t red(color_t color)
    {
        return (color.value & 0xFF000000) >> 24;
    }

    int32_t green(color_t color)
    {
        return (color.value & 0x00FF0000) >> 16;
    }

    int32_t blue(color_t color)
    {
        return (color.value & 0x0000FF00) >> 8;
    }

    int32_t alpha(color_t color)
    {
        return (color.value & 0x000000FF);
    }
} // namespace processing

namespace processing
{
    Graphics::RenderStyle::RenderStyle()
        : fillColor(color(255)), strokeColor(color(255)), strokeWeight(1.0f), isFillEnabled(true), isStrokeEnabled(true)
    {
    }

    const Graphics::RenderStyle Graphics::RenderStyle::Default;
} // namespace processing

namespace processing
{
    Graphics::RenderStyleStack::RenderStyleStack() : m_currentStyle(0)
    {
    }

    void Graphics::RenderStyleStack::pushStyle()
    {
        if (m_currentStyle < m_renderStyles.size())
        {
            m_currentStyle++;
        }
    }

    void Graphics::RenderStyleStack::popStyle()
    {
        if (m_currentStyle > 0)
        {
            m_renderStyles[m_currentStyle] = RenderStyle::Default;
            --m_currentStyle;
        }
    }

    Graphics::RenderStyle& Graphics::RenderStyleStack::peekStyle()
    {
        return m_renderStyles[m_currentStyle];
    }

    void Graphics::RenderStyleStack::reset()
    {
        m_currentStyle = 0;
    }
} // namespace processing

namespace processing
{
    void Graphics::beginDraw()
    {
        m_renderer->beginDraw();
        m_renderTarget->beginDraw();
        m_renderStyles.reset();
    }

    void Graphics::endDraw()
    {
        m_renderTarget->endDraw();
        m_renderer->endDraw();
    }

    uint2 Graphics::getSize()
    {
        return m_renderTarget->getSize();
    }

    void Graphics::background(int red, int green, int blue, int alpha)
    {
        background(color(red, green, blue, alpha));
    }

    void Graphics::background(int grey, int alpha)
    {
        background(color(grey, grey, grey, alpha));
    }

    void Graphics::background(color_t color)
    {
        // error("Function is not yet implemented");

        Vertex vertices[] = {
            Vertex{
                .position = {-0.5f, 0.5f, 0.0f},
                .color = {1.0f, 0.0f, 0.0f, 1.0f},
            },
            Vertex{
                .position = {0.5f, 0.5f, 0.0f},
                .color = {1.0f, 0.0f, 0.0f, 1.0f},
            },
            Vertex{
                .position = {0.5f, -0.5f, 0.0f},
                .color = {1.0f, 0.0f, 0.0f, 1.0f},
            },
            Vertex{
                .position = {-0.5f, -0.5f, 0.0f},
                .color = {1.0f, 0.0f, 0.0f, 1.0f},
            },
        };

        uint32_t indices[] = {0, 1, 2, 2, 3, 0};

        m_renderer->submit(DrawSubmission{
            .vertices = vertices,
            .indices = indices,
        });
    }

    void Graphics::fill(int red, int green, int blue, int alpha)
    {
        fill(color(red, green, blue, alpha));
    }

    void Graphics::fill(int grey, int alpha)
    {
        fill(color(grey, alpha));
    }

    void Graphics::fill(color_t color)
    {
        RenderStyle& style = m_renderStyles.peekStyle();
        style.fillColor = color;
        style.isFillEnabled = true;
    }

    void Graphics::noFill()
    {
        RenderStyle& style = m_renderStyles.peekStyle();
        style.isFillEnabled = false;
    }

    void Graphics::stroke(int red, int green, int blue, int alpha)
    {
        stroke(color(red, green, blue, alpha));
    }

    void Graphics::stroke(int grey, int alpha)
    {
        stroke(color(grey, alpha));
    }

    void Graphics::stroke(color_t color)
    {
        RenderStyle& style = m_renderStyles.peekStyle();
        style.strokeColor = color;
        style.isStrokeEnabled = true;
    }

    void Graphics::noStroke()
    {
        RenderStyle& style = m_renderStyles.peekStyle();
        style.isStrokeEnabled = false;
    }

    void Graphics::strokeWeight(float strokeWeight)
    {
        RenderStyle& style = m_renderStyles.peekStyle();
        style.strokeWeight = strokeWeight;
    }

    void Graphics::rect(float left, float top, float width, float height)
    {
        error("Function is not yet implemented");
    }

    void Graphics::square(float left, float top, float size)
    {
        error("Function is not yet implemented");
    }

    void Graphics::ellipse(float centerX, float centerY, float radiusX, float radiusY)
    {
        error("Function is not yet implemented");
    }

    void Graphics::circle(float centerX, float centerY, float radius)
    {
        error("Function is not yet implemented");
    }

    void Graphics::line(float x1, float y1, float x2, float y2)
    {
        error("Function is not yet implemented");
    }

    void Graphics::triangle(float x1, float y1, float x2, float y2, float x3, float y3)
    {
        error("Function is not yet implemented");
    }

    void Graphics::point(float x, float y)
    {
        error("Function is not yet implemented");
    }

    Graphics::Graphics(std::shared_ptr<RenderTarget> renderTarget, std::shared_ptr<Renderer> renderer) : m_renderTarget(std::move(renderTarget)), m_renderer(std::move(renderer))
    {
    }

} // namespace processing
