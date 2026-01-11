#include <processing/processing.hpp>
#include <processing/processing_data.hpp>
#include <processing/main_graphics.hpp>

#include <iostream>

using namespace processing;

namespace processing
{
    ProcessingData s_data;
}

namespace processing
{
    void close()
    {
        s_data.shouldAppClose = true;
        s_data.shouldRestart = false;
    }

    void close(int exitCode)
    {
        setExitCode(exitCode);
        close();
    }

    void restart()
    {
        s_data.shouldAppClose = true;
        s_data.shouldRestart = true;
    }

    void setExitCode(int exitCode)
    {
        s_data.exitCode = exitCode;
    }

    int getExitCode()
    {
        return s_data.exitCode;
    }
} // namespace processing

namespace processing
{
    void trace(const std::string& message)
    {
        std::cout << message.c_str() << std::endl;
    }

    void debug(const std::string& message)
    {
        std::cout << message.c_str() << std::endl;
    }

    void info(const std::string& message)
    {
        std::cout << message.c_str() << std::endl;
    }

    void warning(const std::string& message)
    {
        std::cout << message.c_str() << std::endl;
    }

    void error(const std::string& message)
    {
        std::cout << message.c_str() << std::endl;
    }
} // namespace processing

void launch()
{
    s_data.window = createWindow(1280, 720, "Processing");
    s_data.context = createContext(*s_data.window);
    s_data.graphics = createMainGraphics();
    s_data.sketch = createSketch();

    s_data.sketch->setup();

    while (not s_data.shouldAppClose)
    {
        if (const auto event = s_data.window->pollEvent())
        {
            if (event->type == Event::closed)
            {
                close();
            }

            if (event->type == Event::resized)
            {
                info("Window resized");
            }
        }

        s_data.context->flush();
    }

    s_data.sketch->destroy();
}

int main()
{
    do
    {
        std::memset((void*)&s_data, 0, sizeof(ProcessingData));
        launch();
    } while (s_data.shouldRestart);

    return s_data.exitCode;
}
