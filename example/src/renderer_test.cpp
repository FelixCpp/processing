#include <processing/processing.hpp>
using namespace processing;

struct Coin
{
    Texture coinTexture;

    int animationColumns;
    int animationRows;
    int animationX;
    int animationY;
    float animationTime;
    float animationFrameTime;

    float positionX;
    float positionY;

    float frameWidth;
    float frameHeight;

    float bounceTime;
    float bounceOffset;

    explicit Coin(Texture coinTexture, int animationColumns, int animationRows, float animationFrameTime, float positionX, float positionY)
        : coinTexture(coinTexture),
          animationColumns(animationColumns),
          animationRows(animationRows),
          animationX(0),
          animationY(0),
          animationTime(0.0f),
          animationFrameTime(animationFrameTime),
          positionX(positionX),
          positionY(positionY),
          frameWidth(static_cast<float>(coinTexture.getSize().x) / static_cast<float>(animationColumns)),
          frameHeight(static_cast<float>(coinTexture.getSize().y) / static_cast<float>(animationRows)),
          bounceTime(0.0f),
          bounceOffset(0.0f)
    {
    }

    void update(const float deltaTime)
    {
        animationTime += deltaTime;

        while (animationTime >= animationFrameTime)
        {
            if (++animationX >= animationColumns)
            {
                if (++animationY >= animationRows)
                {
                    animationY -= animationRows;
                }

                animationX -= animationColumns;
            }

            animationTime -= animationFrameTime;
        }
    }

    void bounce(const float deltaTime, float bounceHeight = 50.0f)
    {
        bounceTime += deltaTime;
        bounceOffset = -(std::sin(bounceTime) * 0.5f + 0.5f) * bounceHeight;
    }

    void show()
    {
        const float srcX = static_cast<float>(animationX) * frameWidth;
        const float srcY = static_cast<float>(animationY) * frameHeight;
        image(coinTexture, positionX, positionY + bounceOffset, frameWidth, frameHeight, srcX, srcY, frameWidth, frameHeight);
    }
};

struct Block
{
    Texture blocks;

    int sourceLeft;
    int sourceTop;
    int sourceWidth;
    int sourceHeight;

    float positionX;
    float positionY;

    float width;
    float height;

    explicit Block(const Texture& blocks, int sourceLeft, int sourceTop, int sourceWidth, int sourceHeight, float positionX, float positionY, float width, float height)
        : blocks(blocks),
          sourceLeft(sourceLeft),
          sourceTop(sourceTop),
          sourceWidth(sourceWidth),
          sourceHeight(sourceHeight),
          positionX(positionX),
          positionY(positionY),
          width(width),
          height(height)
    {
    }

    void show()
    {
        image(blocks, positionX, positionY, width, height, sourceLeft, sourceTop, sourceWidth, sourceHeight);
    }
};

struct RendererTest : Sketch
{
    Coin coin = Coin(loadTexture("assets/platformer/sprites/coin.png"), 12, 1, 0.1f, 100.0f, 100.0f);
    Texture blocks = loadTexture("assets/platformer/sprites/world_tileset.png");
    Block grassBlock = Block(blocks, 0, 0, 16, 16, 0.0f, 0.0f, 32.0f, 32.0f);
    Block dirtBlock = Block(blocks, 0, 16, 16, 16, 0.0f, 32.0f, 32.0f, 32.0f);

    std::chrono::steady_clock::time_point start;

    void setup() override
    {
        setWindowSize(900, 900);
        start = std::chrono::steady_clock::now();
    }

    void event(const Event& event) override
    {
    }

    void draw() override
    {
        const auto now = std::chrono::steady_clock::now();
        const auto elapsed = std::chrono::duration<float>(now - start);
        const float deltaTime = elapsed.count();
        start = now;

        coin.bounce(deltaTime);
        coin.update(deltaTime);

        background(21);
        coin.show();
        grassBlock.show();
        dirtBlock.show();
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<RendererTest>();
}
