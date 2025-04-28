#include "Core/Sphere.h"
#include "Core/Ray.h"

#include <stdexcept>

// Constructor
// Sphere::Sphere(Vector3 Origin_, float radius_)
//     : Origin(Origin_), radius(radius_) {}

// Check intersection
// bool Sphere::Intersect(const Ray3 &ray, Ray3 &reflection) const
// {

//     // Calculate the vector from the ray origin to the sphere center
//     Vector3 L = Origin - ray.origin; // Sphere's center - Ray's origin

//     // Calculate the projection of L onto the ray direction
//     float tc = dot(L, ray.direction);

//    [main] Building folder: /home/dennis/Projects/FatRayTracer/build all

 // Check if the sphere is behind the ray
//     if (tc < 0.0f)
//         return false; // The sphere is behind the ray

//     // Calculate the squared distance from the sphere center to the projection point
//     float d2 = dot(L, L) - tc * tc;

//     // Check if the distance squared is greater than the radius squared
//     float radius2 = radius * radius;
//     if (d2 > radius2)
//         return false; // No intersection

//     // Calculate the distance from the projection point to the intersection points
//     float t1c = sqrt(radius2 - d2); // Distance from the projection to the intersection points

//     // Calculate the intersection points
//     float t1 = tc - t1c; // First intersection point
//     float t2 = tc + t1c; // Second intersection point

//     // Choose the closest positive intersection point
//     Vector3 intersection = Vector3();
//     if (t1 > 0)
//     {
//         intersection = ray.origin + ray.direction * t1; // First intersection point
//     }
//     else if (t2 > 0)
//     {
//         intersection = ray.origin + ray.direction * t2; // Second intersection point
//     }
//     else
//     {
//         return false; // Both intersections are behind
//     }

//     // Calculate the normal
//     Vector3 normal = normalize(intersection - Origin);

//     // Specular reflection ray
//     Vector3 direction = ray.direction - normal * (2 * dot(ray.direction, normal));

//     // Add roughness

//     // Sample a random direction in the hemisphere
//     Vector3 randomSample = getRandomDirectionInHemisphere(normal);
    
//     // Blend the perfect reflection direction with the random sample
//     direction = normalize(lerp(direction, randomSample, roughness));
//     // direction = normalize(direction * (1.0f - roughness) + randomSample * roughness);

//     reflection.origin = intersection + direction * 1e-6;
//     reflection.direction = direction;

//     reflection.color = ray.color * color;

//     return true; // Intersection occurred
// }
