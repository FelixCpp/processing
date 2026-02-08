#include <processing/processing.hpp>
using namespace processing;

#include <algorithm>

struct DemoApp : Sketch
{
    Image img = loadImage("images/TestImage.png");

    void setup() override
    {
        img.setExtendMode(ExtendMode::clamp);
        img.setFilterMode(FilterMode::nearest);
    }

    void draw(f32 deltaTime) override
    {
        f32 imgWidth = img.getSize().x;
        f32 imgHeight = img.getSize().y;
        f32 mx = getMousePosition().x;
        f32 my = getMousePosition().y;
        f32 lenseWidth = 1000.0f;
        f32 lenseHeight = 1000.0f;

        f32 nmx = mx / 900.0f * imgWidth;
        f32 nmy = my / 900.0f * imgHeight;

        f32 sl = nmx - lenseWidth / 2.0f;
        f32 st = nmy - lenseHeight / 2.0f;

        sl = std::clamp(sl, 0.0f, imgWidth - lenseWidth);
        st = std::clamp(st, 0.0f, imgHeight - lenseHeight);

        fprintf(stdout, "Source: %.2f, %.2f\n", sl, st);
        fflush(stdout);

        background(21);
        imageSourceMode(ImageSourceMode::size);
        image(img, 0.0f, 0.0f, 800.0f, 800.0f, sl, st, lenseWidth, lenseHeight);
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<DemoApp>();
}
