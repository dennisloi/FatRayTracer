#include "Core/Ray.h"
#include <stdexcept>

// Default constructor
Ray3::Ray3()
    : origin(Vector3(0.0f, 0.0f, 0.0f)),
      direction(Vector3(0.0f, 0.0f, 1.0f)) // Default direction along +Z axis
{
}

// Parameterized constructor
Ray3::Ray3(Vector3 origin_, Vector3 direction_)
    : origin(origin_)
{
    // Ensure the direction is normalized
    direction = normalize(direction_);
}

void Ray3::setDirection(const Vector3& direction_) {
    direction = normalize(direction_);
}

// TODO move this to the scene
Color getSkybox(const Ray3& ray){

    Color c1 = Color(0.12f, 0.06f, 0.06f); // "Bottom" color
    Color c0 = Color(0.0f, 0.2f, 1.0f); // "Top" color
    
    float r = ((c1.r - c0.r)/2) * ray.direction.y + ((c1.r + c0.r)/2);
    float g = ((c1.g - c0.g)/2) * ray.direction.y + ((c1.g + c0.g)/2);
    float b = ((c1.b - c0.b)/2) * ray.direction.y + ((c1.b + c0.b)/2);
    
    return Color(r, g, b);
}