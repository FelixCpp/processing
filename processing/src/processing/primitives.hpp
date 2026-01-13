#ifndef _PROCESSING_INCLUDE_PRIMITIVES_HPP_
#define _PROCESSING_INCLUDE_PRIMITIVES_HPP_

#include <processing/renderer.hpp>

#include <vector>

namespace processing
{
    struct Shape
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };
} // namespace processing

#endif // _PROCESSING_INCLUDE_PRIMITIVES_HPP_
