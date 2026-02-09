#include <processing/processing.hpp>
using namespace processing;

int width = 900;
int height = 900;
float globalTime = 0.0f;

class Wave
{
public:
    float hue;
    float speed;
    float offset;

    Wave(float h, float s)
    {
        hue = h;
        speed = s;
        offset = random(1000);
    }

    void update()
    {
        hue = std::fmodf((hue + 0.5), 360);
    }

    void display()
    {
        noFill();
        strokeWeight(2);

        // Mehrere Schichten für Tiefe
        for (int layer = 0; layer < 3; layer++)
        {
            strokeJoin(StrokeJoin::bevel);
            beginShape(ShapeMode::polygon);
            for (int x = 0; x <= width; x += 5)
            {
                float angle = (x * 0.01 + globalTime * speed + offset + layer * 0.5) * TAU;
                float y = height / 2 + sin(angle) * (100 + layer * 30) * (1 + sin(globalTime * 0.5) * 0.3);

                // Farbe basierend auf Position variieren
                float colorVar = map(sin(angle), -1, 1, -30, 30);
                Color c = hsbToRgb(std::fmodf((hue + colorVar + 360), 360), 0.8 - layer * 0.2, 0.9 - layer * 0.2);
                stroke(c);

                vertex(x, y);
            }
            endShape();
        }

        // Kreise an den Wellenpunkten
        for (int x = 0; x <= width; x += 40)
        {
            float angle = (x * 0.01 + globalTime * speed + offset) * TAU;
            float y = height / 2 + sin(angle) * 100 * (1 + sin(globalTime * 0.5) * 0.3);

            Color c = hsbToRgb(std::fmodf((hue + 180), 360), 0.7, 0.8);
            fill(c, 204);
            noStroke();
            circle(x, y, 10 + sin(globalTime * 2 + x * 0.1) * 5);
        }
    }

private:
    // Hilfsfunktion zur Konvertierung von HSB zu RGB
    Color hsbToRgb(float h, float s, float b)
    {
        float r = 0, g = 0, bl = 0;
        float c = b * s;
        float x = c * (1 - abs(std::fmodf((h / 60), 2) - 1));
        float m = b - c;

        if (h < 60)
        {
            r = c;
            g = x;
            bl = 0;
        }
        else if (h < 120)
        {
            r = x;
            g = c;
            bl = 0;
        }
        else if (h < 180)
        {
            r = 0;
            g = c;
            bl = x;
        }
        else if (h < 240)
        {
            r = 0;
            g = x;
            bl = c;
        }
        else if (h < 300)
        {
            r = x;
            g = 0;
            bl = c;
        }
        else
        {
            r = c;
            g = 0;
            bl = x;
        }

        return Color((r + m) * 255, (g + m) * 255, (bl + m) * 255);
    }
};

struct RendererTest : Sketch
{
    std::vector<Wave> waves;

    void setup() override
    {
        // Mehrere Wellen mit verschiedenen Eigenschaften erstellen
        for (int i = 0; i < 2; i++)
        {
            waves.push_back(Wave(i * 72, i * 0.5 + 1));
        }
    }

    void draw(const f32 deltaTime) override
    {
        background(13);
        globalTime += 0.02;

        // Alle Wellen zeichnen
        for (Wave w : waves)
        {
            w.update();
            w.display();
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
