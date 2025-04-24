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
    const std::vector<std::shared_ptr<Mesh3>> &objects_,
    int averages_,
    int maxReflections_,
    int resX_,
    int resY_,
    float antialiasing_,
    float gain_)
    : camera(camera_), objects(objects_), averages(averages_), maxReflections(maxReflections_), resX(resX_), resY(resY_), antialiasing(antialiasing_), gain(gain_)
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
    int closestTriangleIndex;
    bool hit = false;

    for (int i = 0; i < maxReflections; i++)
    {
        hit = false;
        minDistance = std::numeric_limits<float>::max();
        closestObjectIndex = -1;

        for (int j = 0; j < objects.size(); j++)
        {
            // Dereference the shared_ptr to access the Mesh3 object
            Mesh3& mesh = *objects[j];
            for (int k = 0; k < mesh.triangles.size(); k++)
            {
                if (mesh.triangles[k]->Intersect(ray, reflection))
                {
                    float distance = (reflection.origin - ray.origin).getLength();
                    if (distance < minDistance)
                    {
                        minDistance = distance;
                        closestObjectReflection = reflection;
                        closestObjectIndex = j;
                        closestTriangleIndex = k;
                    }
                    hit = true;
                }
            }
        }

        if (hit)
        {
            ray = closestObjectReflection;
            Mesh3& mesh = *objects[closestObjectIndex];
            Triangle3& triangle = *mesh.triangles[closestTriangleIndex];

            if(mesh.hasTexture){
                // temp

                Vector3 intersection = ray.origin;

                // Vector3 reference = triangle.v0;
                // // Determine the projection of the interseciton onto v0-v1
                // // The vector "a" is projected to the vector "b"
                // Vector3 a = intersection - reference;
                // Vector3 b = triangle.v1 - triangle.v0;
                // float p0 = dot(a, b) / b.getLength();
                // p0 = p0 / b.getLength();

                // // Determine the projection of the interseciton onto v0-v2
                // a = intersection - reference;
                // b = triangle.v2 - triangle.v0;
                // float p1 = dot(a, b) / b.getLength();
                // p1 = p1 / b.getLength();

                // Vector2 ta = triangle.t0;
                // Vector2 tc = (triangle.t1 - ta) * p0;
                // Vector2 te = (triangle.t2 - ta) * p1;

                // float a1 = tc.y - ta.y;
                // float a2 = te.y - ta.y;
                // float b1 = tc.x - ta.x;
                // float b2 = te.x - ta.x;

                // float D = a1*b2 - a2*b1;
                // // TODO check if D = 0

                // float t = (te.x-tc.x)*(-b2) - (te.y-tc.y)*(a2);
                // t = t/D;

                // float intX = tc.x - a1*t;
                // float intY = tc.y + b1*t;

                Vector3 v0 = triangle.v0;
                Vector3 v1 = triangle.v1;
                Vector3 v2 = triangle.v2;
                Vector3 p = intersection; // intersection point

                Vector3 v0v1 = v1 - v0;
                Vector3 v0v2 = v2 - v0;
                Vector3 v0p  = p - v0;

                float d00 = dot(v0v1, v0v1);
                float d01 = dot(v0v1, v0v2);
                float d11 = dot(v0v2, v0v2);
                float d20 = dot(v0p, v0v1);
                float d21 = dot(v0p, v0v2);

                float denom = d00 * d11 - d01 * d01;
                float v = (d11 * d20 - d01 * d21) / denom;
                float w = (d00 * d21 - d01 * d20) / denom;
                float u = 1.0f - v - w;

                Vector2 texCoord = triangle.t0 * u + triangle.t1 *v + triangle.t2 * w;

                int coordX = texCoord.x * mesh.texture.getSize().x;
                int coordY = texCoord.y * mesh.texture.getSize().y;


                Color textureColor = mesh.getTexture(coordX, coordY);
                ray.color = ray.color * textureColor;
                // ray.color = Color(u, v, w);  // just for debug
            }
            else {
                ray.color = ray.color * mesh.colorMesh;
            }

            if (triangle.emissivity > 0.0f)
            {
                Color c = ray.color * (triangle.emissivity * gain);
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