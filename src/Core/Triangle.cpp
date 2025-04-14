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

    // Check for transparency
    static thread_local std::mt19937 gen(std::random_device{}());
    static thread_local std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    if (dist(gen) < transparency)
        {
            reflection.origin = intersection + ray.direction * 10e-6; //TODO: I had to increase this from 1e-6, why?
            reflection.direction = ray.direction;
            reflection.color = ray.color * color * (transparency + 1.0f);
            return true;
        }

    // Specular reflection ray (shiny object)
    Vector3 direction = ray.direction - n * (2 * dot(ray.direction, n));

    // Add roughness

    // Sample a random direction in the hemisphere
    Vector3 randomSample = getRandomDirectionInHemisphere(n*-1);

    // Bias the random ray to the normal
    // randomSample = normalize(randomSample + n*0.5f); //Removed because I get darker and noisier results  

    // Blend the perfect reflection direction with the random sample
    float roughnessFactor = roughness * roughness;  // Square to make it sharper
    direction = normalize(lerp(direction, randomSample, roughnessFactor));

    direction = normalize(direction);

    reflection.origin = intersection + direction * 1e-6;
    reflection.direction = direction;

    reflection.color = ray.color * color;

    return true;
}