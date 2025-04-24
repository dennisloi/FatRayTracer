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
    std::vector<std::shared_ptr<Mesh3>> objects;

    // Create a scene (temp)

    Sphere sphere0 = Sphere(Vector3(50.0f, 0.0f, 0.0f), 50.f);
    // sphere0.transparency = 0.8;
    sphere0.color = Color(0.6f, 0.6f, 0.6f);
    sphere0.roughness = 1.0f;

    Color lightColor = Color(1.f, 1.f, 1.f);

    float lightSize = 90.0f;
    float lightHeight = -80.f;

    Vector3 lightV0 = Vector3(-lightSize, lightHeight, -lightSize);
    Vector3 lightV1 = Vector3(-lightSize, lightHeight, lightSize);
    Vector3 lightV2 = Vector3(lightSize, lightHeight, -lightSize);
    Vector3 lightV3 = Vector3(lightSize, lightHeight, lightSize);

    Triangle3 light0 = Triangle3(lightV1, lightV0, lightV2);
    Triangle3 light1 = Triangle3(lightV2, lightV3, lightV1);

    // Backside of the light, so no ray can pass through
    Triangle3 light2 = Triangle3(lightV0, lightV1, lightV2);
    Triangle3 light3 = Triangle3(lightV3, lightV2, lightV1);

    light0.color = lightColor;
    light1.color = lightColor;

    // Completely black backside
    light2.color = Color();
    light3.color = Color();

    light0.emissivity = 1.0f;
    light1.emissivity = 1.0f;

    Vector3 v1 = Vector3(-100.0f, -100.0f, -100.0f);
    Vector3 v2 = Vector3(100.0f, -100.0f, -100.0f);
    Vector3 v3 = Vector3(-100.0f, -100.0f, 100.0f);
    Vector3 v4 = Vector3(100.0f, -100.0f, 100.0f);
    Vector3 v5 = Vector3(-100.0f, 100.0f, 100.0f);
    Vector3 v6 = Vector3(100.0f, 100.0f, 100.0f);
    Vector3 v7 = Vector3(-100.0f, 100.0f, -100.0f);
    Vector3 v8 = Vector3(100.0f, 100.0f, -100.0f);

    // Bottom
    Triangle3 triangle0 = Triangle3(v6, v8, v7);
    Triangle3 triangle1 = Triangle3(v6, v7, v5);

    // Left
    Triangle3 triangle2 = Triangle3(v5, v7, v3);
    Triangle3 triangle3 = Triangle3(v3, v7, v1);

    // Top
    Triangle3 triangle4 = Triangle3(v3, v1, v2);
    Triangle3 triangle5 = Triangle3(v3, v2, v4);

    // Right
    Triangle3 triangle6 = Triangle3(v4, v2, v8);
    Triangle3 triangle7 = Triangle3(v6, v4, v8);

    // Back
    Triangle3 triangle8 = Triangle3(v6, v3, v4);
    Triangle3 triangle9 = Triangle3(v6, v5, v3);

    // Front
    Triangle3 triangle10 = Triangle3(v1, v7, v8);
    Triangle3 triangle11 = Triangle3(v1, v8, v2);

    // triangle10.transparency = 1.0f;
    // triangle11.transparency = 0.2f;
    // triangle10.emissivity = 0.1f;
    // triangle11.emissivity = 0.1f;

    triangle0.color = Color(50.0f / 255.0f, 50.0f / 255.0f, 127.0f / 255.0f);
    triangle1.color = Color(50.0f / 255.0f, 50.0f / 255.0f, 127.0f / 255.0f);
    triangle2.color = Color(50.0f / 255.0f, 127.0f / 255.0f, 127.0f / 255.0f);
    triangle3.color = Color(50.0f / 255.0f, 127.0f / 255.0f, 127.0f / 255.0f);
    triangle4.color = Color(50.0f / 255.0f, 127.0f / 255.0f, 50.0f / 255.0f);
    triangle5.color = Color(50.0f / 255.0f, 127.0f / 255.0f, 50.0f / 255.0f);
    triangle6.color = Color(127.0f / 255.0f, 50.0f / 255.0f, 25.0f / 255.0f);
    triangle7.color = Color(127.0f / 255.0f, 50.0f / 255.0f, 25.0f / 255.0f);
    triangle8.color = Color(127.0f / 255.0f, 25.0f / 255.0f, 25.0f / 255.0f);
    triangle9.color = Color(127.0f / 255.0f, 25.0f / 255.0f, 25.0f / 255.0f);
    triangle10.color = Color(127.0f / 255.0f, 127.0f / 255.0f, 127.0f / 255.0f);
    triangle11.color = Color(127.0f / 255.0f, 127.0f / 255.0f, 127.0f / 255.0f);

    float wallRoughness = 0.9f; // 🤡

    triangle0.roughness = wallRoughness;
    triangle1.roughness = wallRoughness;
    triangle2.roughness = wallRoughness;
    triangle3.roughness = wallRoughness;
    triangle4.roughness = wallRoughness;
    triangle5.roughness = wallRoughness;
    triangle6.roughness = wallRoughness;
    triangle7.roughness = wallRoughness;
    triangle8.roughness = wallRoughness;
    triangle9.roughness = wallRoughness;
    triangle10.roughness = wallRoughness;
    triangle11.roughness = wallRoughness;

    float wallEmissivity = 0.0f; // 🤡

    triangle0.emissivity = wallEmissivity;
    triangle1.emissivity = wallEmissivity;
    triangle2.emissivity = wallEmissivity;
    triangle3.emissivity = wallEmissivity;
    triangle4.emissivity = wallEmissivity;
    triangle5.emissivity = wallEmissivity;
    triangle6.emissivity = wallEmissivity;
    triangle7.emissivity = wallEmissivity;
    triangle8.emissivity = wallEmissivity;
    triangle9.emissivity = wallEmissivity;
    triangle10.emissivity = wallEmissivity;
    triangle11.emissivity = wallEmissivity;

    Sphere eye_sx = Sphere(Vector3(-25.0f, 20.0f, 00.0f), 15.f);
    eye_sx.emissivity = 1.0f;
    eye_sx.color = Color(1.f, 0.f, 0.f);
    Sphere eye_dx = Sphere(Vector3(25.0f, 20.0f, 00.0f), 15.f);
    eye_dx.emissivity = 1.0f;
    eye_dx.color = Color(0.f, 0.f, 1.f);

    // objects.push_back(std::make_shared<Sphere>(eye_sx));
    // objects.push_back(std::make_shared<Sphere>(eye_dx));

    // objects.push_back(std::make_shared<Sphere>(sphere0));
    // objects.push_back(std::make_shared<Triangle3>(light0));
    // objects.push_back(std::make_shared<Triangle3>(light1));
    // objects.push_back(std::make_shared<Triangle3>(light2));
    // objects.push_back(std::make_shared<Triangle3>(light3));
    // objects.push_back(std::make_shared<Triangle3>(triangle0));
    // objects.push_back(std::make_shared<Triangle3>(triangle1));
    // objects.push_back(std::make_shared<Triangle3>(triangle2));
    // objects.push_back(std::make_shared<Triangle3>(triangle3));
    // objects.push_back(std::make_shared<Triangle3>(triangle4));
    // objects.push_back(std::make_shared<Triangle3>(triangle5));
    // objects.push_back(std::make_shared<Triangle3>(triangle6));
    // objects.push_back(std::make_shared<Triangle3>(triangle7));
    // objects.push_back(std::make_shared<Triangle3>(triangle8));
    // objects.push_back(std::make_shared<Triangle3>(triangle9));

    // objects.push_back(std::make_shared<Triangle3>(triangle10));
    // objects.push_back(std::make_shared<Triangle3>(triangle11));

    // Load from STL
    // std::vector<std::shared_ptr<SceneObject>> stlObject;
    // stlObject = loadSTL(
    //     getExecutableDir() / "assets" / "meshes" / "Suzanne.stl",
    //     Vector3(.0f,.0f,.0f), //origin
    //     Vector3(.0f,.0f,.0f), //direction
    //     Vector3(70.f,70.f,70.f), //scale
    //     1.f, // Roughness
    //     0.f, // Emissivity
    //     0.f, // Transparency
    //     Color(0.5f, 0.5f, 0.5f));

    // // Add the STL object to the objects vector
    // for (size_t i = 0; i < stlObject.size(); i++)
    // {
    //     objects.push_back(stlObject[i]);
    // }

    Mesh3 Suzanne;

    // Suzanne.loadSTL(
    //     getExecutableDir() / "assets" / "meshes" / "Suzanne.stl",
    //     Vector3(.0f, .0f, .0f), // origin
    //     Vector3(.0f, .0f, .0f), // direction
    //     Vector3(70.f, 70.f, 70.f), // scale
    //     0.6f, // Roughness
    //     0.f, // Emissivity
    //     0.f, // Transparency
    //     Color(0.5f, 0.5f, 0.5f));

    Suzanne.loadOBJ(
        getExecutableDir() / "assets" / "meshes" / "cube2.obj",
        Vector3(.0f, .0f, .0f), // origin
        Vector3(.0f, .0f, .0f), // direction
        Vector3(50.f, 50.f, 50.f), // scale
        1.f, // Roughness
        1.f, // Emissivity
        0.f, // Transparency
        Color(1.f, 0.5f, 0.5f));
    Suzanne.loadTexture(
        getExecutableDir() / "assets" / "meshes" / "cube2.png");
    // Suzanne.hasTexture = false;

    // for (size_t i = 0; i < Suzanne.triangles.size(); i++)
    // {
        objects.push_back(std::make_shared<Mesh3>(Suzanne));
    // }

    // Create a camera
    Vector3 cameraOrigin = Vector3(0.0f, 0.0f, -400.0f);
    Vector3 cameraDirection = normalize(Vector3(0.2f, .0f, 1.0f));
    float aspectRatio = static_cast<float>(width) / height;

    // Prospective
    Camera3 camera(cameraOrigin, cameraDirection, 15.0f, 10.f * aspectRatio, 10.f);
    camera.projection = ProjectionType::Prospective;

    // Orthographic
    // Camera3 camera(cameraOrigin, cameraDirection, 15.0f, 300.f * aspectRatio, 300.f);
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
    int averages = 10;
    int maxReflections = 5;
    float antialiasing = 0.0001f;
    float gain = 1.f;

    // Create render object
    Render3 renderer(camera, objects, averages, maxReflections, width, height, antialiasing, gain);

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
            rendering = renderer.renderLoop(pixelBuffer, numThreads * 2);
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
