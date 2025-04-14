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
    int startX, int endX, int startY, int endY);

private:

};

#endif // CAMERA_H