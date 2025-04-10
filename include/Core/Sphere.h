#ifndef SPHERE_H
#define SPHERE_H

#include "Core/SceneObject.h"
#include "Core/Vector.h"
#include "Core/Ray.h"

class Sphere : public SceneObject
{

public:

    Vector3 Origin;

    float radius;

    // Declaration of the constructor
    Sphere(Vector3 Origin_, float radius_);

    bool Intersect(const Ray3& ray, Ray3& reflection) const override;

};

#endif // SPHERE_H