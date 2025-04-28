#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>

#include <cmath>

#include "Core/Vector.h"
#include "Core/Ray.h"
#include "Core/Triangle.h"
#include "Core/Mesh.h"
#include "Core/Sphere.h"
#include "Render/PixelBuffer.h"
#include "Render/Render.h"
#include "Utils/Color.h"
#include "Render/Camera.h"
#include <iostream>

#include <thread>
#include <chrono>
#include <future>

#include <fstream>
#include <iostream>
#include <filesystem>

// Windows specific
#ifdef _WIN32
#include <windows.h>
std::string getExecutablePath() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return std::string(buffer);
}
#endif

// Linux specific
#ifdef __linux__
#include <unistd.h>
#include <limits.h>
std::string getExecutablePath() {
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
    if (len != -1) {
        buffer[len] = '\0';
        return std::string(buffer);
    }
    return "";
}
#endif

std::filesystem::path getExecutableDir() {
    return std::filesystem::path(getExecutablePath()).parent_path();
}

#define window_width 500
#define window_height 500
#define font_path "../../assets/fonts/Open_Sans/OpenSans-VariableFont_wdth,wght.ttf"

int main()
{
    // Setup the SFML window
    auto window = sf::RenderWindow{{window_width, window_height}, "FatRayTracer"};
    // window.setFramerateLimit(144);

    // Select a font for the debug text
    sf::Font font;
    font.loadFromFile(getExecutableDir() / "assets" / "fonts" / "Open_Sans" / "OpenSans-VariableFont_wdth,wght.ttf"); 

    // FPS Counter text element
    sf::Text fpsCounter;
    fpsCounter.setFont(font);
    fpsCounter.setCharacterSize(24);
    fpsCounter.setFillColor(sf::Color::Red);

    int width = window_width;
    int height = window_height;

    // Create a pixel buffer
    PixelBuffer pixelBuffer(width, height);

    // Create an SFML texture to display the pixel buffer
    sf::Texture texture;
    texture.create(width, height);

    // Variables to convert the pixelBuffer in a format accepted from SFML

    // SFML Color vector
    std::vector<sf::Color> pixels_sfml;
    pixels_sfml.resize(width * height);

    // Temp color vector
    std::vector<Color> pixels;
    pixels.resize(width * height);

    // Create a vector to store pointers to SceneObject (this will become a class)
    std::vector<std::shared_ptr<SceneObject>> objects;

    std::shared_ptr<Mesh3> Suzanne = std::make_shared<Mesh3>();

    Suzanne->loadOBJ(
        getExecutableDir() / "assets" / "meshes" / "cube.obj",
        Vector3(.0f, .0f, .0f), // origin
        Vector3(.0f, .0f, .0f), // direction
        Vector3(50.f, 50.f, 50.f), // scale
        1.f, // Roughness
        0.f, // Emissivity
        0.f, // Transparency
        Color(1.f, 0.5f, 0.5f));
    Suzanne->loadTexture(
        getExecutableDir() / "assets" / "meshes" / "cube2.png");
    objects.push_back(Suzanne);

    std::shared_ptr<Mesh3> Light = std::make_shared<Mesh3>();
    Light->loadOBJ(
        getExecutableDir() / "assets" / "meshes" / "face.obj",
        Vector3(-80.0f, 100.0f, 0.0f), // origin
        Vector3(.0f, .0f, .0f), // direction
        Vector3(100.f, 100.f, 100.f), // scale
        1.f, // Roughness
        1.f, // Emissivity
        0.f, // Transparency
        Color(1.f, 0.5f, 0.5f));
    
    objects.push_back(Light);

    // Create a camera
    Vector3 cameraOrigin = Vector3(-200.f, -400.0f, 200.0f);
    Vector3 cameraDirection = normalize(Vector3(.5f, 1.0f, -.5f));
    Vector3 cameraUp = Vector3(.0f, 0.f, 1.f);
    float aspectRatio = static_cast<float>(width) / height;

    // Prospective
    Camera3 camera(cameraOrigin, cameraDirection, cameraUp, 15.0f, 10.f * aspectRatio, 10.f);
    camera.projection = ProjectionType::Prospective;

    // Orthographic
    // Camera3 camera(cameraOrigin, cameraDirection, cameraUp, 15.0f, 300.f * aspectRatio, 300.f);
    // camera.projection = ProjectionType::Orthographic;

    // Camera settings
    sf::Text CameraText;
    CameraText.setFont(font);
    CameraText.setCharacterSize(24);
    CameraText.setFillColor(sf::Color::Red);
    CameraText.setPosition(sf::Vector2f(10, 10));
    CameraText.setString("Press 'A' to start the render");

    // Create a list of rendering threads
    std::vector<std::future<void>> renderingThreads;
    int numThreads = std::thread::hardware_concurrency();

    // Print stuff before starting the render
    std::cout << "Number of concurrent threads supported: " << std::thread::hardware_concurrency() << "(" << numThreads << " used)" << std::endl;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    std::chrono::time_point<std::chrono::high_resolution_clock> stop;
    sf::Sprite sprite(texture);

    // Rendering settings
    int averages = 20;
    int maxReflections = 10;
    float antialiasing = 0.0001f;
    float gain = 1.f;

    // Create render object
    Render3 renderer= Render3(camera, objects, averages, maxReflections, width, height, antialiasing, gain);

    // Create render queue
    int divs = 20;

    bool rendering = false;
    bool renderFinished = false;

    // Main rendering loop
    while (window.isOpen())
    {
        // Handle key presses
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                //'A' key
                if (event.key.code == sf::Keyboard::A)
                {
                    // Start rendering
                    if (!rendering)
                    {

                        rendering = true;
                        CameraText.setString("Rendering...");
                        pixelBuffer.clearBuffer();

                        renderer.createRenderQueue(divs, RenderQueueType::Spiral); //or RenderQueueType::Grid
                        start = std::chrono::high_resolution_clock::now();
                    }
                }

                //'C' key
                if (event.key.code == sf::Keyboard::C)
                {
                    rendering = false;
                    CameraText.setString("Stopping...");
                }
                if (event.key.code == sf::Keyboard::Escape || ((event.key.control && event.key.code == sf::Keyboard::C)))
                {
                    window.close();
                }
            }
        }

        // Rendering part
        window.clear();

        if (rendering)
        {
            // Update the rendering threads
            rendering = renderer.renderLoop(pixelBuffer, 1);
            // rendering = renderer.renderLoop(pixelBuffer, numThreads * 2);
            if (!rendering)
            {
                renderFinished = true;
                stop = std::chrono::high_resolution_clock::now();
            }
        }

        // Convert the pixel buffer to SFML TODO move to a function/Class/method/somethingthatisnotfullyhere
        pixels = pixelBuffer.getPixels(); // Get the colors
        for (unsigned int y = 0; y < height; y++)
        {
            for (unsigned int x = 0; x < width; x++)
            {
                Color pixel = pixels[y * width + x];

                // Apply Gamma correction
                float r = std::sqrt(pixel.r) * 255;
                float g = std::sqrt(pixel.g) * 255;
                float b = std::sqrt(pixel.b) * 255;

                // Cast to Uint8
                pixels_sfml[y * width + x] = sf::Color(
                    static_cast<unsigned char>(std::min(std::max(r, 0.f), 255.f)),
                    static_cast<unsigned char>(std::min(std::max(g, 0.f), 255.f)),
                    static_cast<unsigned char>(std::min(std::max(b, 0.f), 255.f)));
            }
        }

        // Update the texture with the pixel buffer data
        texture.update(reinterpret_cast<const sf::Uint8 *>(pixels_sfml.data()));

        // Draw the sprite containing the texture
        window.draw(sprite);

        if (renderFinished)
        {
            // Render finished
            renderFinished = false;
            auto stop = std::chrono::high_resolution_clock::now();
            CameraText.setString("Render finished in " +
                                 std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()) +
                                 "ms!");
        }

        window.draw(CameraText);
        window.display();
    }

    return 0;
}
