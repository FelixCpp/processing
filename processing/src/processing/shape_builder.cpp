#include <processing/shape_builder.hpp>
#include <cassert>
#include <cmath>
#include <algorithm>

namespace processing
{
    struct StrokeSegment
    {
        float2 point;

        float2 interInner;
        float2 interOuter;
        float2 prevOuter;
        float2 nextOuter;

        float2 bisector;
        float miterLength;
        bool miterLimitExceeded;
    };

    constexpr float compute_signed_area(const std::vector<float2>& points)
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

    StrokeSegment segment_between_points(const float2& previous, const float2& current, const float2& next, const float strokeWeight, const float miterLimit, const bool isClockwise)
    {
        const float2 dirPrev = value2_normalized(current - previous);
        const float2 dirNext = value2_normalized(next - current);

        const float2 nPrev = isClockwise ? value2_perpendicular_cw(dirPrev) : value2_perpendicular_ccw(dirPrev);
        const float2 nNext = isClockwise ? value2_perpendicular_cw(dirNext) : value2_perpendicular_ccw(dirNext);

        const float halfStrokeWeight = strokeWeight * 0.5f;

        const float2 prevOuter = current + nPrev * halfStrokeWeight;
        const float2 prevInner = current - nPrev * halfStrokeWeight;
        const float2 nextOuter = current + nNext * halfStrokeWeight;
        const float2 nextInner = current - nNext * halfStrokeWeight;

        const float2 bisector = value2_normalized(nPrev + nNext);
        float dotProduct = value2_dot(bisector, nNext);

        constexpr float epsilon = std::numeric_limits<float>::epsilon();
        if (std::abs(dotProduct) < epsilon)
        {
            dotProduct = (dotProduct >= 0.0f) ? epsilon : -epsilon;
        }

        const float maxMiterLength = halfStrokeWeight * miterLimit;

        float miterLength = halfStrokeWeight / dotProduct;
        bool miterLimitExceeded = false;
        if (std::abs(miterLength) > maxMiterLength)
        {
            miterLength = maxMiterLength * (miterLength > 0 ? 1.0f : -1.0f);
            miterLimitExceeded = true;
        }

        const float2 outerIntersection = current + bisector * miterLength;
        const float2 innerIntersection = current - bisector * miterLength;

        return {
            .point = current,
            .interInner = innerIntersection,
            .interOuter = outerIntersection,
            .prevOuter = prevOuter,
            .nextOuter = nextOuter,
            .bisector = bisector,
            .miterLength = miterLength,
            .miterLimitExceeded = miterLimitExceeded,
        };
    }

    static std::vector<StrokeSegment> compute_stroke_segments(const std::vector<float2>& points, const float strokeWeight, const float miterLimit)
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

            segments.push_back(segment_between_points(prevPt, currPt, nextPt, strokeWeight, miterLimit, isClockwise));
        }

        return segments;
    }

    static Contour contour_stroke_from_path(const std::vector<float2>& points, const StrokeProperties& properties)
    {
        if (points.size() < 3)
        {
            return {};
        }

        const std::vector<StrokeSegment> segments = compute_stroke_segments(points, properties.strokeWeight, properties.miterLimit);
        std::vector<float2> positions;
        std::vector<float2> texcoords;
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

                indices.push_back(idxStart + 0);
                indices.push_back(idxStart + 1);
                indices.push_back(idxStart + 2);

                indices.push_back(idxStart + 2);
                indices.push_back(idxStart + 3);
                indices.push_back(idxStart + 0);
            }

            StrokeJoin join = properties.strokeJoin;
            if (join == StrokeJoin::miter && curr.miterLimitExceeded)
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
                    const size_t numSegments = 8;
                    const float angleStepSize = sweepAngle / static_cast<float>(numSegments);
                    const float radius = properties.strokeWeight * 0.5f;

                    const size_t idxStart = positions.size();
                    positions.push_back(center);
                    const size_t centerIdx = idxStart;

                    for (size_t j = 0; j <= numSegments; ++j)
                    {
                        const float angle = startAngle + angleStepSize * static_cast<float>(j);
                        const float2 arcPoint = {
                            center.x + std::cos(angle) * radius,
                            center.y + std::sin(angle) * radius,
                        };

                        positions.push_back(arcPoint);
                    }

                    for (size_t j = 0; j < numSegments; ++j)
                    {
                        indices.push_back(centerIdx);
                        indices.push_back(idxStart + 1 + j);
                        indices.push_back(idxStart + 1 + j + 1);
                    }

                    positions.push_back(curr.interInner);
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
            .positions = std::move(positions),
            .texcoords = std::move(texcoords),
            .indices = std::move(indices),
        };
    }
} // namespace processing

namespace processing
{
    static constexpr float4 float4_from_color(color_t color)
    {
        return float4{
            .x = static_cast<float>(red(color)) / 255.0f,
            .y = static_cast<float>(green(color)) / 255.0f,
            .z = static_cast<float>(blue(color)) / 255.0f,
            .w = static_cast<float>(alpha(color)) / 255.0f,
        };
    }

    Shape shape_from_contour(const Contour& contour, const matrix4x4& transform, color_t color, float depth)
    {
        assert(contour.positions.size() == contour.texcoords.size() and "Contour must contain the same number of positions as texcoords");

        Shape shape;
        shape.vertices.reserve(contour.positions.size());
        shape.indices.append_range(contour.indices);

        for (size_t i = 0; i < contour.positions.size(); ++i)
        {
            shape.vertices.push_back(Vertex{
                .position = matrix4x4_transform_point(transform, float3(contour.positions[i], depth)),
                .texcoord = contour.texcoords[i],
                .color = float4_from_color(color),
            });
        }

        return shape;
    }
} // namespace processing

namespace processing
{
    RectPath path_rect(const rect2f& boundary)
    {
        const float left = boundary.left;
        const float top = boundary.top;
        const float right = boundary.right();
        const float bottom = boundary.bottom();

        return RectPath{
            .boundary = boundary,
            .points = {
                {left, top},
                {right, top},
                {right, bottom},
                {left, bottom},
            }
        };
    }

    Contour contour_rect_fill(const RectPath& path)
    {
        return {
            .positions = path.points,
            .texcoords = {
                {0.0f, 0.0f},
                {1.0f, 0.0f},
                {1.0f, 1.0f},
                {0.0f, 1.0f},
            },
            .indices = {0, 1, 2, 2, 3, 0}
        };
    }

    Contour contour_rect_stroke(const RectPath& path, const StrokeProperties& properties)
    {
        return contour_stroke_from_path(path.points, properties);
    }
} // namespace processing

namespace processing
{
    EllipsePath path_ellipse(const EllipseSpecification& specification)
    {
        if (specification.segments < 3)
        {
            return EllipsePath{
                .specification = {},
                .points = {},
            };
        }

        std::vector<float2> points;

        for (size_t i = 0; i < specification.segments; ++i)
        {
            const float angle = 2.0f * PI * i / specification.segments;
            const float x = specification.center.x + std::cos(angle) * specification.radius.x;
            const float y = specification.center.y + std::sin(angle) * specification.radius.y;

            points.push_back({x, y});
        }

        return EllipsePath{
            .specification = specification,
            .points = std::move(points)
        };
    }

    Contour contour_ellipse_fill(const EllipsePath& path)
    {
        const float left = path.specification.center.x - path.specification.radius.x;
        const float right = path.specification.center.x + path.specification.radius.x;
        const float top = path.specification.center.y - path.specification.radius.y;
        const float bottom = path.specification.center.y + path.specification.radius.y;
        const float width = right - left;
        const float height = bottom - top;

        Contour c;

        c.positions.emplace_back(path.specification.center);
        c.texcoords.emplace_back(0.5f, 0.5f);

        for (size_t i = 0; i < path.points.size(); ++i)
        {
            const float2& point = path.points[i];
            c.positions.emplace_back(point);

            const float tx = (point.x - left) / width;
            const float ty = (point.y - top) / height;
            c.texcoords.emplace_back(tx, ty);
        }

        for (size_t i = 1; i <= path.points.size(); ++i)
        {
            c.indices.push_back(0);
            c.indices.push_back(i);
            c.indices.push_back(i < path.points.size() ? i + 1 : 1);
        }

        return c;
    }

    Contour contour_ellipse_stroke(const EllipsePath& path, const StrokeProperties& properties)
    {
        return contour_stroke_from_path(path.points, properties);
    }
} // namespace processing

namespace processing
{
    TrianglePath path_triangle(const TriangleSpecification& specification)
    {
        return {
            .specification = specification,
            .points = {
                specification.a,
                specification.b,
                specification.c
            },
        };
    }

    Contour contour_triangle_fill(const TrianglePath& path)
    {
        Contour c;

        c.positions = path.points;

        float minX = path.points[0].x;
        float minY = path.points[0].y;
        float maxX = path.points[0].x;
        float maxY = path.points[0].y;

        for (size_t i = 1; i < path.points.size(); ++i)
        {
            minX = std::min(minX, path.points[i].x);
            minY = std::min(minY, path.points[i].y);
            maxX = std::max(maxX, path.points[i].x);
            maxY = std::max(maxY, path.points[i].y);
        }

        const float width = maxX - minX;
        const float height = maxY - minY;

        c.texcoords.reserve(path.points.size());
        for (size_t i = 0; i < path.points.size(); ++i)
        {
            const float tx = (path.points[i].x - minX) / width;
            const float ty = (path.points[i].y - minY) / height;
            c.texcoords.emplace_back(tx, ty);
        }

        c.indices = {0, 1, 2};

        return c;
    }

    Contour contour_triangle_stroke(const TrianglePath& path, const StrokeProperties& properties)
    {
        return contour_stroke_from_path(path.points, properties);
    }
} // namespace processing

namespace processing
{
    RoundedRectPath path_rounded_rect(const RoundedRectSpecification& roundedRect)
    {
        constexpr size_t SEGMENTS = 8;

        const float right = roundedRect.boundary.right();
        const float bottom = roundedRect.boundary.bottom();

        const auto clamp_radius = [&](const Radius& radius)
        {
            const float x = std::max(0.0f, std::min(radius.x, roundedRect.boundary.width * 0.5f));
            const float y = std::max(0.0f, std::min(radius.y, roundedRect.boundary.height * 0.5f));
            return Radius{x, y};
        };

        const Radius topLeft = clamp_radius(roundedRect.topLeft);
        const Radius topRight = clamp_radius(roundedRect.topRight);
        const Radius bottomRight = clamp_radius(roundedRect.bottomRight);
        const Radius bottomLeft = clamp_radius(roundedRect.bottomLeft);

        const auto append_arc_or_corner = [&](std::vector<float2>& path, float cx, float cy, const Radius& radius, float cornerX, float cornerY, float startAngle, float endAngle)
        {
            if (radius.x <= 0.0f || radius.y <= 0.0f)
            {
                path.emplace_back(cornerX, cornerY);
                return;
            }

            for (size_t i = 0; i <= SEGMENTS; ++i)
            {
                float t = static_cast<float>(i) / SEGMENTS;
                float a = startAngle + (endAngle - startAngle) * t;

                path.emplace_back(
                    cx + std::cos(a) * radius.x,
                    cy + std::sin(a) * radius.y
                );
            }
        };

        std::vector<float2> path;
        path.reserve(SEGMENTS * 4 + 4);

        append_arc_or_corner(
            path,
            roundedRect.boundary.left + topLeft.x,
            roundedRect.boundary.top + topLeft.y,
            topLeft,
            roundedRect.boundary.left, roundedRect.boundary.top,
            PI, PI * 1.5f
        );

        append_arc_or_corner(
            path,
            right - topRight.x,
            roundedRect.boundary.top + topRight.y,
            topRight,
            right, roundedRect.boundary.top,
            PI * 1.5f, PI * 2.0f
        );

        append_arc_or_corner(
            path,
            right - bottomRight.x,
            bottom - bottomRight.y,
            bottomRight,
            right, bottom,
            0.0f, PI * 0.5f
        );

        append_arc_or_corner(
            path,
            roundedRect.boundary.left + bottomLeft.x,
            bottom - bottomLeft.y,
            bottomLeft,
            roundedRect.boundary.left, bottom,
            PI * 0.5f, PI
        );

        return RoundedRectPath{
            .specification = roundedRect,
            .points = std::move(path),
        };
    }

    Contour contour_rounded_rect_fill(const RoundedRectPath& path)
    {
        const auto [roundedRect, positions] = path;
        Contour c;

        const float2 center = roundedRect.boundary.center();
        c.positions.push_back(center);
        c.texcoords.push_back({0.5f, 0.5f});

        for (const float2& p : positions)
        {
            c.positions.push_back(p);

            const float tx = (p.x - roundedRect.boundary.left) / roundedRect.boundary.width;
            const float ty = (p.y - roundedRect.boundary.top) / roundedRect.boundary.height;
            c.texcoords.emplace_back(tx, ty);
        }

        const size_t ringStart = 1;
        const size_t ringCount = c.positions.size() - 1;
        for (size_t i = 0; i < ringCount; ++i)
        {
            const size_t a = ringStart + i;
            const size_t b = ringStart + (i + 1) % ringCount;
            c.indices.push_back(0);
            c.indices.push_back(static_cast<uint32_t>(a));
            c.indices.push_back(static_cast<uint32_t>(b));
        }

        return c;
    }

    Contour contour_rounded_rect_stroke(const RoundedRectPath& path, const StrokeProperties& properties)
    {
        return contour_stroke_from_path(path.points, properties);
    }
} // namespace processing

namespace processing
{
    Contour contour_quad_fill(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
    {
        return {
            .positions = {
                {x1, y1},
                {x2, y2},
                {x3, y3},
                {x4, y4},
            },
            .texcoords = {
                {0.0f, 0.0f},
                {1.0f, 0.0f},
                {1.0f, 1.0f},
                {0.0f, 1.0f},
            },
            .indices = {0, 1, 2, 2, 3, 0}
        };
    }

    Contour contour_quad_stroke(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, const StrokeProperties& properties)
    {
        const std::vector<float2> positions = {
            {x1, y1},
            {x2, y2},
            {x3, y3},
            {x4, y4},
        };

        return contour_stroke_from_path(positions, properties);
    }

    inline float2 value2_rotated(float2 v, float angle)
    {
        const float c = std::cos(angle);
        const float s = std::sin(angle);

        return float2{
            v.x * c - v.y * s,
            v.x * s + v.y * c
        };
    }

    Contour contour_line(float x1, float y1, float x2, float y2, float strokeWeight, StrokeCap strokeCap)
    {
        const float2 start = {x1, y1};
        const float2 end = {x2, y2};
        const float2 direction = value2_normalized(end - start);
        const float2 offset = value2_perpendicular_cw(direction) * strokeWeight * 0.5f;

        const float circumference = PI * strokeWeight;
        const size_t segments = std::max(4, static_cast<int>(circumference / 4.0f));

        std::vector<float2> positions;
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
                    float2 dir = value2_rotated(offset, angle);
                    positions.emplace_back(start - dir);
                }
                break;
            }
        }

        const size_t startPositionsCount = positions.size();

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
                    float2 dir = value2_rotated(offset, PI - angle);
                    positions.emplace_back(end + dir);
                }
                break;
            }
        }

        const size_t endPositionsCount = positions.size() - startPositionsCount;
        const size_t maxCount = std::max(startPositionsCount, endPositionsCount);

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

        return Contour{
            .positions = std::move(positions),
            .texcoords = std::move(texcoords),
            .indices = std::move(indices)
        };
    }

    Contour contour_image(float left, float top, float width, float height, float sourceLeft, float sourceTop, float sourceWidth, float sourceHeight)
    {
        Contour contour;

        {
            const float right = left + width;
            const float bottom = top + height;

            contour.positions = {
                {left, top},
                {right, top},
                {right, bottom},
                {left, bottom},
            };
        }

        {
            const float sourceRight = sourceLeft + sourceWidth;
            const float sourceBottom = sourceTop + sourceHeight;

            contour.texcoords = {
                {sourceLeft, sourceTop},
                {sourceRight, sourceTop},
                {sourceRight, sourceBottom},
                {sourceLeft, sourceBottom},
            };
        }

        contour.indices = {0, 1, 2, 2, 3, 0};

        return contour;
    }
} // namespace processing
