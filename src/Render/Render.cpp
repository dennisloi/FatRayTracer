#include "Render/Render.h"
#include "Render/PixelBuffer.h"

#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <iostream>
#include <algorithm>

// Default constructor
Render3::Render3(
    Camera3 camera_,
    const std::vector<std::shared_ptr<SceneObject>> &objects_,
    int averages_,
    int maxReflections_,
    int resX_,
    int resY_,
    float antialiasing_)
    : camera(camera_), objects(objects_), averages(averages_), maxReflections(maxReflections_), resX(resX_), resY(resY_), antialiasing(antialiasing_)
{
}

// TODO manage not integer divisions
void Render3::createRenderQueue(int divs, RenderQueueType type)
{
    renderingQueue.clear();

    int xStep = (resX + divs - 1) / divs;
    int yStep = (resY + divs - 1) / divs;

    switch (type)
    {
    case RenderQueueType::Grid:
        for (int y = divs - 1; y >= 0; y--)
        {
            for (int x = divs - 1; x >= 0; x--)
            {
                int startX = x * xStep;
                int startY = y * yStep;
                renderingQueue.push_back(renderRectangle3(startX, std::min(startX + xStep, resX),
                                                          startY, std::min(startY + yStep, resY)));
            }
        }
        break;

    case RenderQueueType::Spiral:
    {
        for (int i = divs - 1; i > 0; i--)
        {
            renderingQueue.push_back(renderRectangle3(0, xStep, i * yStep, i * yStep + yStep));
        }

        int i = divs - 1;
        int x = 0;
        int y = 0;
        int dir = 1;
        while (true)
        {
            for (int j = 0; j < i; j++)
            {
                renderingQueue.push_back(renderRectangle3(x * xStep, x * xStep + xStep, y * yStep, y * yStep + yStep));
                x += dir;
            }
            for (int j = 0; j < i; j++)
            {
                renderingQueue.push_back(renderRectangle3(x * xStep, x * xStep + xStep, y * yStep, y * yStep + yStep));
                y += dir;
            }
            i--;
            if (i == 0)
            {
                renderingQueue.push_back(renderRectangle3(x * xStep, x * xStep + xStep, y * yStep, y * yStep + yStep));
                break;
            }
            dir = -dir;
        }
    }
    break;
    }
}

Color Render3::renderRay(Ray3 ray)
{
    Ray3 reflection;

    float minDistance;
    Ray3 closestObjectReflection;
    int closestObjectIndex;
    bool hit = false;

    for (int i = 0; i < maxReflections; i++)
    {
        hit = false;
        minDistance = std::numeric_limits<float>::max();
        closestObjectIndex = -1;

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
                Color c = ray.color * objects[closestObjectIndex]->emissivity * gain;
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

    // In case no return hit above (shouldn't happen? But I don't pay for characters)
    return Color();
}

Ray3 Render3::createRay(int x, int y, ProjectionType projection)
{
    // Prospectic
    float ndcX = (x + 0.5f) / resX; // normalized device coordinates [0,1]
    float ndcY = (y + 0.5f) / resY;

    float screenX = (ndcX - 0.5f) * camera.width;
    float screenY = (ndcY - 0.5f) * camera.height; //(0.5f - ndcY) will flip Y

    Vector3 RayDirection;
    Vector3 RayOrigin;

    if (projection == ProjectionType::Prospective)
    {
        RayDirection = normalize(Vector3(screenX, screenY, camera.focalLength));

        RayDirection = normalize(RayDirection + getRandomDirection() * antialiasing);

        RayOrigin = camera.origin;
    }
    else if (projection == ProjectionType::Orthographic)
    {
        // In orthographic, direction is fixed
        RayDirection = camera.direction;

        // Apply antialiasing jitter to origin, not direction
        Vector3 jitter = getRandomDirection() * antialiasing;
        RayOrigin = camera.origin + Vector3(screenX, screenY, 0.0f) + jitter;
    }

    // Orthographic projection

    return Ray3(RayOrigin, RayDirection);
}

void Render3::render(PixelBuffer &pixelBuffer, renderRectangle3 &rectangle)
{
    // Render the specified rectangle

    // Mark the rectangle as rendering
    rectangle.rendering = true;

    Ray3 ray;
    Color pixelColor;
    for (int y = rectangle.startY; y < rectangle.endY; y++)
    {
        for (int x = rectangle.startX; x < rectangle.endX; x++)
        {
            float r = 0.f;
            float g = 0.f;
            float b = 0.f;

            for (int i = 0; i < averages; i++)
            {
                // Generate the ray
                ray = createRay(x, y, camera.projection);

                // Render the ray
                pixelColor = renderRay(ray);

                r += pixelColor.r;
                g += pixelColor.g;
                b += pixelColor.b;
            }

            // Divide by the averages
            r = r / averages;
            g = g / averages;
            b = b / averages;

            pixelBuffer.setPixel(x, y, Color(r, g, b));
        }
    }

    rectangle.rendering = false;
}

bool Render3::renderLoop(PixelBuffer &pixelBuffer, int numThreads)
{
    while (renderingThreads.size() < numThreads && renderingQueue.size() > 0)
    {
        // Pop a value from the renderingQueue
        renderRectangle3 rectangle = renderingQueue.back();
        renderingQueue.pop_back();

        renderingThreads.push_back(
            std::async(
                std::launch::async,
                [this](PixelBuffer &buffer, renderRectangle3 rect)
                {
                    this->render(buffer, rect);
                },
                std::ref(pixelBuffer),
                rectangle));
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

    if (renderingThreads.size() == 0 && renderingQueue.size() == 0)
        return false;
    else
        return true;
}