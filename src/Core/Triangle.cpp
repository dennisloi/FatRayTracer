#include "Core/Vector.h"
#include "Core/Triangle.h"
#include "Core/Ray.h"
#include <stdexcept>
#include <random>

#include <cstdlib>  // For rand(), srand()
#include <chrono>   // For high-resolution clock

// Constructor
Triangle3::Triangle3(Vector3 v0_, Vector3 v1_, Vector3 v2_)
    : v0(v0_), v1(v1_), v2(v2_) {

        // The normal follows the right-hand rule, where the normal points on the side in which the vertex are declared counter-clockwise
        n = normalize(cross(v1_ - v0_, v2_ - v0_));
    }

// Check intersection

bool Triangle3::Intersect(const Ray3& ray, Ray3& reflection) const
{
    float normal_dot_dir = dot(n, ray.direction);

    // Check if the line is parallel to the plane
    if (normal_dot_dir < 1e-6) {
        return false;
    }

    float d = -dot(n, v0);

    float t = -(dot(n, ray.origin) + d)/ normal_dot_dir;
    
    // Compute the independent value (t) of the line's formula at the intersection
    // with the plane
    // float t = dot(n, v0 - ray.origin) / normal_dot_dir;

    // If t is negative, the intersection point is behind the origin
    if (t < 0) {
        return false;
    }

    // Calculate the intersection point
    Vector3 intersection = ray.origin + ray.direction * t;
    
    //Edges tests to check if the intersection point is inside the triangle
    Vector3 edge0 = v1 - v0;
    Vector3 c0 = intersection - v0;
    if (dot(n, cross(edge0, c0)) < 0) return false;

    Vector3 edge1 = v2 - v1;
    Vector3 c1 = intersection - v1;
    if (dot(n, cross(edge1, c1)) < 0) return false;
    
    Vector3 edge2 = v0 - v2;
    Vector3 c2 = intersection - v2;
    if (dot(n, cross(edge2, c2)) < 0) return false;

    // If all the checks fails, the ray intersects the triangle

    // Specular reflection ray (shiny object)
    Vector3 direction = ray.direction - n * (2 * dot(ray.direction, n));

    // Add roughness
    // Get the current time in nanoseconds since epoch
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    srand((unsigned int) duration.count());
    Vector3 randomDirection = getRandomDirection() * roughness;
    direction = direction + randomDirection;

    // // If the dot product between the random vector and the normal is negative, flip it so it points outwards
    if (dot(n, direction) > 0) // ???? This works only if I change the sign
    {
        direction = direction * -1;
    }

    direction = normalize(direction);

    reflection.origin = intersection + direction * 1e-6;
    reflection.direction = direction;

    reflection.color = ray.color * color;

    return true;
}