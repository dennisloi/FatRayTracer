#include "Core/Vector.h"
#include "Core/Triangle.h"
#include "Core/Ray.h"
#include <stdexcept>
#include <random>

#include <cstdlib> // For rand(), srand()
#include <chrono>  // For high-resolution clock

// Constructor
Triangle3::Triangle3(Vector3 v0_, Vector3 v1_, Vector3 v2_)
    : v0(v0_), v1(v1_), v2(v2_)
{

    // The normal follows the right-hand rule, where the normal points on the side in which the vertex are declared counter-clockwise
    n = normalize(cross(v1_ - v0_, v2_ - v0_));
}

// Check intersection

bool Triangle3::Intersect(const Ray3 &ray, hitInfo &hitInfo)
{
    float normal_dot_dir = dot(n, ray.direction);

    // Backface culling: ignore if normal faces away from ray
    // Otherwise the reflection calculation are fucked-up
    // if (normal_dot_dir > 0)
    // {
    //     return false;
    // }

    // Check if the line is parallel to the plane
    if (std::abs(normal_dot_dir) < 1e-6)
    {
        return false;
    }

    float d = -dot(n, v0);

    float t = -(dot(n, ray.origin) + d) / normal_dot_dir;

    // If t is negative, the intersection point is behind the origin
    if (t < 0)
    {
        return false;
    }

    // Calculate the intersection point
    Vector3 intersection = ray.origin + ray.direction * t;

    // Alternate version which is triangle winding agnostic
    Vector3 edge0 = v1 - v0;
    Vector3 c0 = intersection - v0;
    float test0 = dot(n, cross(edge0, c0));

    Vector3 edge1 = v2 - v1;
    Vector3 c1 = intersection - v1;
    float test1 = dot(n, cross(edge1, c1));

    Vector3 edge2 = v0 - v2;
    Vector3 c2 = intersection - v2;
    float test2 = dot(n, cross(edge2, c2));

    // Check if all are same sign (>= 0 or <= 0)
    if ((test0 >= 0 && test1 >= 0 && test2 >= 0) ||
        (test0 <= 0 && test1 <= 0 && test2 <= 0))
    {
        // Intersection is inside triangle
        Vector3 distance = ray.origin - intersection;

        hitInfo.hitPoint = intersection;
        hitInfo.distance = abs(distance.getLength());

        return true;
    }

    return false;
}