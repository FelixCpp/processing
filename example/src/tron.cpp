#include <chrono>
#include <processing/processing.hpp>
using namespace processing;

enum Direction
{
    left,
    right,
    up,
    down
};

static constexpr int WIDTH = 600;
static constexpr int HEIGHT = 480;

struct Player
{
    color_t color;
    Direction dir;
    int x;
    int y;

    Player(color_t col)
    {
        dir = (Direction)(rand() % 4);
        color = col;
        x = rand() % WIDTH;
        y = rand() % HEIGHT;
    }

    void tick()
    {
        if (dir == left) --x;
        if (dir == right) ++x;
        if (dir == up) --y;
        if (dir == down) ++y;

        if (x < 0) x = WIDTH - 1;
        if (y < 0) y = HEIGHT - 1;
        if (x >= WIDTH) x = 0;
        if (y >= HEIGHT) y = 0;
    }
};

inline static constexpr const char* VERTEX_SHADER = R"(
    #version 330 core

    layout (location = 0) in vec3 a_Position;
    layout (location = 1) in vec2 a_TexCoord;
    layout (location = 2) in vec4 a_Color;

    out vec2 v_TexCoord;
    out vec4 v_Color;

    uniform mat4 u_ProjectionMatrix;

    void main()
    {
        gl_Position = u_ProjectionMatrix * vec4(a_Position, 1.0);
        v_TexCoord = a_TexCoord;
        v_Color = a_Color;
    }
)";

inline static constexpr const char* FRAGMENT_SHADER = R"(
    #version 330 core

    layout (location = 0) out vec4 o_Color;

    in vec2 v_TexCoord;
    in vec4 v_Color;

    uniform vec2 frag_LightOrigin;
    uniform vec3 frag_LightColor;
    uniform float frag_LightAttenuation;
    uniform vec2 frag_ScreenResolution;

    uniform sampler2D u_TextureSampler;

    void main()
    {
        vec2 baseDistance =  gl_FragCoord.xy;
        baseDistance.y = frag_ScreenResolution.y-baseDistance.y;
        float d = length(frag_LightOrigin - baseDistance);
        float a = 1.0/(frag_LightAttenuation * d);
        vec4 color = vec4(a,a,a,1.0) * vec4(frag_LightColor, 1.0);
        vec4 t = texture(u_TextureSampler, v_TexCoord);
        if (t[0]>color[0]) color[0]=t[0];
        if (t[1]>color[1]) color[1]=t[1];
        if (t[2]>color[2]) color[2]=t[2];
        o_Color=color;
    }
)";

struct TronGame : Sketch
{
    Player p1 = Player(color(255, 0, 0));
    Player p2 = Player(color(0, 255, 0));

    Texture backgroundImage = loadTexture("images/background.jpg");
    Renderbuffer offscreen = createRenderbuffer(WIDTH, HEIGHT);
    Shader playerShader = loadShader(VERTEX_SHADER, FRAGMENT_SHADER);

    int field[HEIGHT][WIDTH] = {};
    bool over = false;
    float speedFactor = 1.0f;

    void setup() override
    {
        srand(time(nullptr));
        setWindowSize(WIDTH, HEIGHT);

        renderbuffer(offscreen);
        background(backgroundImage);
        noRenderbuffer();

        playerShader.uploadUniform("frag_ScreenResolution", WIDTH, HEIGHT);
        playerShader.uploadUniform("frag_LightAttenuation", 100.0f);
    }

    void event(const Event& event) override
    {
        if (event.type == Event::key_pressed)
        {
            if (event.key.code == KeyCode::w and p1.dir != down) p1.dir = up;
            if (event.key.code == KeyCode::a and p1.dir != right) p1.dir = left;
            if (event.key.code == KeyCode::d and p1.dir != left) p1.dir = right;
            if (event.key.code == KeyCode::s and p1.dir != up) p1.dir = down;

            if (event.key.code == KeyCode::up and p2.dir != down) p2.dir = up;
            if (event.key.code == KeyCode::left and p2.dir != right) p2.dir = left;
            if (event.key.code == KeyCode::right and p2.dir != left) p2.dir = right;
            if (event.key.code == KeyCode::down and p2.dir != up) p2.dir = down;

            if (event.key.code == KeyCode::space) restart();
        }
    }

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    void draw() override
    {
        const auto now = std::chrono::steady_clock::now();
        const float deltaTime = std::chrono::duration<float>(now - start).count();
        start = now;

        // speedFactor += 0.5f * deltaTime;
        int speed = 1;
        for (int i = 0; i < speed; ++i)
        {
            p1.tick();
            p2.tick();

            if (field[p1.y][p1.x] == 1) over = true;
            if (field[p2.y][p2.x] == 1) over = true;

            field[p1.y][p1.x] = 1;
            field[p2.y][p2.x] = 1;

            renderbuffer(offscreen);
            noStroke();
            fill(p1.color);
            circle(p1.x, p1.y, 3.0f);
            noRenderbuffer();

            renderbuffer(offscreen);
            shader(playerShader);
            playerShader.uploadUniform("frag_LightOrigin", p1.x, p1.y);
            playerShader.uploadUniform("frag_LightColor", red(p1.color), green(p1.color), blue(p1.color));
            image(offscreen.getTexture(), 0.0f, 0.0f);
            noShader();
            noRenderbuffer();

            renderbuffer(offscreen);
            noStroke();
            fill(p2.color);
            circle(p2.x, p2.y, 3.0f);
            noRenderbuffer();

            renderbuffer(offscreen);
            shader(playerShader);
            playerShader.uploadUniform("frag_LightOrigin", p2.x, p2.y);
            playerShader.uploadUniform("frag_LightColor", red(p2.color), green(p2.color), blue(p2.color));
            image(offscreen.getTexture(), 0.0f, 0.0f);
            noShader();
            noRenderbuffer();
        }

        background(offscreen.getTexture());
        if (over) noLoop();
    }

    void destroy() override
    {
    }
};

std::unique_ptr<Sketch> processing::createSketch()
{
    return std::make_unique<TronGame>();
}
