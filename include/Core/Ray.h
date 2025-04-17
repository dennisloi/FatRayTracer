#ifndef RAY_H
#define RAY_H

#include "Vector.h"
#include "Utils/Color.h"

class Ray3
{
    public:
    
    Vector3 origin, direction;

    Color color = Color(1.f, 1.f, 1.f);

    // Default constructor
    Ray3();

    // Parametric constructor
    Ray3(Vector3 origin_, Vector3 direction_);

    void setDirection(const Vector3& direction_);

};

Color getSkybox(const Ray3& ray); // Prob to be moved from here

#endif // RAY_H