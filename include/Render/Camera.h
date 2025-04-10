#ifndef CAMERA_H
#define CAMERA_H

#include "Core/Vector.h"
#include "Core/Sphere.h"
#include "Render/PixelBuffer.h"
#include "Utils/Color.h"
#include <memory>

class Camera3
{
public:
    // Origin and direction
    Vector3 origin, direction;

    // Focal length
    float focalLength;

    // Canvas size
    float width, height;

    // Constructor
    Camera3(Vector3 origin_, Vector3 direction_, float focalLength_, float width_, float height_);

    void render(PixelBuffer &pixelBuffer,
    std::vector<std::shared_ptr<SceneObject>> &objects,
    int numThreads);

private:
    Color renderPixel(
        const int x,const int y,
        const float xStep, float yStep,
        const float xResolution, float yResolution,
        PixelBuffer &pixelBuffer,
        const std::vector<std::shared_ptr<SceneObject>> &objects);

    void renderRows(
        int startY, int endY,
        float xStep, float yStep,
        float xResolution, float yResolution,
        PixelBuffer &pixelBuffer,
        const std::vector<std::shared_ptr<SceneObject>> &objects);
};

#endif // CAMERA_H