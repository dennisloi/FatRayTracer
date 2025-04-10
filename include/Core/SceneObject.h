#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "Core/Vector.h"
#include "Core/Ray.h"
#include "Utils/Color.h"
#include <random>

class SceneObject {

    public:

    Color color = Color();

    float emissivity = 0.0f;

    float roughness = 0.0f;

    float transparency = 0.0f;
    
    // Checks if a ray intersects with the triangle (Abstract method)
    virtual bool Intersect(const Ray3& ray, Ray3& reflection) const = 0;

    // Destructor
    //virtual ~SceneObject(); //TODO

    // Color ReflectedColor(Ray3& ray);

    protected:
    // Mersenne Twister random number generator
    mutable std::mt19937 generator;

    // Distribution for generating floats in the range [min, max]
    mutable std::uniform_real_distribution<float> distribution;
};

#endif