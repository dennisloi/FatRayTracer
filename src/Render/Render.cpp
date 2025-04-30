#include "Render/Render.h"
#include "Render/PixelBuffer.h"

#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <iostream>
#include <algorithm>
#include <random> // Used to pick a random pixel for random rendering

// Default constructor
Render3::Render3(
    Camera3 camera_,
    std::vector<std::shared_ptr<SceneObject>> &objects_,
    int averages_,
    int maxReflections_,
    int resX_,
    int resY_,
    float antialiasing_,
    float gain_)
    : camera(camera_), objects(objects_), averages(averages_), maxReflections(maxReflections_), resX(resX_), resY(resY_), antialiasing(antialiasing_), gain(gain_) {}

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
    // Ray3 closestObjectReflection;
    std::shared_ptr<SceneObject> closestObject = nullptr;
    int closestTriangleIndex;
    bool hit = false;
    hitInfo hitI;
    

    for (int i = 0; i < maxReflections; i++)
    {
        hit = false;
        minDistance = std::numeric_limits<float>::max();

        for (std::shared_ptr<SceneObject> obj : objects) {

            hitInfo tempHitInfo;
            if (obj->Intersect(ray, tempHitInfo)){
                if (tempHitInfo.distance < minDistance){
                    minDistance = tempHitInfo.distance;
                    closestObject = obj;
                    hitI = tempHitInfo;
                    hit = true;
                }
            }
        }

        if (hit){
            
            // Check if is a light
            if (closestObject->material->emissivity > 0.0f)
            {
                Color c = ray.color * (closestObject->material->emissivity * gain);
                return c;
            }

            closestObject->Shading(ray, hitI, reflection);
            ray = reflection;
        }
        else{
            break;
        }
    }

    if (!hit)
    {
        return Color();
        return ray.color * getSkybox(ray);
    }

    // In case no return hit above (shouldn't happen? But I don't pay for characters)
    return Color();
}

Ray3 Render3::createRay(int x, int y, ProjectionType projection)
{
    Vector3 RayDirection;
    Vector3 RayOrigin;

    Vector3 forward = normalize(camera.direction);
    Vector3 right = normalize(cross(forward, camera.up));
    Vector3 up = normalize(camera.up);

    float ndcX = (x + 0.5f) / resX;
    float ndcY = (y + 0.5f) / resY;

    float screenX = (ndcX - 0.5f) * camera.width;
    float screenY = (0.5f - ndcY) * camera.height; // Flip Y axis

    if (projection == ProjectionType::Prospective)
    {
        Vector3 localDirection = forward * camera.focalLength
                                + right * screenX
                                + up * screenY;

        RayDirection = normalize(localDirection);

        RayDirection = normalize(RayDirection + getRandomDirection() * antialiasing);

        RayOrigin = camera.origin;
    }
    else if (projection == ProjectionType::Orthographic)
    {
        // In orthographic, direction is fixed
        RayDirection = camera.direction;

        // Apply antialiasing jitter to origin, not direction
        Vector3 jitter = getRandomDirection() * antialiasing;
        
        Vector3 forward = normalize(camera.direction);
        Vector3 right = normalize(cross(forward, camera.up));
        Vector3 up = cross(right, forward);

        RayOrigin = camera.origin + right * screenX + up * screenY + jitter;
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

void Render3::renderRand(PixelBuffer &pixelBuffer, int seed){

    std::mt19937 rng(seed);
    

    while(true){
        // TODO, use tiles, even if small (8x8?)

        // TODO, look into RAII-based scoped lock

        // Lock the pixelQueue
        pixelBuffer.pixelsQueueMutex.lock();

        // Check if the render has finished
        if (pixelBuffer.pixelsQueue.size() == 0) return;

        // Select a random pixel
        std::uniform_int_distribution<int> distI(0, pixelBuffer.pixelsQueue.size() - 1);
        int index = distI(rng);

        // Pop the pixel
        Pixel *pixel = pixelBuffer.pixelsQueue[index];
        pixelBuffer.pixelsQueue.erase(pixelBuffer.pixelsQueue.begin() + index);

        // Release the lock
        pixelBuffer.pixelsQueueMutex.unlock();

        // Render the pixel
        Ray3 ray = createRay(pixel->x, pixel->y, camera.projection);

        pixel->addSample(renderRay(ray));

        // Check if the samples limit has been reached
        if(pixel->getSample() >= pixelBuffer.samplesLimit) continue;

        // Check if the variance is below the threshold TODO!
        // if(pixelBuffer.variance[index] <= pixelBuffer.varianceThreshold) continue;

        // Lock the pixelQueue
        pixelBuffer.pixelsQueueMutex.lock();

        // Push the unfinished pixel back to the queue
        pixelBuffer.pixelsQueue.push_back(pixel);

        // Release the lock
        pixelBuffer.pixelsQueueMutex.unlock();

    }
}

void Render3::renderRandDispatcher(PixelBuffer &pixelBuffer, int numThreads)
{
    for(int i=0; i<numThreads; i++){

        // Generate a random seed
        std::random_device rd;
        int seed = rd();

        // Start the thread
        std::thread t(&Render3::renderRand, this, std::ref(pixelBuffer), seed);

        // Detach the thread
        t.detach();
    }
}


