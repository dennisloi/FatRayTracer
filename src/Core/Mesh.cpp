
#include "Core/Mesh.h"

#include <fstream>
#include <iostream>
#include <filesystem>
#include "Core/Mesh.h"

// Constructor
Mesh3::Mesh3()
{
    // Allocate memory for the material
    material = new Material();
}

bool Mesh3::loadSTL(
    const std::filesystem::path &fileName,
    Vector3 origin,
    Vector3 direction,
    Vector3 scale,
    float roughness,
    float emissivity,
    float transparency,
    Color color)
{
    // Open the STL file
    std::ifstream file(fileName.string(), std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return false;
    }

    // Read the header (80 bytes)
    char header[80];
    file.read(header, 80);
    // std::cout << "Header: " << header << std::endl;

    bool isASCII = std::string(header, header + 5) == "solid";
    if (isASCII)
    {
        std::cerr << "Error: ASCII STL detected. This loader only supports binary STL." << std::endl;
        return false;
    }

    // Read the number of triangles (4 bytes)
    unsigned int numTriangles;
    file.read(reinterpret_cast<char *>(&numTriangles), sizeof(unsigned int));
    std::cout << "Number of triangles: " << numTriangles << std::endl;

    // Read the triangles
    for (int i = 0; i < numTriangles; i++)
    {
        float normal_x;
        float normal_y;
        float normal_z;
        float v1_x;
        float v1_y;
        float v1_z;
        float v2_x;
        float v2_y;
        float v2_z;
        float v3_x;
        float v3_y;
        float v3_z;
        char byteCount[2];

        file.read(reinterpret_cast<char *>(&normal_x), sizeof(float));
        file.read(reinterpret_cast<char *>(&normal_z), sizeof(float));
        file.read(reinterpret_cast<char *>(&normal_y), sizeof(float));
        file.read(reinterpret_cast<char *>(&v1_x), sizeof(float));
        file.read(reinterpret_cast<char *>(&v1_z), sizeof(float));
        file.read(reinterpret_cast<char *>(&v1_y), sizeof(float));
        file.read(reinterpret_cast<char *>(&v2_x), sizeof(float));
        file.read(reinterpret_cast<char *>(&v2_z), sizeof(float));
        file.read(reinterpret_cast<char *>(&v2_y), sizeof(float));
        file.read(reinterpret_cast<char *>(&v3_x), sizeof(float));
        file.read(reinterpret_cast<char *>(&v3_z), sizeof(float));
        file.read(reinterpret_cast<char *>(&v3_y), sizeof(float));
        file.read(byteCount, 2);

        // Invert the Y axis
        v1_y = -v1_y;
        v2_y = -v2_y;
        v3_y = -v3_y;

        // Flip the normal
        normal_x = -normal_x;
        normal_z = -normal_z;

        // // Debug print
        // std::cout << "Triangle n:" << i  << std::endl;
        // std::cout << "Normal: (" << normal_x << ", " << normal_y << ", " << normal_z << ")" << std::endl;
        // std::cout << "Vertex 1: (" << v1_x << ", " << v1_y << ", " << v1_z << ")" << std::endl;
        // std::cout << "Vertex 2: (" << v2_x << ", " << v2_y << ", " << v2_z << ")" << std::endl;
        // std::cout << "Vertex 3: (" << v3_x << ", " << v3_y << ", " << v3_z << ")" << std::endl;
        // std::cout << "Byte count: " << (unsigned int)byteCount[0] << std::endl;

        // Scale the vectors
        v1_x = v1_x * scale.x + origin.x;
        v1_y = v1_y * scale.y + origin.y;
        v1_z = v1_z * scale.z + origin.z;
        v2_x = v2_x * scale.x + origin.x;
        v2_y = v2_y * scale.y + origin.y;
        v2_z = v2_z * scale.z + origin.z;
        v3_x = v3_x * scale.x + origin.x;
        v3_y = v3_y * scale.y + origin.y;
        v3_z = v3_z * scale.z + origin.z;

        Triangle3 triangle = Triangle3(
            Vector3(v2_x, v2_y, v2_z),
            Vector3(v1_x, v1_y, v1_z),
            Vector3(v3_x, v3_y, v3_z));

        triangle.n = normalize(Vector3(normal_x, normal_y, normal_z));

        // // Triangle settings
        // triangle.color = color,
        // triangle.roughness = roughness;
        // triangle.emissivity = emissivity;
        // triangle.transparency = transparency;

        triangles.push_back(std::make_shared<Triangle3>(triangle));

        material->hasTexture = false;
        // colorMesh = color;
    }
    return true;
};

bool Mesh3::loadOBJ(
    const std::filesystem::path &fileName,
    Vector3 origin,
    Vector3 direction,
    Vector3 scale,
    float roughness,
    float emissivity,
    float transparency,
    Color color)
{

    std::vector<Vector3> vertexes;
    std::vector<Vector2> textures;
    std::vector<Vector3> normals;

    // Open the OBJ file
    std::ifstream file(fileName.string(), std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return false;
    }

    std::string line;
    Vector3 v;
    Vector2 t;
    float hasTexture_ = false;
    while (std::getline(file, line))
    {
        std::istringstream iss;
        iss.str(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v")
        {
            iss >> v.x >> v.y >> v.z;

            // Scale the vertexes
            v.x *= scale.x;
            v.y *= scale.y;
            v.z *= scale.z;

            vertexes.push_back(v);
        }

        else if (prefix == "vn")
        {
            iss >> v.x >> v.y >> v.z;
            normals.push_back(v);
        }

        else if (prefix == "vt")
        {
            iss >> t.x >> t.y;
            textures.push_back(t);
            hasTexture_ = true;
        }

        else if (prefix == "f")
        {

            std::vector<Vector3> faceVertexes;
            std::vector<Vector2> faceTextures;
            Vector3 normal;

            std::string token;

            while (iss >> token)
            {
                size_t first = token.find('/');
                size_t second = token.find('/', first + 1);

                int firstIndex = std::stoi(token.substr(0, first)) - 1;
                int thirdIndex = std::stoi(token.substr(second + 1)) - 1;

                if (!hasTexture_)
                {
                    faceVertexes.push_back(vertexes[firstIndex]);
                    normal = normals[thirdIndex];
                }
                else
                {
                    int secondIndex = std::stoi(token.substr(first + 1, second)) - 1;
                    faceVertexes.push_back(vertexes[firstIndex]);
                    faceTextures.push_back(textures[secondIndex]);
                    normal = normals[thirdIndex];
                }
            }

            if (faceVertexes.size() > 3)
            {
                std::cerr << "Not triangulated mesh!" << std::endl;
                return false;
            }

            Vector3 v1 = faceVertexes[0];
            Vector3 v2 = faceVertexes[1];
            Vector3 v3 = faceVertexes[2];

            // Move the vertex to the origin
            v1 = v1 + origin;
            v2 = v2 + origin;
            v3 = v3 + origin;

            Triangle3 triangle = Triangle3(v1, v2, v3);
            triangle.n = normalize(normal);

            // Set texture vertexes if present
            if (hasTexture_)
            {
                Vector2 t0 = faceTextures[0];
                Vector2 t1 = faceTextures[1];
                Vector2 t2 = faceTextures[2];

                triangle.t0 = t0;
                triangle.t1 = t1;
                triangle.t2 = t2;
            }

            triangles.push_back(std::make_shared<Triangle3>(triangle));
        }
    }

    material->baseColor = color;
    material->roughness = roughness;
    material->emissivity = emissivity;

    return true;
};

bool Mesh3::loadTexture(
    const std::filesystem::path &fileName)
{
    if (!material->texture.loadFromFile(fileName.string()))
    {
        std::cerr << "Error loading image!" << std::endl;
        return false; // Exit with an error code
    }
    material->hasTexture = true;
    return true;
}

bool Mesh3::Intersect(const Ray3 &ray, hitInfo &hitI)
{
    bool hit = false;
    float minDistance = std::numeric_limits<float>::max();
    int closestTriangleIndex = -1;
    Ray3 closestObjectReflection;

    for (int i = 0; i < triangles.size(); i++)
    {
        hitInfo tempHitI;
        if (triangles[i]->Intersect(ray, tempHitI))
        {
            if (tempHitI.distance < minDistance)
            {
                minDistance = tempHitI.distance;
                closestTriangleIndex = i;
                hitI = tempHitI;
            }
            hit = true;
        }
    }

    if (hit)
    {
        hitI.hitObjectIndex = closestTriangleIndex;
        hitI.distance = minDistance;
        return true;
    }
    else
    {
        return false;
    }
}

void Mesh3::Shading(
    const Ray3 &incomingRay,
    const hitInfo &hitInfo,
    Ray3 &reflection)
{

    Triangle3 &triangle = *triangles[hitInfo.hitObjectIndex];

    reflection.origin = hitInfo.hitPoint;
    Vector3 normal = triangle.n;

    // Specular reflection ray (shiny object)
    Vector3 direction = incomingRay.direction - normal * (2 * dot(incomingRay.direction, normal));

    // Roughness
    if (material->roughness > 0)
    {

        // Sample a random direction in the hemisphere
        Vector3 randomSample = getRandomDirectionInHemisphere(normal);

        // Blend the perfect reflection direction with the random sample
        float roughnessFactor = material->roughness * material->roughness; // Square to make it sharper
        direction = normalize(lerp(direction, randomSample, roughnessFactor));
    }

    // Bump the reflected origin to avoid self-intersections
    reflection.origin = reflection.origin + normal * 1e-3;
    reflection.direction = direction;

    if (material->hasTexture)
    {
        // UV Texture mapping
        Vector3 v0 = triangle.v0;
        Vector3 v1 = triangle.v1;
        Vector3 v2 = triangle.v2;
        Vector3 p = hitInfo.hitPoint;

        Vector3 v0v1 = v1 - v0;
        Vector3 v0v2 = v2 - v0;
        Vector3 v0p = p - v0;

        float d00 = dot(v0v1, v0v1);
        float d01 = dot(v0v1, v0v2);
        float d11 = dot(v0v2, v0v2);
        float d20 = dot(v0p, v0v1);
        float d21 = dot(v0p, v0v2);

        float denom = d00 * d11 - d01 * d01;
        float v = (d11 * d20 - d01 * d21) / denom;
        float w = (d00 * d21 - d01 * d20) / denom;
        float u = 1.0f - v - w;

        Vector2 texCoord = triangle.t0 * u + triangle.t1 * v + triangle.t2 * w;

        int coordX = texCoord.x * material->texture.getSize().x;
        int coordY = texCoord.y * material->texture.getSize().y;

        Color textureColor = material->getTexture(coordX, coordY);
        reflection.color = incomingRay.color * textureColor;
    }
    else
    {
        reflection.color = incomingRay.color * material->baseColor;
    }

}