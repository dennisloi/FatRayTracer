#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "Core/Vector.h"
#include "Core/Ray.h"
#include "Utils/Color.h"
#include "Core/Material.h"
#include "Core/HitInfo.h"
#include <random>

class SceneObject {

    public:

        Material* material;
        
        // Checks if a ray intersects with the triangle (Abstract method)
        virtual bool Intersect(const Ray3& ray, hitInfo& hitInfo) = 0;
        virtual void Shading(const Ray3& ray, const hitInfo& hitInfo, Ray3& reflection)  = 0;

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