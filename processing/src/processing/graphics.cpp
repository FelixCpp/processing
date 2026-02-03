#include <processing/processing.hpp>

namespace processing
{
    void Graphics::push()
    {
        pushStyle();
        pushMatrix();
    }

    void Graphics::pop()
    {
        popStyle();
        popMatrix();
    }

    void Graphics::pushStyle(const bool extendPreviousStyle)
    {
        if (extendPreviousStyle)
        {
            m_renderStyles.push(peekStyle());
        }
        else
        {
            m_renderStyles.push(RenderStyle());
        }
    }

    void Graphics::popStyle()
    {
        if (m_renderStyles.size() > 1)
        {
            m_renderStyles.pop();
        }
    }

    RenderStyle& Graphics::peekStyle()
    {
        return m_renderStyles.top();
    }

    void Graphics::pushMatrix(const bool extendPreviousMatrix)
    {
        if (extendPreviousMatrix)
        {
            m_metrics.push(peekMatrix());
        }
        else
        {
            m_metrics.push(matrix4x4::identity);
        }
    }

    void Graphics::popMatrix()
    {
        if (m_metrics.size() > 1)
        {
            m_metrics.pop();
        }
    }

    void Graphics::resetMatrix()
    {
        peekMatrix() = matrix4x4::identity;
    }

    void Graphics::resetMatrix(const matrix4x4& matrix)
    {
        peekMatrix() = matrix;
    }

    matrix4x4& Graphics::peekMatrix()
    {
        return m_metrics.top();
    }

    void Graphics::translate(const f32 x, const f32 y)
    {
        resetMatrix(matrix4x4::translation(x, y).combined(peekMatrix()));
    }

    void Graphics::scale(const f32 x, const f32 y)
    {
        resetMatrix(matrix4x4::scaling(x, y).combined(peekMatrix()));
    }

    void Graphics::rotate(const f32 angle)
    {
        resetMatrix(matrix4x4::rotation(angle).combined(peekMatrix()));
    }

    void Graphics::blendMode(const BlendMode mode)
    {
        peekStyle().blendMode = mode;
    }

    void Graphics::angleMode(const AngleMode mode)
    {
        peekStyle().angleMode = mode;
    }

    void Graphics::rectMode(const RectMode mode)
    {
        peekStyle().rectMode = mode;
    }

    void Graphics::ellipseMode(const EllipseMode mode)
    {
        peekStyle().ellipseMode = mode;
    }

    void Graphics::imageMode(const RectMode mode)
    {
        peekStyle().imageMode = mode;
    }

    void Graphics::fill(const i32 red, const i32 green, const i32 blue, const i32 alpha)
    {
        fill(Color(red, green, blue, alpha));
    }

    void Graphics::fill(const i32 grey, const i32 alpha)
    {
        fill(Color(grey, alpha));
    }

    void Graphics::fill(const Color color)
    {
        RenderStyle& style = peekStyle();
        style.fillColor = color;
        style.isFillEnabled = true;
    }

    void Graphics::noFill()
    {
        peekStyle().isFillEnabled = false;
    }

    void Graphics::stroke(const i32 red, const i32 green, const i32 blue, const i32 alpha)
    {
        stroke(Color(red, green, blue, alpha));
    }

    void Graphics::stroke(const i32 grey, const i32 alpha)
    {
        stroke(Color(grey, alpha));
    }

    void Graphics::stroke(const Color color)
    {
        RenderStyle& style = peekStyle();
        style.strokeColor = color;
        style.isStrokeEnabled = true;
    }

    void Graphics::noStroke()
    {
        peekStyle().isStrokeEnabled = false;
    }

    void Graphics::strokeWeight(const f32 strokeWeight)
    {
        peekStyle().strokeWeight = strokeWeight;
    }

    void Graphics::strokeCap(const StrokeCap strokeCap)
    {
        peekStyle().strokeCap = strokeCap;
    }

    void Graphics::strokeJoin(const StrokeJoin strokeJoin)
    {
        peekStyle().strokeJoin = strokeJoin;
    }

    void Graphics::background(i32 red, i32 green, i32 blue, i32 alpha)
    {
        background(Color(red, green, blue, alpha));
    }

    void Graphics::background(i32 grey, i32 alpha)
    {
        background(Color(grey, alpha));
    }

    void Graphics::background(Color color)
    {
        RenderStyle& style = peekStyle();
    }
} // namespace processing
