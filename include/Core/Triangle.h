#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Core/SceneObject.h"
#include "Core/Vector.h"
#include "Core/Ray.h"
#include "Core/HitInfo.h"

class Triangle3
{

public:

    // Vertexes
    Vector3 v0, v1, v2;

    // Texture UVs
    Vector2 t0, t1, t2;

    //Normal vector
    Vector3 n;

    // Declaration of the constructor
    Triangle3(Vector3 v0_, Vector3 v1_, Vector3 v2_);

    bool Intersect(const Ray3& ray, hitInfo& hitInfo);


};

#endif // TRIANGLE_H