#include <processing/processing.hpp>
using namespace processing;

struct DemoApp : Sketch
{
    Image* img;
    Image wallpaper = loadImage("images/wallpaper.jpg");

    void setup() override
    {
        getGfx().background(51);
        getGfx().imageSourceMode(ImageSourceMode::normal);
        getGfx().image(wallpaper, 0.0f, 0.0f, wallpaper.getSize().x, wallpaper.getSize().y, 0.0f, 0.0f, 2.0f, 2.0f);
        img = &getGfx().getImage();

        auto pixels = img->loadPixels();
        for (int y = 0; y < img->getSize().y; ++y)
        {
            for (int x = 0; x < img->getSize().x; ++x)
            {
                const auto c = pixels.get(x, y);
                pixels.set(x, y, Color(255, 0, 0));
            }
        }
        pixels.commit();
    }

    void draw(f32 deltaTime) override
    {
        // // getGfx().background(51);
        // getGfx().image(img, 0.0f, 0.0f, 300.0f, 300.0f);
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<DemoApp>();
}
