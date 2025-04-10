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

Color Camera3::renderPixel(
    int x, int y,
    float xStep, float yStep,
    float xResolution, float yResolution,
    PixelBuffer &pixelBuffer,
    const std::vector<std::shared_ptr<SceneObject>> &objects)
{
    Vector3 RayDirection(
        (float)x * xStep - xStep * xResolution / 2,
        (float)y * yStep - yStep * yResolution / 2,
        focalLength);

    RayDirection = normalize(RayDirection + getRandomDirection() * 0.01f);

    Vector3 RayOrigin = origin;
    Ray3 ray = Ray3(RayOrigin, normalize(RayDirection));

    Ray3 reflection;
    int maxReflections = 5;

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
        Color c = Color();;//ray.color * getSkybox(ray);
        return c;
    }

    // In case no return hit above (shouldn't happen logically, but to be safe)
    return Color();
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
            Color pixelColor = renderPixel(x, y, xStep, yStep, xResolution, yResolution, pixelBuffer, objects);

            for (int i = 0; i < 5; i++){
                pixelColor = pixelColor + renderPixel(x, y, xStep, yStep, xResolution, yResolution, pixelBuffer, objects);
            }

            pixelBuffer.setPixel(x, y, pixelColor);
            
        }
    }
}

// Render
void Camera3::render(PixelBuffer &pixelBuffer, std::vector<std::shared_ptr<SceneObject>> &objects, int numThreads)
{
    int xResolution = pixelBuffer.getWidth();
    float xStep = width / xResolution;
    int yResolution = pixelBuffer.getHeight();
    float yStep = height / yResolution;

    int rowsPerThread = yResolution / numThreads;

    std::vector<std::thread> threads;

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