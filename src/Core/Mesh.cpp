
#include "Core/Mesh.h"

#include <fstream>
#include <iostream>
#include <filesystem>

// Constructor
Mesh3::Mesh3() {}

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

        // Triangle settings
        triangle.color = color,
        triangle.roughness = roughness;
        triangle.emissivity = emissivity;
        triangle.transparency = transparency;

        triangles.push_back(std::make_shared<Triangle3>(triangle));
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
    std::vector<Vector3> normals;

    // Open the OBJ file
    std::ifstream file(fileName.string(), std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss;
        iss.str(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            Vector3 v;
            iss >> v.x >> v.z >> v.y;
            
            // Scale the vertexes
            v.x *= scale.x;
            v.y *= scale.y;
            v.z *= scale.z;

            vertexes.push_back(v);
        }
        
        else if(prefix == "vn"){
            Vector3 v;
            iss >> v.x >> v.z >> v.y;
            normals.push_back(v);
        }

        else if(prefix == "f"){

            std::vector<Vector3> faceVertexes;
            Vector3 normal;

            std::string token;

            while(iss >> token){
                size_t first = token.find('/');
                size_t second = token.find('/', first + 1);

                int firstIndex = std::stoi(token.substr(0, first)) - 1;
                int secondIndex = std::stoi(token.substr(second + 1)) - 1;

                faceVertexes.push_back(vertexes[firstIndex]);
                normal = normals[secondIndex];
            }
            Vector3 v1 = faceVertexes[0];
            Vector3 v2 = faceVertexes[1];
            Vector3 v3 = faceVertexes[2];

            // Flip Y axis
            v1.y = -v1.y;
            v2.y = -v2.y;
            v3.y = -v3.y;

            normal.x = -normal.x;
            // normal.y = -normal.y;
            normal.z = -normal.z;

            Triangle3 triangle = Triangle3(v2, v1, v3);
            triangle.n = normalize(normal);
            
            // Triangle settings
            triangle.color = color,
            triangle.roughness = roughness;
            triangle.emissivity = emissivity;
            triangle.transparency = transparency;

            triangles.push_back(std::make_shared<Triangle3>(triangle));
        }
    }

    return true;
};
