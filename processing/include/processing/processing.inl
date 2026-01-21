// #ifndef _PROCESSING_INCLUDE_PROCESSING_INL_
// #define _PROCESSING_INCLUDE_PROCESSING_INL_
//
// namespace processing
// {
//     // clang-format off
//     template <typename T> constexpr value2<T>::value2() : x(T{}), y(T{}) { }
//     template <typename T> constexpr value2<T>::value2(T x, T y) : x(x), y(y) { }
//     template <typename T> constexpr value2<T>::value2(T scalar) : x(scalar), y(scalar) { }
//     template <typename T> template <typename U> constexpr value2<T>::value2(const value2<U>& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) { }
//
//     template <typename T>
//     T value2_length(const value2<T>& value)
//     {
//         return std::hypot(value.x, value.y);
//     }
//
//     template <typename T>
//     constexpr T value2_dot(const value2<T>& lhs, const value2<T>& rhs)
//     {
//         return lhs.x * rhs.x + lhs.y * rhs.y;
//     }
//
//     template <typename T>
//     constexpr T value2_length_squared(const value2<T>& value)
//     {
//         return value.x * value.x + value.y * value.y;
//     }
//
//     template <typename T>
//     constexpr value2<T> value2_perpendicular(const value2<T>& value)
//     {
//         return { value.y, -value.x };
//     }
//
//     template <typename T>
//     value2<T> value2_normalized(const value2<T>& value)
//     {
//         const T length = value2_length(value);
//         if (length != static_cast<T>(0)) {
//             return { static_cast<T>(value.x / length), static_cast<T>(value.y / length) };
//         }
//         return value;
//     }
//
//     template <typename T>
//     void value2_normalize(value2<T>& value)
//     {
//         return value = value2_normalized(value);
//     }
//
//     template <typename T> constexpr value2<T> operator+(const value2<T>& lhs, const value2<T>& rhs) { return { lhs.x + rhs.x, lhs.y + rhs.y }; }
//     template <typename T> constexpr value2<T> operator-(const value2<T>& lhs, const value2<T>& rhs) { return { lhs.x - rhs.x, lhs.y - rhs.y }; }
//     template <typename T> constexpr value2<T> operator*(const value2<T>& lhs, const value2<T>& rhs) { return { lhs.x * rhs.x, lhs.y * rhs.y }; }
//     template <typename T> constexpr value2<T> operator/(const value2<T>& lhs, const value2<T>& rhs) { return { lhs.x / rhs.x, lhs.y / rhs.y }; }
//
//     template <typename T> constexpr value2<T> operator+(const value2<T>& lhs, const T rhs) { return { lhs.x + rhs, lhs.y + rhs }; }
//     template <typename T> constexpr value2<T> operator-(const value2<T>& lhs, const T rhs) { return { lhs.x - rhs, lhs.y - rhs }; }
//     template <typename T> constexpr value2<T> operator*(const value2<T>& lhs, const T rhs) { return { lhs.x * rhs, lhs.y * rhs }; }
//     template <typename T> constexpr value2<T> operator/(const value2<T>& lhs, const T rhs) { return { lhs.x / rhs, lhs.y / rhs }; }
//     // clang-format on
// } // namespace processing
//
// namespace processing
// {
//     // clang-format off
//     template <typename T> constexpr value3<T>::value3() : x(T{}), y(T{}), z(T{}) { }
//     template <typename T> constexpr value3<T>::value3(const T x, const T y, const T z) : x(x), y(y), z(z) { }
//     template <typename T> constexpr value3<T>::value3(T scalar) : x(scalar), y(scalar), z(scalar) { }
//     template <typename T> constexpr value3<T>::value3(const value2<T>& xy, const T z) : x(xy.x), y(xy.y), z(z) {}
//     template <typename T> constexpr value3<T>::value3(const T x, const value2<T>& yz) : x(x), y(yz.x), z(yz.y) {}
//     // clang-format on
// } // namespace processing
//
// namespace processing
// {
//     // clang-format off
//     constexpr matrix4x4 matrix4x4_create(
//         float m00, float m01, float m02, float m03,
//         float m10, float m11, float m12, float m13,
//         float m20, float m21, float m22, float m23,
//         float m30, float m31, float m32, float m33
//     )
//     {
//         return matrix4x4 {
//             .data = {
//                 m00, m01, m02, m03,
//                 m10, m11, m12, m13,
//                 m20, m21, m22, m23,
//                 m30, m31, m32, m33,
//             }
//         };
//     }
//
//     constexpr matrix4x4 matrix4x4_identity()
//     {
//         return matrix4x4_create(
//             1.0f, 0.0f, 0.0f, 0.0f,
//             0.0f, 1.0f, 0.0f, 0.0f,
//             0.0f, 0.0f, 1.0f, 0.0f,
//             0.0f, 0.0f, 0.0f, 1.0f
//         );
//     }
//
//     constexpr matrix4x4 matrix4x4_translate(float x, float y, float z)
//     {
//         return matrix4x4_create(
//             1.0f, 0.0f, 0.0f, 0.0f,
//             0.0f, 1.0f, 0.0f, 0.0f,
//             0.0f, 0.0f, 1.0f, 0.0f,
//             x, y, z, 1.0f
//         );
//     }
//
//     constexpr matrix4x4 matrix4x4_scale(float x, float y, float z) {
//         return matrix4x4_create(
//             x, 0.0f, 0.0f, 0.0f,
//             0.0f, y, 0.0f, 0.0f,
//             0.0f, 0.0f, z, 0.0f,
//             0.0f, 0.0f, 0.0f, 1.0f
//         );
//     }
//
//     constexpr matrix4x4 matrix4x4_orthographic(float left, float top, float width, float height, float near, float far) {
//         const float right = left + width;
//         const float bottom = top + height;
//
//         return matrix4x4_create(
//             2.0f / (right - left),  0.0f,                      0.0f,                      0.0f,
//             0.0f,                   2.0f / (top - bottom),    0.0f,                      0.0f,
//             0.0f,                   0.0f,                     -2.0f / (far - near),      0.0f,
//            -(right + left) / (right - left),
//            -(top + bottom) / (top - bottom),
//            -(far + near) / (far - near),
//             1.0f
//         );
//     }
//
//     constexpr float3 transformPoint(const matrix4x4 &matrix, const float3 &point) {
//         return {
//             matrix.data[0] * point.x + matrix.data[4] * point.y + matrix.data[8] * point.z + matrix.data[12],
//             matrix.data[1] * point.x + matrix.data[5] * point.y + matrix.data[9] * point.z + matrix.data[13],
//             matrix.data[2] * point.x + matrix.data[6] * point.y + matrix.data[10] * point.z + matrix.data[14],
//         };
//     }
//
//     constexpr float2 transformVector(const matrix4x4 &matrix, const float2 &vector) {
//         return {
//             matrix.data[0] * vector.x + matrix.data[4] * vector.y,
//             matrix.data[1] * vector.x + matrix.data[5] * vector.y,
//         };
//     }
//     // clang-format on
// } // namespace processing
//
// namespace processing
// {
//     // clang-format off
//     template <typename T> constexpr rect2<T>::rect2() : left(T{}), top(T{}), width(T{}), height(T{}) {}
//     template <typename T> constexpr rect2<T>::rect2(T left, T top, T width, T height) : left(left), top(top), width(width), height(height) {}
//     template <typename T> template <typename U> constexpr rect2<T>::rect2(const rect2<U>& other): left(static_cast<T>(other.left)), top(static_cast<T>(other.top)), width(static_cast<T>(other.width)), height(static_cast<T>(other.height)) {}
//     template <typename T> constexpr value2<T> rect2<T>::center() const { return { left + width / 2, top + height / 2 }; }
//     template <typename T> constexpr T rect2<T>::right() const { return left + width; }
//     template <typename T> constexpr T rect2<T>::bottom() const { return top + height; }
//     template <typename T> constexpr bool operator == (const rect2<T>& lhs, const rect2<T>& rhs) { return lhs.left == rhs.left and lhs.top == rhs.top and lhs.width == rhs.width and lhs.height == rhs.height; }
//     template <typename T> constexpr bool operator != (const rect2<T>& lhs, const rect2<T>& rhs) { return lhs.left != rhs.left or lhs.top != rhs.top or lhs.width != rhs.width or lhs.height != rhs.height; }
//     // clang-format on
// } // namespace processing
//
// namespace processing
// {
//     // clang-format off
//     constexpr bool operator == (const ShaderProgramId& lhs, const ShaderProgramId& rhs) { return lhs.value == rhs.value; }
//     constexpr bool operator != (const ShaderProgramId& lhs, const ShaderProgramId& rhs) { return lhs.value != rhs.value; }
//     // clang-format on
// } // namespace processing
//
// namespace processing
// {
//     // clang-format off
//     constexpr bool operator == (const TextureId& lhs, const TextureId& rhs) { return lhs.value == rhs.value; }
//     constexpr bool operator != (const TextureId& lhs, const TextureId& rhs) { return lhs.value != rhs.value; }
//     // clang-format on
// } // namespace processing
//
// namespace processing
// {
//     // clang-format off
//     constexpr color_t color(int32_t red, int32_t green, int32_t blue, int32_t alpha) { return color_t{ .value = (uint32_t)(red << 24) | (uint32_t)(green << 16) | (uint32_t)(blue << 8) | (uint32_t)alpha }; }
//     constexpr color_t color(int32_t grey, int32_t alpha) { return color(grey, grey, grey, alpha); }
//     constexpr int32_t red(color_t color) { return (color.value & 0xFF000000) >> 24; }
//     constexpr int32_t green(color_t color) { return (color.value & 0x00FF0000) >> 16; }
//     constexpr int32_t blue(color_t color) { return (color.value & 0x0000FF00) >> 8; }
//     constexpr int32_t alpha(color_t color) { return (color.value & 0x000000FF); }
//     // clang-format on
// } // namespace processing
//
// namespace processing
// {
//     inline constexpr StrokeCap StrokeCap::butt = {.start = StrokeCapStyle::butt, .end = StrokeCapStyle::butt};
//     inline constexpr StrokeCap StrokeCap::square = {.start = StrokeCapStyle::square, .end = StrokeCapStyle::square};
//     inline constexpr StrokeCap StrokeCap::round = {.start = StrokeCapStyle::round, .end = StrokeCapStyle::round};
// } // namespace processing
//
// namespace processing
// {
//     constexpr BlendMode::BlendMode(Factor sourceFactor, Factor destinationFactor, Equation blendEquation) :
//         : colorSourceFactor(sourceFactor), colorDestinationFactor(destinationFactor), colorBlendEquation(blendEquation), alphaSourceFactor(sourceFactor), alphaDestinationFactor(destinationFactor), alphaBlendEquation(blendEquation)
//     {
//     }
//     constexpr BlendMode::BlendMode(Factor colorSourceFactor, Factor colorDestinationFactor, Equation colorBlendEquation, Factor alphaSourceFactor, Factor alphaDestinationFactor, Equation alphaBlendEquation)
//         : colorSourceFactor(colorSourceFactor), colorDestinationFactor(colorDestinationFactor), colorBlendEquation(colorBlendEquation), alphaSourceFactor(alphaSourceFactor), alphaDestinationFactor(alphaDestinationFactor), alphaBlendEquation(alphaBlendEquation)
//     {
//     }
//
//     inline constexpr BlendMode BlendMode::alpha{Factor::srcAlpha, Factor::oneMinusSrcAlpha};
//     inline constexpr BlendMode BlendMode::add{Factor::srcAlpha, Factor::one};
//     inline constexpr BlendMode BlendMode::multiply{Factor::dstColor, Factor::zero};
//     inline constexpr BlendMode BlendMode::opaque{Factor::one, Factor::zero};
//     inline constexpr BlendMode BlendMode::none{Factor::one, Factor::zero};
//     inline constexpr BlendMode BlendMode::additive{Factor::one, Factor::one};
//     inline constexpr BlendMode BlendMode::screen{Factor::one, Factor::oneMinusSrcColor};
//     inline constexpr BlendMode BlendMode::subtract{Factor::srcAlpha, Factor::one, Equation::reverseSubtract};
//     inline constexpr BlendMode BlendMode::premultipliedAlpha{Factor::one, Factor::oneMinusSrcAlpha};
// } // namespace processing
//
// #endif // _PROCESSING_INCLUDE_PROCESSING_INL_
