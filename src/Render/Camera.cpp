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