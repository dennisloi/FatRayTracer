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

Color getSkybox(const Ray3& ray){
    
    // If the y direction is negative, return the sky
    if(ray.direction.y < 0){
        return Color(0, 5, 40, 0);
    }
    return Color(30, 10, 0, 0);
}