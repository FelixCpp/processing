#ifndef _PROCESSING_INCLUDE_MATRIX_STACK_HPP_
#define _PROCESSING_INCLUDE_MATRIX_STACK_HPP_

#include <processing/processing.hpp>
#include <array>

namespace processing
{
    struct MatrixStack
    {
        std::array<matrix4x4, 64> metrics;
        size_t currentIndex;
    };

    MatrixStack matrix_stack_create();
    void matrix_stack_push(MatrixStack& stack, const matrix4x4& matrix);
    void matrix_stack_pop(MatrixStack& stack);
    void matrix_stack_reset(MatrixStack& stack);
    matrix4x4& matrix_stack_peek(MatrixStack& stack);
} // namespace processing

#endif // _PROCESSING_INCLUDE_MATRIX_STACK_HPP_
