#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>

#include <cmath>

#include "Core/Vector.h"
#include "Core/Ray.h"
#include "Core/Triangle.h"
#include "Core/Sphere.h"
#include "Render/PixelBuffer.h"
#include "Utils/Color.h"
#include "Render/Camera.h"
#include <iostream>

#include <thread>
#include <chrono>
#include <future>

#include <fstream>
#include <iostream>

#define window_width  1080//640
#define window_height 1080//480
#define font_path "../../assets/fonts/Open_Sans/OpenSans-VariableFont_wdth,wght.ttf"

void Filter(
    PixelBuffer &pixelBuffer
)
{
    int kernelRadius = 1;
    std::vector<float> kernel(9); // Todo kernel class

    // Gaussian kernel
    kernel[0] = 1.0f / 16.0f;
    kernel[1] = 2.0f / 16.0f;
    kernel[2] = 1.0f / 16.0f;
    kernel[3] = 2.0f / 16.0f;
    kernel[4] = 4.0f / 16.0f;
    kernel[5] = 2.0f / 16.0f;
    kernel[6] = 1.0f / 16.0f;
    kernel[7] = 2.0f / 16.0f;
    kernel[8] = 1.0f / 16.0f;
    

    PixelBuffer oldBuffer = pixelBuffer;

    for (int y = 0; y < pixelBuffer.height; y++){
        for(int x = 0; x< pixelBuffer.width; x++){

            float r = 0.f;
            float g = 0.f;
            float b = 0.f;

            for(int dy = -kernelRadius; dy <= kernelRadius; dy++){
                for(int dx = -kernelRadius; dx <= kernelRadius; dx++){

                    int ny = y + dy;
                    int nx = x + dx;

                    // Check for edge of the buffer
                    if (nx >= pixelBuffer.width || nx < 0 || ny >= pixelBuffer.height || ny < 0) continue;

                    r += oldBuffer.getPixels()[ny * pixelBuffer.width + nx].r * kernel[(dy + kernelRadius) * kernelRadius + (dx + kernelRadius)];
                    g += oldBuffer.getPixels()[ny * pixelBuffer.width + nx].g * kernel[(dy + kernelRadius) * kernelRadius + (dx + kernelRadius)];
                    b += oldBuffer.getPixels()[ny * pixelBuffer.width + nx].b * kernel[(dy + kernelRadius) * kernelRadius + (dx + kernelRadius)];
                }
            }

            pixelBuffer.setPixel(x, y, Color(r, g, b, 0));
        }
    }
}

// Structure used to segment the rendering TODO move
struct Rectangle
{
    int startX;
    int endX;
    int startY;
    int endY;
    bool done = false;
    bool rendering = false;

    Rectangle(int sx, int ex, int sy, int ey)
        : startX(sx), endX(ex), startY(sy), endY(ey) {}
};

// TODO move
Color renderPixel(
    int x, int y,
    Camera3 camera,
    int width, int height,
    int maxReflections,
    const std::vector<std::shared_ptr<SceneObject>> &objects)
{

    float xStep = camera.width / width;
    float yStep = camera.height / height;

    Vector3 RayDirection(
        (float)x * xStep - xStep * width / 2,
        (float)y * yStep - yStep * height / 2,
        camera.focalLength);

    RayDirection = normalize(RayDirection + getRandomDirection() * 0.01f); // TODO variable

    Vector3 RayOrigin = camera.origin;
    Ray3 ray = Ray3(RayOrigin, RayDirection);

    Ray3 reflection;

    float minDistance = std::numeric_limits<float>::max();
    Ray3 closestObjectReflection;
    int closestObjectIndex = -1;
    bool hit = false;

    for (int i = 0; i < maxReflections; i++)
    {
        hit = false;

        for (size_t j = 0; j < objects.size(); j++)
        {
            if (objects[j]->Intersect(ray, reflection))
            {
                float distance = (reflection.origin - ray.origin).getLength();
                if (distance < minDistance)
                {
                    minDistance = distance;
                    closestObjectReflection = reflection;
                    closestObjectIndex = j;
                }
                hit = true;
            }
        }

        if (hit)
        {
            ray = closestObjectReflection;
            if (objects[closestObjectIndex]->emissivity > 0.0f)
            {
                Color c = ray.color * objects[closestObjectIndex]->emissivity * 2;
                return c;
            }
        }
    }

    if (!hit)
    {
        Color c = Color();
        return ray.color * getSkybox(ray);
        return c;
    }

    // In case no return hit above (shouldn't happen logically, but to be safe)
    return Color();
}

// TODO move
void render(
    PixelBuffer &pixelBuffer,
    Camera3 camera,
    int width, int height,
    std::vector<std::shared_ptr<SceneObject>> &objects,
    Rectangle &rectangle,
    int averages,
    int maxReflections)
{
    rectangle.rendering = true;

    for (int y = rectangle.startY; y < rectangle.endY; y++)
    {
        for (int x = rectangle.startX; x < rectangle.endX; x++)
        {
            float r = 0.f;
            float g = 0.f;
            float b = 0.f;


            for (int i = 0; i < averages; i++)
            {
                Color pixelColor = renderPixel(x, y, camera, width, height, maxReflections, objects);

                r += (float) pixelColor.r;
                g += (float) pixelColor.g;
                b += (float) pixelColor.b;
            }

            // Divide by the averages
            r = r / averages;
            g = g / averages;
            b = b / averages;

            // // Blend it with the current pixel buffer
            // if (blend) {
            //     Color oldColor = pixelBuffer.getPixels()[y * width + x];
            //     float oldRed = oldColor.r;
            //     float oldGreen = oldColor.g;
            //     float oldBlue = oldColor.b;

            //     // Undo Gamma correction
            //     oldRed = std::pow(oldRed / 255, 2) * 255;
            //     oldGreen = std::pow(oldGreen / 255, 2) * 255;
            //     oldBlue = std::pow(oldBlue / 255, 2) * 255;

                
            //     // Average old and new colors
            //     r = (r + oldRed) / 2;
            //     g = (g + oldGreen) / 2;
            //     b = (b + oldBlue) / 2;
            // }

            // Gamma correction
            r = std::sqrt(r/255)*255;
            g = std::sqrt(g/255)*255;
            b = std::sqrt(b/255)*255;


            Color finalColor = Color(
                static_cast<int>(r),
                static_cast<int>(g),
                static_cast<int>(b),
                0
            );
            
            pixelBuffer.setPixel(x, y, finalColor);
        }
    }

    rectangle.rendering = false;
}

bool updateRender(
    PixelBuffer &pixelBuffer,
    Camera3 camera,
    int width, int height,
    std::vector<std::shared_ptr<SceneObject>> &objects,
    std::vector<std::future<void>> &renderingThreads,
    std::vector<Rectangle> &testQueue,
    int numThreads,
    int &counter)
{
    // SETTINGS
    int averages = 100;
    int maxReflections = 10;

        // Start new render threads
    while (renderingThreads.size() < numThreads && counter > 0)
    {
        counter--;

        // Start a rendering thread that uses camera.render and the coords from the rectangle
        renderingThreads.push_back(std::async(std::launch::async, render, std::ref(pixelBuffer), camera, width, height, std::ref(objects), std::ref(testQueue[counter]), averages, maxReflections));
    }

    // Terminate finished threads
    for (int i = renderingThreads.size() - 1; i >= 0; --i)
    {
        if (renderingThreads[i].wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
        {
            // Thread is finished
            renderingThreads.erase(renderingThreads.begin() + i); // Remove finished thread
        }
    }

    if (renderingThreads.size() == 0)
        return false;
    else
        return true;
}

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
    sphere0.color = Color(160, 160, 160, 0);
    sphere0.roughness = 1.0f;

    Color lightColor = Color(255, 255, 255, 0);

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

    triangle0.color = Color(50, 50, 127, 0);
    triangle1.color = Color(50, 50, 127, 0);
    triangle2.color = Color(50, 127, 127, 0);
    triangle3.color = Color(50, 127, 127, 0);
    triangle4.color = Color(50, 127, 50, 0);
    triangle5.color = Color(50, 127, 50, 0);
    triangle6.color = Color(127, 50, 25, 0);
    triangle7.color = Color(127, 50, 25, 0);
    triangle8.color = Color(127, 25, 25, 0);
    triangle9.color = Color(127, 25, 25, 0);
    triangle10.color = Color(127, 127, 127, 0);
    triangle11.color = Color(127, 127, 127, 0);

    // triangle0.color = Color(255, 255, 255, 0);
    // triangle1.color = Color(255, 255, 255, 0);
    // triangle2.color = Color(255, 255, 255, 0);
    // triangle3.color = Color(255, 255, 255, 0);
    // triangle4.color = Color(255, 255, 255, 0);
    // triangle5.color = Color(255, 255, 255, 0);
    // triangle6.color = Color(255, 255, 255, 0);
    // triangle7.color = Color(255, 255, 255, 0);
    // triangle8.color = Color(255, 255, 255, 0);
    // triangle9.color = Color(255, 255, 255, 0);
    // triangle10.color = Color(255, 255, 255, 0);
    // triangle11.color = Color(255, 255, 255, 0);

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
    eye_sx.color = Color(255, 0, 0, 0);
    Sphere eye_dx = Sphere(Vector3(25.0f, 20.0f, 00.0f), 15.f);
    eye_dx.emissivity = 1.0f;
    eye_dx.color = Color(0, 0, 255, 0);

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
        Color(127, 127, 127, 0));

    // Add the STL object to the objects vector
    for (size_t i = 0; i < stlObject.size(); i++)
    {
        objects.push_back(stlObject[i]);
    }

    // Create a camera
    Vector3 cameraOrigin = Vector3(0.0f, 0.0f, -400.0f);
    Vector3 cameraDirection = Vector3(0.0f, 0.0f, 1.0f);
    float aspectRatio = static_cast<float>(width) / height;
    Camera3 camera(cameraOrigin, cameraDirection, 15.0f, 10.f*aspectRatio, 10.f);

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

    // Prepare the task queue
    std::vector<Rectangle> testQueue;

    int xDivs = 20;
    int xStep = width / xDivs; //TODO, check for fractional divisions! May cause part of the image to not be rendered
    int yDivs = 20;
    int yStep = height / yDivs;

    for (int y = yDivs - 1; y >= 0; y--)
    {
        for (int x = xDivs - 1; x >= 0; x--)
        {
            int startX = x * xStep;
            int startY = y * yStep;
            testQueue.push_back(Rectangle(startX, startX + xStep, startY, startY + yStep));
            // std::cout << "startX: " << startX << ", endX: " << startX+xStep << ", startY: " << startY << ", endY: " << startY+yStep << std::endl;
        }
    }

    // Print stuff before starting the render
    std::cout << "Number of concurrent threads supported: " << std::thread::hardware_concurrency() << "(" << numThreads << " used)" << std::endl;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    std::chrono::time_point<std::chrono::high_resolution_clock> stop;
    sf::Sprite sprite(texture);

    bool rendering = false;
    bool renderFinished = false;
    int counter = testQueue.size();

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

                        counter = testQueue.size();

                        for (int i = 0; i < testQueue.size(); i++)
                        {
                            testQueue[i].done = false;
                            testQueue[i].rendering = false;
                        }

                        start = std::chrono::high_resolution_clock::now();
                    }
                }

                //'C' key
                if (event.key.code == sf::Keyboard::C)
                {
                    rendering = false;
                    for (int i = 0; i < testQueue.size(); i++)
                    {
                        testQueue[i].done = false;
                    }

                    CameraText.setString("Stopping...");
                }
                //'F' key
                if (event.key.code == sf::Keyboard::F)
                {
                    Filter(pixelBuffer);
                }
                //'Escape' key
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
            rendering = updateRender(pixelBuffer, camera, width, height, objects, renderingThreads, testQueue, numThreads, counter);
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
                pixels_sfml[y * width + x] = sf::Color(pixel.r, pixel.g, pixel.b);
            }
        }

        // Update the texture with the pixel buffer data
        texture.update(reinterpret_cast<const sf::Uint8 *>(pixels_sfml.data()));

        // Draw the sprite containing the texture
        window.draw(sprite);

        // Draw rendering rectangles
        for (int i = 0; i < testQueue.size(); i++)
        {
            if (testQueue[i].rendering == true)
            {
                sf::RectangleShape rectangle(sf::Vector2f(testQueue[i].endX - testQueue[i].startX, testQueue[i].endY - testQueue[i].startY));
                rectangle.setFillColor(sf::Color::Transparent);
                rectangle.setOutlineThickness(1);
                rectangle.setPosition(testQueue[i].startX, testQueue[i].startY);

                if (rendering)
                    rectangle.setOutlineColor(sf::Color::Yellow);
                else
                    rectangle.setOutlineColor(sf::Color::Red);

                window.draw(rectangle);
            }
        }

        // TODO clean up this fucking hack
        // if (counter == 0 && finished == true && !renderFinished)
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
