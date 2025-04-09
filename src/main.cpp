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

#define window_width 600
#define window_height 400
#define font_path "/home/dennis/Projects/FatRayTracer/assets/fonts/Open_Sans/OpenSans-VariableFont_wdth,wght.ttf"


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

    // FPS Counter variables
    sf::Clock clock;
    int framecount = 0;           // Used to count the frames between two intervals
    float updateInterval = 0.01f; // Time between FPS updates
    float elapsedTime = 0.0f;     // Used to count the elapsed time from the last FPS print

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

    Sphere sphere0 = Sphere(Vector3(0.0f, 0.0f, 0.0f), 50.f);
    sphere0.color = Color(160, 160, 160, 0);
    sphere0.roughness = 10.8f;

    Color lightColor = Color(255, 255, 255, 0);

    float lightSize = 50.0f;
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

    Triangle3 triangle0 = Triangle3(v6, v8, v7);
    Triangle3 triangle1 = Triangle3(v6, v7, v5);
    Triangle3 triangle2 = Triangle3(v5, v7, v3);
    Triangle3 triangle3 = Triangle3(v3, v7, v1);
    Triangle3 triangle4 = Triangle3(v3, v1, v2);
    Triangle3 triangle5 = Triangle3(v3, v2, v4);
    Triangle3 triangle6 = Triangle3(v4, v2, v8);
    Triangle3 triangle7 = Triangle3(v6, v4, v8);
    Triangle3 triangle8 = Triangle3(v6, v3, v4);
    Triangle3 triangle9 = Triangle3(v6, v5, v3);

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

    float wallRoughness = 2.0f;

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

    objects.push_back(std::make_shared<Triangle3>(light0));
    objects.push_back(std::make_shared<Triangle3>(light1));
    objects.push_back(std::make_shared<Triangle3>(light2));
    objects.push_back(std::make_shared<Triangle3>(light3));
    objects.push_back(std::make_shared<Sphere>(sphere0));
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

    // Create a camera
    Vector3 cameraOrigin = Vector3(0.0f, 0.0f, -450.0f);
    Vector3 cameraDirection = Vector3(0.0f, 0.0f, 1.0f);
    Camera3 camera(cameraOrigin, cameraDirection, 15.0f, 10.0f, 10.0f);

    // Camera settings
    sf::Text CameraText;
    CameraText.setFont(font);
    CameraText.setCharacterSize(24);
    CameraText.setFillColor(sf::Color::Red);
    CameraText.setPosition(sf::Vector2f(10, height - 24 * 4));

    int iterations = 0;
    int iterationsLimit = 1000000;

    bool averaging = true;

    // Main rendering loop
    while (window.isOpen())
    {
        // Check for key presses
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) // Check if a key is pressed
            {

                // Check if 'Control' is held down and 'C' is pressed
                if (event.key.code == sf::Keyboard::C && event.key.control)
                {
                    // Action for when 'Ctrl+C' is pressed
                    window.close();
                }

                pixelBuffer.clearBuffer();
                iterations = 0;

                if (event.key.code == sf::Keyboard::A)
                {
                    camera.origin.x -= 5.0f;
                }
                if (event.key.code == sf::Keyboard::D)
                {
                    camera.origin.x += 5.0f;
                }
                if (event.key.code == sf::Keyboard::E)
                {
                    camera.origin.y += 5.0f;
                }
                if (event.key.code == sf::Keyboard::Q)
                {
                    camera.origin.y -= 5.0f;
                }
                if (event.key.code == sf::Keyboard::W)
                {
                    camera.origin.z += 1.0f;
                }
                if (event.key.code == sf::Keyboard::S)
                {
                    camera.origin.z -= 1.0f;
                }

                if (event.key.code == sf::Keyboard::F)
                {
                    camera.focalLength -= 0.1f;
                    if (camera.focalLength < 0.1f)
                    {
                        camera.focalLength = 0.1f;
                    }
                }
                if (event.key.code == sf::Keyboard::G)
                {
                    camera.focalLength += 0.1f;
                }

                // You can add other keys in a similar way
                if (event.key.code == sf::Keyboard::Escape) // Example: check for 'Escape' key
                {
                    window.close(); // Close the window when 'Escape' is pressed
                }
            }
        }

        if (iterations < iterationsLimit)
        {
            camera.render(pixelBuffer, objects);
            iterations++;
        }
        if (iterations == iterationsLimit - 1)
        {
            std::cout << "Done" << std::endl;
        }

        // Convert the pixel buffer to SFML (very slow)
        pixels = pixelBuffer.getPixels(); // Get the colors

        for (unsigned int y = 0; y < height; ++y)
        {
            for (unsigned int x = 0; x < width; ++x)
            {

                Color pixel = pixels[y * width + x];

                if (pixel.r == 0 && pixel.g == 0 && pixel.b == 0) continue;
                Color tmp;

                if (averaging) {
                sf::Color oldColor_sfml = pixels_sfml[y * width + x];
                Color oldColor = Color(oldColor_sfml.r, oldColor_sfml.g, oldColor_sfml.b, oldColor_sfml.a);// * (1/(float)iterations);
                tmp = averageColors(pixel, oldColor);
                }
                else{
                tmp = pixel;
                }

                pixels_sfml[y * width + x] = sf::Color(tmp.r, tmp.g, tmp.b);
            }
        }

        // Update the texture with the pixel buffer data
        texture.update(reinterpret_cast<const sf::Uint8 *>(pixels_sfml.data()));

        // Camera debug settings
        char buffer[100];
        sprintf(buffer, "Camera position: x:%.2f, y:%.2f, z:%.2f\nCamera rotation: x:%.2f, y:%.2f, z:%.2f\nFocal length%.2f",
                camera.origin.x,
                camera.origin.y,
                camera.origin.z,
                camera.direction.x,
                camera.direction.y,
                camera.direction.z,
                camera.focalLength);
        CameraText.setString(buffer);

        window.clear();

        // Create a sprite to draw the texture
        sf::Sprite sprite(texture);
        window.draw(sprite); // Draw the sprite containing the texture
        
        // window.draw(CameraText);

        window.display();
    }

    return 0;
}
