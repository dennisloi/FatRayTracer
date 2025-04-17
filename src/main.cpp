#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>

#include <cmath>

#include "Core/Vector.h"
#include "Core/Ray.h"
#include "Core/Triangle.h"
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

#define window_width 600
#define window_height 600
#define font_path "../../assets/fonts/Open_Sans/OpenSans-VariableFont_wdth,wght.ttf"

std::vector<std::shared_ptr<SceneObject>> loadSTL(
    const std::string &fileName,
    float roughness = 0.0f,
    float emissivity = 0.0f,
    float transparency = 0.0f,
    Color color = Color())
{
    std::vector<std::shared_ptr<SceneObject>> objects;

    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file!" << std::endl;
        return objects;
    }

    char header[80];

    file.read(header, 80);
    // Print the header
    // std::cout << "STL Header: " << std::string(header, 80) << std::endl;

    unsigned int numTriangles;
    file.read(reinterpret_cast<char *>(&numTriangles), sizeof(unsigned int));
    std::cout << "Number of triangles: " << numTriangles << std::endl;

    for (int i = 0; i < numTriangles - 1; i++)
    {
        float normal_x;
        float normal_y;
        float normal_z;
        float v1_x;
        float v1_y;
        float v1_z;
        float v2_x;
        float v2_y;
        float v2_z;
        float v3_x;
        float v3_y;
        float v3_z;
        char byteCount[2];

        file.read(reinterpret_cast<char *>(&normal_x), sizeof(float));
        file.read(reinterpret_cast<char *>(&normal_y), sizeof(float));
        file.read(reinterpret_cast<char *>(&normal_z), sizeof(float));
        file.read(reinterpret_cast<char *>(&v1_x), sizeof(float));
        file.read(reinterpret_cast<char *>(&v1_y), sizeof(float));
        file.read(reinterpret_cast<char *>(&v1_z), sizeof(float));
        file.read(reinterpret_cast<char *>(&v2_x), sizeof(float));
        file.read(reinterpret_cast<char *>(&v2_y), sizeof(float));
        file.read(reinterpret_cast<char *>(&v2_z), sizeof(float));
        file.read(reinterpret_cast<char *>(&v3_x), sizeof(float));
        file.read(reinterpret_cast<char *>(&v3_y), sizeof(float));
        file.read(reinterpret_cast<char *>(&v3_z), sizeof(float));
        file.read(byteCount, 2);

        // Debug print
        // std::cout << "Normal: (" << normal_x << ", " << normal_y << ", " << normal_z << ")" << std::endl;
        // std::cout << "Vertex 1: (" << v1_x << ", " << v1_y << ", " << v1_z << ")" << std::endl;
        // std::cout << "Vertex 2: (" << v2_x << ", " << v2_y << ", " << v2_z << ")" << std::endl;
        // std::cout << "Vertex 3: (" << v3_x << ", " << v3_y << ", " << v3_z << ")" << std::endl;
        // std::cout << "Byte count: " << (unsigned int)byteCount[0] << std::endl;

        float scale = 70.0f;   // Scale factor for the STL model
        float offsetX = 0.0f;  // X-axis offset
        float offsetY = 70.0f; // Y-axis offset
        float offsetZ = 70.0f; // Z-axis offset

        Triangle3 triangle = Triangle3(
            Vector3(v1_y * scale + offsetX, v1_x * scale + offsetY, v1_z * scale + offsetZ),
            Vector3(v2_y * scale + offsetX, v2_x * scale + offsetY, v2_z * scale + offsetZ),
            Vector3(v3_y * scale + offsetX, v3_x * scale + offsetY, v3_z * scale + offsetZ));
        // std::cout << "Normal:" << triangle.n.x << ", " << triangle.n.y << ", " << triangle.n.z << std::endl;
        triangle.n = normalize(Vector3(normal_y, normal_x, normal_z) * -1.0f);
        // std::cout << "Normal:" << triangle.n.x << ", " << triangle.n.y << ", " << triangle.n.z << std::endl;

        // Triangle settings
        triangle.color = color,
        triangle.roughness = roughness;
        triangle.emissivity = emissivity;
        triangle.transparency = transparency;

        objects.push_back(std::make_shared<Triangle3>(triangle));
    }
    return objects;
}

int main()
{
    // Setup the SFML window
    auto window = sf::RenderWindow{{window_width, window_height}, "FatRayTracer"};
    // window.setFramerateLimit(144);

    // Select a font for the debug text
    sf::Font font;
    font.loadFromFile(font_path);

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

    objects.push_back(std::make_shared<Triangle3>(light0));
    objects.push_back(std::make_shared<Triangle3>(light1));
    objects.push_back(std::make_shared<Triangle3>(light2));
    objects.push_back(std::make_shared<Triangle3>(light3));
    objects.push_back(std::make_shared<Triangle3>(triangle0));
    objects.push_back(std::make_shared<Triangle3>(triangle1));
    objects.push_back(std::make_shared<Triangle3>(triangle2));
    objects.push_back(std::make_shared<Triangle3>(triangle3));
    objects.push_back(std::make_shared<Triangle3>(triangle4));
    objects.push_back(std::make_shared<Triangle3>(triangle5));
    objects.push_back(std::make_shared<Triangle3>(triangle6));
    objects.push_back(std::make_shared<Triangle3>(triangle7));
    objects.push_back(std::make_shared<Triangle3>(triangle8));
    objects.push_back(std::make_shared<Triangle3>(triangle9));

    objects.push_back(std::make_shared<Triangle3>(triangle10));
    objects.push_back(std::make_shared<Triangle3>(triangle11));

    // Load from STL
    std::vector<std::shared_ptr<SceneObject>> stlObject;
    stlObject = loadSTL(
        "../../Suzanne2.stl",
        1.f, // Roughness
        0.f, // Emissivity
        0.f, // Transparency
        Color(0.5f, 0.5f, 0.5f));

    // Add the STL object to the objects vector
    for (size_t i = 0; i < stlObject.size(); i++)
    {
        objects.push_back(stlObject[i]);
    }

    // Create a camera
    Vector3 cameraOrigin = Vector3(0.0f, 0.0f, -400.0f);
    Vector3 cameraDirection = Vector3(0.0f, 0.0f, 1.0f);
    float aspectRatio = static_cast<float>(width) / height;
    Camera3 camera(cameraOrigin, cameraDirection, 15.0f, 10.f * aspectRatio, 10.f);

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
    int maxReflections = 5;
    float antialiasing = 0.0001f;

    // Create render object
    Render3 renderer(camera, objects, averages, maxReflections, width, height, antialiasing);

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
