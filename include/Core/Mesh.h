#ifndef MESH_H
#define MESH_H

#include "Core/Triangle.h"
#include "Core/SceneObject.h"
#include "Core/Sphere.h"

#include <SFML/Graphics.hpp> // To be removed to use stb_image directly

#include <filesystem>

class Mesh3
{
    public:

    std::vector<std::shared_ptr<Triangle3>> triangles;
    sf::Image texture;
    Color colorMesh;
    // Sphere boundingSphere;

    bool hasTexture;

    // Default constructor
    Mesh3();

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

    Color getTexture(
        int x,
        int y
    );
};

#endif // MESH_H