#include <processing/shape_builder.hpp>
#include <cassert>
#include <cmath>
#include <algorithm>

namespace processing
{
    struct StrokeSegment
    {
        float2 point;
        Color color;

        float2 interInner;
        float2 interOuter;
        float2 prevOuter;
        float2 nextOuter;

        float2 bisector;
        float miterLength;
        bool miterLimitExceeded;
    };

    constexpr float compute_signed_area(const std::span<const float2>& points)
    {
        float area = 0.0f;

        for (size_t i = 0; i < points.size(); ++i)
        {
            const size_t j = (i + 1) % points.size();
            area += points[i].x * points[j].y;
            area -= points[j].x * points[i].y;
        }

        return area;
    }

    StrokeSegment segment_between_points(
        const float2& previous,
        const float2& current,
        const float2& next,
        const Color& currentColor,
        const float strokeWeight,
        const float miterLimit,
        const bool isClockwise
    )
    {
        const float2 dirPrev = (current - previous).normalized();
        const float2 dirNext = (next - current).normalized();

        const float2 nPrev = isClockwise ? dirPrev.perpendicular_cw() : dirPrev.perpendicular_ccw();
        const float2 nNext = isClockwise ? dirNext.perpendicular_cw() : dirNext.perpendicular_ccw();

        const float halfStrokeWeight = strokeWeight * 0.5f;

        const float2 prevOuter = current + nPrev * halfStrokeWeight;
        const float2 prevInner = current - nPrev * halfStrokeWeight;
        const float2 nextOuter = current + nNext * halfStrokeWeight;
        const float2 nextInner = current - nNext * halfStrokeWeight;

        const float2 bisector = (nPrev + nNext).normalized();
        float dotProduct = bisector.dot(nNext);

        constexpr float epsilon = std::numeric_limits<float>::epsilon();
        if (std::abs(dotProduct) < epsilon)
        {
            dotProduct = (dotProduct >= 0.0f) ? epsilon : -epsilon;
        }

        const float maxMiterLength = halfStrokeWeight * miterLimit;

        float miterLength = halfStrokeWeight / dotProduct;
        const bool miterLimitExceeded = std::abs(miterLength) > maxMiterLength;
        if (miterLimitExceeded)
        {
            miterLength = maxMiterLength * (miterLength > 0 ? 1.0f : -1.0f);
        }

        const float2 outerIntersection = current + bisector * miterLength;
        const float2 innerIntersection = current - bisector * miterLength;

        return {
            .point = current,
            .color = currentColor,
            .interInner = innerIntersection,
            .interOuter = outerIntersection,
            .prevOuter = prevOuter,
            .nextOuter = nextOuter,
            .bisector = bisector,
            .miterLength = miterLength,
            .miterLimitExceeded = miterLimitExceeded,
        };
    }

    static std::vector<StrokeSegment> compute_stroke_segments(const std::span<const float2>& points, const std::span<const Color>& colors, const float strokeWeight, const float miterLimit)
    {
        std::vector<StrokeSegment> segments;
        segments.reserve(points.size());

        const bool isClockwise = compute_signed_area(points) > 0.0f;

        for (size_t i = 0; i < points.size(); ++i)
        {
            const size_t prevIdx = i == 0 ? (points.size() - 1) : (i - 1);
            const size_t nextIdx = (i + 1) % points.size();
            const size_t currIdx = i;

            const float2& prevPt = points[prevIdx];
            const float2& nextPt = points[nextIdx];
            const float2& currPt = points[currIdx];

            segments.push_back(segment_between_points(prevPt, currPt, nextPt, colors[currIdx], strokeWeight, miterLimit, isClockwise));
        }

        return segments;
    }

    static PolygonContour contour_stroke_from_path(const std::span<const float2>& points, const std::span<const Color>& colors, const StrokeProperties& properties)
    {
        if (points.size() < 3)
        {
            return {};
        }

        const std::vector<StrokeSegment> segments = compute_stroke_segments(points, colors, properties.strokeWeight, properties.miterLimit);
        std::vector<float2> positions;
        std::vector<float2> texcoords;
        std::vector<Color> cols;
        std::vector<uint32_t> indices;

        for (size_t i = 0; i < segments.size(); ++i)
        {
            const size_t prevIdx = (i == 0) ? (segments.size() - 1) : (i - 1);
            const size_t nextIdx = (i + 1) % segments.size();
            const size_t currIdx = i;

            const StrokeSegment& prev = segments[prevIdx];
            const StrokeSegment& next = segments[nextIdx];
            const StrokeSegment& curr = segments[currIdx];

            if (true)
            {
                const size_t idxStart = positions.size();
                positions.push_back(curr.nextOuter);
                positions.push_back(curr.interInner);
                positions.push_back(next.interInner);
                positions.push_back(next.prevOuter);

                cols.push_back(curr.color);
                cols.push_back(curr.color);
                cols.push_back(next.color);
                cols.push_back(next.color);

                indices.push_back(idxStart + 0);
                indices.push_back(idxStart + 1);
                indices.push_back(idxStart + 2);

                indices.push_back(idxStart + 2);
                indices.push_back(idxStart + 3);
                indices.push_back(idxStart + 0);
            }

            StrokeJoin join = properties.strokeJoin;
            if (join == StrokeJoin::miter and curr.miterLimitExceeded)
                join = StrokeJoin::bevel;

            switch (join)
            {
                case StrokeJoin::miter:
                {
                    const size_t idxStart = positions.size();
                    positions.push_back(curr.interInner);
                    positions.push_back(curr.prevOuter);
                    positions.push_back(curr.interOuter);
                    positions.push_back(curr.nextOuter);

                    cols.push_back(curr.color);
                    cols.push_back(curr.color);
                    cols.push_back(curr.color);
                    cols.push_back(curr.color);

                    indices.push_back(idxStart + 0);
                    indices.push_back(idxStart + 1);
                    indices.push_back(idxStart + 2);

                    indices.push_back(idxStart + 2);
                    indices.push_back(idxStart + 3);
                    indices.push_back(idxStart + 0);
                    break;
                }

                case StrokeJoin::bevel:
                {
                    const size_t idxStart = positions.size();
                    positions.push_back(curr.interInner);
                    positions.push_back(curr.prevOuter);
                    positions.push_back(curr.nextOuter);

                    cols.push_back(curr.color);
                    cols.push_back(curr.color);
                    cols.push_back(curr.color);

                    indices.push_back(idxStart + 0);
                    indices.push_back(idxStart + 1);
                    indices.push_back(idxStart + 2);
                    break;
                }

                case StrokeJoin::round:
                {
                    const float2 center = curr.point;
                    const float2 arcStart = curr.prevOuter;
                    const float2 arcEnd = curr.nextOuter;
                    const float2 toStart = arcStart - center;
                    const float2 toEnd = arcEnd - center;
                    const float startAngle = std::atan2(toStart.y, toStart.x);
                    const float endAngle = std::atan2(toEnd.y, toEnd.x);
                    float sweepAngle = endAngle - startAngle;
                    while (sweepAngle > PI)
                        sweepAngle -= TAU;
                    while (sweepAngle < -PI)
                        sweepAngle += TAU;
                    const size_t numSegments = 4;
                    const float angleStepSize = sweepAngle / static_cast<float>(numSegments);
                    const float radius = properties.strokeWeight * 0.5f;

                    const size_t idxStart = positions.size();
                    positions.push_back(center);
                    cols.push_back(curr.color);
                    const size_t centerIdx = idxStart;

                    for (size_t j = 0; j <= numSegments; ++j)
                    {
                        const float angle = startAngle + angleStepSize * static_cast<float>(j);
                        const float2 arcPoint = {
                            center.x + std::cos(angle) * radius,
                            center.y + std::sin(angle) * radius,
                        };

                        positions.push_back(arcPoint);
                        cols.push_back(curr.color);
                    }

                    for (size_t j = 0; j < numSegments; ++j)
                    {
                        indices.push_back(centerIdx);
                        indices.push_back(idxStart + 1 + j);
                        indices.push_back(idxStart + 1 + j + 1);
                    }

                    positions.push_back(curr.interInner);
                    cols.push_back(curr.color);
                    const size_t innerIdx = positions.size() - 1;

                    indices.push_back(centerIdx);
                    indices.push_back(idxStart + 1 + numSegments);
                    indices.push_back(innerIdx);

                    indices.push_back(centerIdx);
                    indices.push_back(innerIdx);
                    indices.push_back(idxStart + 1);

                    break;
                }
            }
        }

        texcoords.resize(positions.size(), {});

        return {
            .points = std::move(positions),
            .texcoords = std::move(texcoords),
            .colors = std::move(cols),
            .indices = std::move(indices),
        };
    }
} // namespace processing

// Contour contour_image(float left, float top, float width, float height, float sourceLeft, float sourceTop, float sourceWidth, float sourceHeight)
// {
//     Contour contour;
//
//     {
//         const float right = left + width;
//         const float bottom = top + height;
//
//         contour.positions = {
//             {left, top},
//             {right, top},
//             {right, bottom},
//             {left, bottom},
//         };
//     }
//
//     {
//         const float sourceRight = sourceLeft + sourceWidth;
//         const float sourceBottom = sourceTop + sourceHeight;
//
//         //     contour.texcoords = {
//         //         {sourceLeft, sourceTop},
//         //         {sourceRight, sourceTop},
//         //         {sourceRight, sourceBottom},
//         //         {sourceLeft, sourceBottom},
//         //     };
//
//         contour.texcoords = {
//             {sourceLeft, sourceBottom},
//             {sourceRight, sourceBottom},
//             {sourceRight, sourceTop},
//             {sourceLeft, sourceTop},
//         };
//     }
//
//     contour.indices = {0, 1, 2, 2, 3, 0};
//
//     return contour;
// }

namespace processing
{
    PolygonContour contour_polygon_quad_fill(const std::span<const float2, 4>& points, const std::span<const Color, 4>& colors)
    {
        return PolygonContour{
            .points = std::vector<float2>{points.begin(), points.end()},
            .texcoords = {
                {0.0f, 0.0f},
                {1.0f, 0.0f},
                {1.0f, 1.0f},
                {0.0f, 1.0f},
            },
            .colors = std::vector<Color>{colors.begin(), colors.end()},
            .indices = {0, 1, 2, 2, 3, 0}
        };
    }

    PolygonContour contour_polygon_quad_stroke(const std::span<const float2, 4>& points, const std::span<const Color, 4>& colors, const StrokeProperties& properties)
    {
        return contour_stroke_from_path(points, colors, properties);
    }
} // namespace processing

namespace processing
{
    EllipsePath path_ellipse(const EllipseProperties& properties)
    {
        std::vector<float2> points;
        for (usize i = 0; i < properties.segments; ++i)
        {
            const f32 angle = 2.0f * PI * i / static_cast<f32>(properties.segments);
            const f32 x = properties.center.x + std::cos(angle) * properties.radius.x;
            const f32 y = properties.center.y + std::sin(angle) * properties.radius.y;

            points.emplace_back(x, y);
        }

        return EllipsePath{
            .properties = properties,
            .points = points,
        };
    }

    PolygonContour contour_polygon_ellipse_fill(const EllipsePath& path, Color color)
    {
        const float left = path.properties.center.x - path.properties.radius.x;
        const float right = path.properties.center.x + path.properties.radius.x;
        const float top = path.properties.center.y - path.properties.radius.y;
        const float bottom = path.properties.center.y + path.properties.radius.y;
        const float width = right - left;
        const float height = bottom - top;

        std::vector<float2> points;
        std::vector<float2> texcoords;
        std::vector<Color> colors;

        points.emplace_back(left + width * 0.5f, top + height * 0.5f);
        texcoords.emplace_back(0.5f, 0.5f);
        colors.emplace_back(color);

        for (size_t i = 0; i < path.properties.segments; ++i)
        {
            const float2& point = path.points[i];
            points.emplace_back(point);

            const float tx = (point.x - left) / width;
            const float ty = (point.y - top) / height;
            texcoords.emplace_back(tx, ty);
            colors.emplace_back(color);
        }

        std::vector<u32> indices;
        for (size_t i = 1; i <= path.properties.segments; ++i)
        {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i < path.properties.segments ? i + 1 : 1);
        }

        return PolygonContour{
            .points = std::move(points),
            .texcoords = std::move(texcoords),
            .colors = std::move(colors),
            .indices = std::move(indices),
        };
    }

    PolygonContour contour_polygon_ellipse_stroke(const EllipsePath& path, Color color, const StrokeProperties& properties)
    {
        std::vector<Color> colors(path.points.size(), color);
        return contour_stroke_from_path(path.points, colors, properties);
    }
} // namespace processing

namespace processing
{
    PolygonContour contour_polygon_triangle_fill(const std::span<const float2, 3>& points, const std::span<const Color, 3>& colors)
    {
        float minX = points[0].x;
        float minY = points[0].y;
        float maxX = points[0].x;
        float maxY = points[0].y;

        for (size_t i = 1; i < points.size(); ++i)
        {
            minX = std::min(minX, points[i].x);
            minY = std::min(minY, points[i].y);
            maxX = std::max(maxX, points[i].x);
            maxY = std::max(maxY, points[i].y);
        }

        const float width = maxX - minX;
        const float height = maxY - minY;

        PolygonContour contour;
        contour.points.append_range(points);
        contour.colors.append_range(colors);
        contour.texcoords.reserve(points.size());
        for (size_t i = 0; i < points.size(); ++i)
        {
            const float tx = (points[i].x - minX) / width;
            const float ty = (points[i].y - minY) / height;
            contour.texcoords.emplace_back(tx, ty);
        }

        contour.indices = {0, 1, 2};
        return contour;
    }

    PolygonContour contour_polygon_triangle_stroke(const std::span<const float2, 3>& points, const std::span<const Color, 3>& colors, const StrokeProperties& properties)
    {
        return contour_stroke_from_path(points, colors, properties);
    }
} // namespace processing

namespace processing
{
    PolygonContour contour_polygon_line(const std::span<const float2, 2>& points, const std::span<const Color, 2>& colors, float strokeWeight, StrokeCap strokeCap)
    {
        const float2 start = points[0];
        const float2 end = points[1];
        const float2 direction = (end - start).normalized();
        const float2 offset = direction.perpendicular_cw() * strokeWeight * 0.5f;

        const float circumference = PI * strokeWeight;
        const size_t segments = std::max(4, static_cast<int>(circumference / 4.0f));

        std::vector<float2> positions;
        std::vector<Color> cols;
        switch (strokeCap.start)
        {
            case StrokeCapStyle::butt:
            {
                positions.emplace_back(start - offset);
                positions.emplace_back(start + offset);
                break;
            }

            case StrokeCapStyle::square:
            {
                const float2 extend = direction * strokeWeight * 0.5f;
                positions.emplace_back(start - extend - offset);
                positions.emplace_back(start - extend + offset);
                break;
            }

            case StrokeCapStyle::round:
            {
                for (size_t i = 0; i <= segments; ++i)
                {
                    float angle = PI * i / segments;
                    float2 dir = offset.rotated(angle);
                    positions.emplace_back(start - dir);
                }
                break;
            }
        }

        const size_t startPositionsCount = positions.size();
        cols.insert(cols.end(), startPositionsCount, colors[0]);

        switch (strokeCap.end)
        {
            case StrokeCapStyle::butt:
            {
                positions.emplace_back(end - offset);
                positions.emplace_back(end + offset);
                break;
            }

            case StrokeCapStyle::square:
            {
                const float2 extend = direction * strokeWeight * 0.5f;
                positions.emplace_back(end + extend - offset);
                positions.emplace_back(end + extend + offset);
                break;
            }

            case StrokeCapStyle::round:
            {
                for (size_t i = 0; i <= segments; ++i)
                {
                    float angle = PI * i / segments;
                    float2 dir = offset.rotated(PI - angle);
                    positions.emplace_back(end + dir);
                }
                break;
            }
        }

        const size_t endPositionsCount = positions.size() - startPositionsCount;
        const size_t maxCount = std::max(startPositionsCount, endPositionsCount);
        cols.insert(cols.end(), endPositionsCount, colors[0]);

        std::vector<uint32_t> indices;
        for (size_t i = 0; i < maxCount; ++i)
        {
            const size_t left1 = std::min(i, startPositionsCount - 1);
            const size_t left2 = std::min(i + 1, startPositionsCount - 1);
            const size_t right1 = startPositionsCount + std::min(i, endPositionsCount - 1);
            const size_t right2 = startPositionsCount + std::min(i + 1, endPositionsCount - 1);

            // Erstes Dreieck
            indices.emplace_back(left1);
            indices.emplace_back(right1);
            indices.emplace_back(left2);

            // Zweites Dreieck
            indices.emplace_back(left2);
            indices.emplace_back(right1);
            indices.emplace_back(right2);
        }

        std::vector<float2> texcoords(positions.size());

        return PolygonContour{
            .points = std::move(positions),
            .texcoords = std::move(texcoords),
            .colors = std::move(cols),
            .indices = std::move(indices)
        };
    }
} // namespace processing

namespace processing
{
    PolygonContour contour_polygon_image(const std::span<const float2, 4>& points, const std::span<const Color>& colors, const rect2f& sourceRect)
    {
        return PolygonContour{
            .points = std::vector<float2>{points.begin(), points.end()},
            .texcoords = {
                float2{sourceRect.left, sourceRect.top},
                float2{sourceRect.right(), sourceRect.top},
                float2{sourceRect.right(), sourceRect.bottom()},
                float2{sourceRect.left, sourceRect.bottom()},
            },
            .colors = std::vector<Color>{colors.begin(), colors.end()},
            .indices = {0, 1, 2, 2, 3, 0}
        };
    }
} // namespace processing

namespace processing
{
    PolygonContour contour_polygon_polygon_fill(const std::span<const float2>& points, const std::span<const float2>& texcoords, const std::span<const Color>& colors)
    {
        std::vector<uint32_t> indices;
        for (size_t i = 0; i < points.size(); ++i)
            indices.emplace_back(i);

        return PolygonContour{
            .points = std::vector<float2>{points.begin(), points.end()},
            .texcoords = std::vector<float2>{texcoords.begin(), texcoords.end()},
            .colors = std::vector<Color>{colors.begin(), colors.end()},
            .indices = std::move(indices),
        };
    }

    PolygonContour contour_polygon_polygon_stroke(const std::span<const float2>& points, const std::span<const Color>& colors, const StrokeProperties& properties)
    {
        return contour_stroke_from_path(points, colors, properties);
    }
} // namespace processing
