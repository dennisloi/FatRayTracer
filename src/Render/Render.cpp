#include "Render/Render.h"
#include "Render/PixelBuffer.h"

#include <thread>
#include <vector>
#include <mutex>
#include <atomic>

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

void Render3::createRenderQueue(
    int divsX, int divsY)
{
    // Clear renderere queue
    renderingQueue.clear();

    // TODO, check for fractional divisions! May cause part of the image to not be rendered
    int xStep = resX / divsX;
    int yStep = resY / divsY;

    // TODO, fancy spiral queue to render the center first?
    for (int y = divsY - 1; y >= 0; y--)
    {
        for (int x = divsX - 1; x >= 0; x--)
        {
            int startX = x * xStep;
            int startY = y * yStep;
            renderingQueue.push_back(renderRectangle3(startX, startX + xStep, startY, startY + yStep));
            // std::cout << "startX: " << startX << ", endX: " << startX+xStep << ", startY: " << startY << ", endY: " << startY+yStep << std::endl;
        }
    }
}

Color Render3::renderRay(Ray3 ray)
{
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
                Color c = ray.color * objects[closestObjectIndex]->emissivity * 2; // TODO why 2?
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

Ray3 Render3::createRay(int x, int y)
{
    // TODO: ortoview?

    // Prospectic
    float ndcX = (x + 0.5f) / resX; // normalized device coordinates [0,1]
    float ndcY = (y + 0.5f) / resY;

    float screenX = (ndcX - 0.5f) * camera.width;
    float screenY = (ndcY - 0.5f) * camera.height; //(0.5f - ndcY) will flip Y

    Vector3 RayDirection = normalize(Vector3(screenX, screenY, camera.focalLength));

    RayDirection = normalize(RayDirection + getRandomDirection() * antialiasing);

    Vector3 RayOrigin = camera.origin;
    return Ray3(RayOrigin, RayDirection);
}

void Render3::render(PixelBuffer &pixelBuffer, renderRectangle3 &rectangle)
{
    // Render the specified rectangle

    // Mark the rectangle as rendering
    rectangle.rendering = true;
    for (int y = rectangle.startY; y < rectangle.endY; y++)
    {
        for (int x = rectangle.startX; x < rectangle.endX; x++)
        {
            float r = 0.f;
            float g = 0.f;
            float b = 0.f;

            // Generate the ray
            Ray3 ray = createRay(x, y);

            for (int i = 0; i < averages; i++)
            {
                // Render the ray
                Color pixelColor = renderRay(ray);

                r += pixelColor.r;
                g += pixelColor.g;
                b += pixelColor.b;
            }

            // Divide by the averages
            r = r / averages;
            g = g / averages;
            b = b / averages;

            Color finalColor = Color(r, g, b);

            pixelBuffer.setPixel(x, y, finalColor);
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

    if (renderingThreads.size() == 0)
        return false;
    else
        return true;
}