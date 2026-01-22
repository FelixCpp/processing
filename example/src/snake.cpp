#include <processing/processing.hpp>
using namespace processing;

#include <vector>
#include <chrono>
#include <random>
#include <unordered_set>

struct SnakeGame : Sketch
{
    inline static constexpr uint32_t COLUMNS = 20;
    inline static constexpr uint32_t ROWS = 20;

    inline static constexpr int32_t RIGHT = 0;
    inline static constexpr int32_t DOWN = 1;
    inline static constexpr int32_t LEFT = 2;
    inline static constexpr int32_t UP = 3;

    Texture backgroundTexture;
    Texture snakeTexture;
    Texture foodTexture;

    std::vector<int2> snake;
    std::vector<int2> foodSpots;
    int direction;

    std::chrono::steady_clock::time_point lastUpdate;

    void setup() override
    {
        setWindowSize(1000, 1000);

        backgroundTexture = loadTexture("images/white.png");
        snakeTexture = loadTexture("images/red.png");
        foodTexture = loadTexture("images/green.png");

        snake.resize(4);
        for (size_t i = 0; i < 10; ++i)
            foodSpots.push_back(getFoodPosition());
        direction = RIGHT;
        lastUpdate = std::chrono::steady_clock::now();
    }

    void event(const Event& event) override
    {
        if (event.type == Event::key_pressed)
        {
            if (event.key.code == KeyCode::right) direction = RIGHT;
            if (event.key.code == KeyCode::down) direction = DOWN;
            if (event.key.code == KeyCode::left) direction = LEFT;
            if (event.key.code == KeyCode::up) direction = UP;
        }
    }

    void draw() override
    {
        const float2 size = getViewport().size;
        const float cellWidth = size.x / static_cast<float>(COLUMNS);
        const float cellHeight = size.y / static_cast<float>(ROWS);

        const auto now = std::chrono::steady_clock::now();
        const auto diff = now - lastUpdate;
        if (diff > std::chrono::milliseconds{100})
        {
            for (size_t i = snake.size() - 1; i > 0; --i)
            {
                snake[i] = snake[i - 1];
            }

            int2& head = snake[0];
            if (direction == RIGHT) ++head.x;
            if (direction == DOWN) ++head.y;
            if (direction == LEFT) --head.x;
            if (direction == UP) --head.y;

            if (head.x < 0) head.x = COLUMNS - 1;
            if (head.x > COLUMNS - 1) head.x = 0;
            if (head.y < 0) head.y = ROWS - 1;
            if (head.y > ROWS - 1) head.y = 0;

            for (int2& food : foodSpots)
            {
                if (head.x == food.x and head.y == food.y)
                {
                    snake.push_back(snake.back());
                    food = getFoodPosition();
                }
            }

            lastUpdate = now;
        }

        // Draw background
        blendMode(BlendMode::screen);
        imageTint(51);
        for (uint32_t row = 0; row < ROWS; ++row)
        {
            for (uint32_t column = 0; column < COLUMNS; ++column)
            {
                const float px = static_cast<float>(column) * cellWidth;
                const float py = static_cast<float>(row) * cellHeight;
                image(backgroundTexture, px, py, cellWidth, cellHeight);
            }
        }

        // Draw food
        imageTint(0, 255, 0, 100);
        for (size_t i = 0; i < foodSpots.size(); ++i)
        {
            const float px = static_cast<float>(foodSpots[i].x) * cellWidth;
            const float py = static_cast<float>(foodSpots[i].y) * cellHeight;
            image(backgroundTexture, px, py, cellWidth, cellHeight);
        }

        // Draw snake
        imageTint(255, 0, 0, 100);
        for (size_t i = 0; i < snake.size(); ++i)
        {
            const float px = static_cast<float>(snake[i].x) * cellWidth;
            const float py = static_cast<float>(snake[i].y) * cellHeight;
            image(backgroundTexture, px, py, cellWidth, cellHeight);
        }
    }

    void image(const Texture& texture, float x1, float y1, float x2, float y2)
    {
        const auto size = float2{texture.getSize()};
        processing::image(texture, x1, y1, x2, y2, 0.0f, 0.0f, size.x, size.y);
    }

    int2 getFoodPosition() const
    {
        auto posHash = [](const int2& p)
        {
            return p.x * COLUMNS + p.y;
        };

        auto posEqual = [](const int2& lhs, const int2& rhs)
        {
            return lhs.x == rhs.x and lhs.y == rhs.y;
        };

        std::unordered_set<int2, decltype(posHash), decltype(posEqual)> snakeSet(10, posHash, posEqual);
        snakeSet.insert(snake.begin(), snake.end());
        snakeSet.insert(foodSpots.begin(), foodSpots.end());

        std::vector<int2> freeSpots;
        for (uint32_t row = 0; row < ROWS; ++row)
        {
            for (uint32_t column = 0; column < COLUMNS; ++column)
            {
                int2 pos = {static_cast<int32_t>(column), static_cast<int32_t>(row)};
                if (snakeSet.count(pos) == 0) freeSpots.push_back(pos);
            }
        }

        if (freeSpots.empty())
            return {0, 0};

        static std::mt19937 engine(std::random_device{}());
        return freeSpots[std::uniform_int_distribution<size_t>(0, freeSpots.size() - 1)(engine)];
    }

    virtual void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<SnakeGame>();
}
