#include "Render/Camera.h"
#include "Utils/Color.h"
#include <cstdio>
#include <memory>
#include <thread>

// Constructor
Camera3::Camera3(Vector3 origin_, Vector3 direction_, float focalLength_, float width_, float height_)
    : origin(origin_),
      direction(direction_),
      focalLength(focalLength_),
      width(width_),
      height(height_) {}

void Camera3::renderPixel(
    int x, int y,
    float xStep, float yStep,
    float xResolution, float yResolution,
    PixelBuffer &pixelBuffer,
    const std::vector<std::shared_ptr<SceneObject>> &objects)
{
    // Create the first ray

    // Perspective
    Vector3 RayDirection(
        (float)x * xStep - xStep * xResolution / 2,
        (float)y * yStep - yStep * yResolution / 2,
        focalLength);

    // Fake antialiasing
    RayDirection = RayDirection + getRandomDirection() * 0.05f;

    // Orthogonal
    // Vector3 RayDirection(
    //     0.0f,
    //     0.0f,
    //     1.0f);

    Vector3 RayOrigin = origin;
    Ray3 ray = Ray3(RayOrigin, normalize(RayDirection));

    Ray3 reflection;

    int maxReflections = 10;

    // A ray is considered done when a light is hit or no objects have been hit
    bool hit;
    bool lightHit = false;
    float minDistance = std::numeric_limits<float>::max(); // max max super max?
    Ray3 closestObjectReflection;
    int closestObjectIndex = 0;

    // TODO add distance checking so the objects are not rendered out of order

    for (int i = 0; i < maxReflections; i++)
    {
        hit = false;

        // Loop through all the object
        for (size_t j = 0; j < objects.size(); j++)
        {

            if (objects[j]->Intersect(ray, reflection) == true)
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
                lightHit = true;
                break;
            }
        }
    }
    if (lightHit == true)
    {
        pixelBuffer.setPixel(x, y, ray.color);
    }

    // else if (hit == false)
    // {
    //     pixelBuffer.setPixel(x, y, ray.color*getSkybox(ray));
    // }
}

void Camera3::renderRows(
    int startY, int endY,
    float xStep, float yStep,
    float xResolution, float yResolution,
    PixelBuffer &pixelBuffer,
    const std::vector<std::shared_ptr<SceneObject>> &objects)
{
    for (int y = startY; y < endY; ++y)
    {
        for (int x = 0; x < xResolution; ++x)
        {
            renderPixel(x, y, xStep, yStep, xResolution, yResolution, pixelBuffer, objects);
        }
    }
}

// Render
void Camera3::render(PixelBuffer &pixelBuffer, std::vector<std::shared_ptr<SceneObject>> &objects)
{
    int xResolution = pixelBuffer.getWidth();
    float xStep = width / xResolution;
    int yResolution = pixelBuffer.getHeight();
    float yStep = height / yResolution;

    std::vector<Color> pixels;
    pixels.resize(width * height);
    pixels = pixelBuffer.getPixels(); // Get the colors

    int numThreads = std::thread::hardware_concurrency(); // Use this as a default
    // int numThreads = 20;
    int rowsPerThread = yResolution / numThreads;

    std::vector<std::thread> threads;

    // For each pixel
    // for (unsigned int y = 0; y < yResolution; ++y)
    // {
    //     for (unsigned int x = 0; x < xResolution; ++x)
    //     {
    //         // Render the pixel
    //         renderPixel(
    //             x, y,
    //             xStep, yStep,
    //             xResolution, yResolution,
    //             pixelBuffer,
    //             objects);

    //     }
    // }

    for (int i = 0; i < numThreads; ++i)
    {
        int startY = i * rowsPerThread;
        int endY = (i == numThreads - 1) ? yResolution : (i + 1) * rowsPerThread;

        threads.emplace_back(&Camera3::renderRows, this,
                             startY, endY,
                             xStep, yStep, xResolution, yResolution,
                             std::ref(pixelBuffer),
                             std::cref(objects));
    }

    for (auto &t : threads)
        t.join();
}