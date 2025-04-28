#ifndef MESH_H
#define MESH_H

#include "Core/Triangle.h"
#include "Core/SceneObject.h"
#include "Core/Sphere.h"

#include <SFML/Graphics.hpp> // To be removed to use stb_image directly

#include <filesystem>

class Mesh3: public SceneObject 
{
    public:

    std::vector<std::shared_ptr<Triangle3>> triangles;

    // Sphere boundingSphere;

    // Default constructor
    Mesh3();

    bool Intersect(const Ray3& ray, hitInfo& hitInfo) override;
    void Shading(const Ray3& ray, const hitInfo& hitInfo, Ray3& reflection) override;

    bool loadSTL(
    const std::filesystem::path &fileName,
    Vector3 origin,
    Vector3 direction,
    Vector3 scale,
    float roughness = 0.0f,
    float emissivity = 0.0f,
    float transparency = 0.0f,
    Color color = Color());

    bool loadOBJ(
    const std::filesystem::path &fileName,
    Vector3 origin,
    Vector3 direction,
    Vector3 scale,
    float roughness = 0.0f,
    float emissivity = 0.0f,
    float transparency = 0.0f,
    Color color = Color());

    bool loadTexture(
        const std::filesystem::path &fileName
    );
    
};

#endif // MESH_H