#include <processing/matrix_stack.hpp>

namespace processing
{
    MatrixStack matrix_stack_create()
    {
        return MatrixStack{
            .metrics = std::invoke(
                []()
                {
                    std::array<matrix4x4, 64> metrics;
                    metrics.fill(matrix4x4_identity());
                    return metrics;
                }
            ),
            .currentIndex = 0,
        };
    }

    void matrix_stack_push(MatrixStack& stack, const matrix4x4& matrix)
    {
        if (stack.currentIndex < stack.metrics.size() - 1)
        {
            ++stack.currentIndex;
            stack.metrics[stack.currentIndex] = matrix;
        }
        else
        {
            warning("Matrix-Stack exceeeded");
        }
    }

    void matrix_stack_pop(MatrixStack& stack)
    {
        if (stack.currentIndex > 0)
        {
            --stack.currentIndex;
        }
    }

    void matrix_stack_reset(MatrixStack& stack)
    {
        stack.currentIndex = 0;
        stack.metrics[stack.currentIndex] = matrix4x4_identity();
    }

    matrix4x4& matrix_stack_peek(MatrixStack& stack)
    {
        return stack.metrics.at(stack.currentIndex);
    }
} // namespace processing
