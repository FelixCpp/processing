#include <processing/processing.hpp>
using namespace processing;

struct RendererTest : Sketch
{
    Texture buffer1;
    Texture buffer2;

    inline static constexpr int cols = 900;
    inline static constexpr int rows = 900;

    void setup() override
    {
        setWindowSize(900, 900);
        buffer1 = createTexture(cols, rows, nullptr);
        buffer2 = createTexture(cols, rows, nullptr);
    }

    void event(const Event& event) override
    {
    }

    void draw() override
    {
        buffer1.modifyPixels(
            [](PixelBuffer& buffer)
            {
                for (int x = 0; x < cols; ++x)
                {
                    buffer.set(x, rows - 1, color(255));
                    buffer.set(x, rows - 2, color(255));
                }
            }
        );

        buffer2.modifyPixels(
            [](PixelBuffer& buffer)
            {
                for (int x = 0; x < cols; ++x)
                {
                    buffer.set(x, rows - 1, color(255));
                    buffer.set(x, rows - 2, color(255));
                }
            }
        );

        background(0);

        buffer1.readPixels(
            [this](const PixelBuffer& buffer1)
            {
                buffer2.modifyPixels(
                    [this, &buffer1](PixelBuffer& buffer2)
                    {
                        for (int y = 0; y < rows; ++y)
                        {
                            for (int x = 0; x < cols; ++x)
                            {
                                const color_t left = buffer1.get(x - 1, y);
                                const color_t right = buffer1.get(x + 1, y);
                                const color_t up = buffer1.get(x, y - 1);
                                const color_t down = buffer1.get(x, y + 1);
                                const int32_t b = brightness(left) + brightness(right) + brightness(up) + brightness(down);

                                buffer2.set(x, y - 1, color(fmin(b * 0.25, 255)));
                            }
                        }
                    }
                );
            }
        );

        // std::swap(buffer1, buffer2);
        swapPixels(buffer1, buffer2);

        image(buffer2, 0.0f, 0.0f);
    }

    void swapPixels(Texture& a, Texture& b)
    {
        std::vector<color_t> a_data(cols * rows);
        a.readPixels(
            [&](const PixelBuffer& buffer)
            {
                for (int y = 0; y < rows; ++y)
                {
                    for (int x = 0; x < cols; ++x)
                    {
                        a_data[y * cols + x] = buffer.get(x, y);
                    }
                }
            }
        );

        std::vector<color_t> b_data(cols * rows);
        b.readPixels(
            [&](const PixelBuffer& buffer)
            {
                for (int y = 0; y < rows; ++y)
                {
                    for (int x = 0; x < cols; ++x)
                    {
                        b_data[y * cols + x] = buffer.get(x, y);
                    }
                }
            }
        );

        a.modifyPixels(
            [&](PixelBuffer& buffer)
            {
                for (int y = 0; y < rows; ++y)
                {
                    for (int x = 0; x < cols; ++x)
                    {
                        buffer.set(x, y, b_data[y * cols + x]);
                    }
                }
            }
        );

        b.modifyPixels(
            [&](PixelBuffer& buffer)
            {
                for (int y = 0; y < rows; ++y)
                {
                    for (int x = 0; x < cols; ++x)
                    {
                        buffer.set(x, y, a_data[y * cols + x]);
                    }
                }
            }
        );
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<RendererTest>();
}
