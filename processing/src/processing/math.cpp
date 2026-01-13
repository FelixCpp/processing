#include <processing/processing.hpp>

namespace processing
{
    // clang-format off
    matrix4x4 create_matrix(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    )
    {
        return matrix4x4 {
            .data = {
                m00, m01, m02, m03,
                m10, m11, m12, m13,
                m20, m21, m22, m23,
                m30, m31, m32, m33,
            }
        };
    }

    matrix4x4 translate(float x, float y, float z)
    {
        return create_matrix(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            x, y, z, 1.0f
        );
    }

    matrix4x4 scale(float x, float y, float z) {
        return create_matrix(
            x, 0.0f, 0.0f, 0.0f,
            0.0f, y, 0.0f, 0.0f,
            0.0f, 0.0f, z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    matrix4x4 orthographic(float left, float top, float width, float height, float near, float far) {
        const float right = left + width;
        const float bottom = top + height;

        return create_matrix(
            2.0f / (right - left),  0.0f,                      0.0f,                      0.0f,
            0.0f,                   2.0f / (top - bottom),    0.0f,                      0.0f,
            0.0f,                   0.0f,                     -2.0f / (far - near),      0.0f,
           -(right + left) / (right - left),
           -(top + bottom) / (top - bottom),
           -(far + near) / (far - near),
            1.0f
        );
    }

    float3 transformPoint(const matrix4x4 &matrix, const float3 &point) {
        return {
            matrix.data[0] * point.x + matrix.data[4] * point.y + matrix.data[8] * point.z + matrix.data[12],
            matrix.data[1] * point.x + matrix.data[5] * point.y + matrix.data[9] * point.z + matrix.data[13],
            matrix.data[2] * point.x + matrix.data[6] * point.y + matrix.data[10] * point.z + matrix.data[14],
        };
    }

    float2 transformVector(const matrix4x4 &matrix, const float2 &vector) {
        return {
            matrix.data[0] * vector.x + matrix.data[4] * vector.y,
            matrix.data[1] * vector.x + matrix.data[5] * vector.y,
        };
    }
    // clang-format on
} // namespace processing
