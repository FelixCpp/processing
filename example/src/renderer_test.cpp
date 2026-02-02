#include <algorithm>
#include <processing/processing.hpp>
#include <random>
using namespace processing;

#include <vector>

struct RendererTest : Sketch
{
    Texture texture;
    std::vector<rect2i> tiles;

    int columns;
    int rows;
    int tileWidth;
    int tileHeight;

    void setup() override
    {
        texture = loadTexture("images/wallpaper.jpg");
        columns = 10;
        rows = 10;
        tileWidth = static_cast<int>(texture.getSize().x) / columns;
        tileHeight = static_cast<int>(texture.getSize().y) / rows;

        for (int y = 0; y < rows; ++y)
        {
            for (int x = 0; x < columns; ++x)
            {
                tiles.push_back(rect2i{
                    x * tileWidth,
                    y * tileHeight,
                    tileWidth,
                    tileHeight,
                });
            }
        }

        std::mt19937 generator{std::random_device{}()};
        std::ranges::shuffle(tiles, generator);
    }

    void event(const Event& event) override
    {
    }

    void draw() override
    {
        background(21);
        const float w = getViewport().width / static_cast<float>(columns);
        const float h = getViewport().height / static_cast<float>(rows);

        for (int y = 0; y < rows; ++y)
        {
            for (int x = 0; x < columns; ++x)
            {
                const rect2i& tile = tiles[y * columns + x];
                const float px = static_cast<float>(x) * w;
                const float py = static_cast<float>(y) * h;
                image(texture, px, py, w, h, tile.left, tile.top, tile.width, tile.height);
            }
        }
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<RendererTest>();
}
