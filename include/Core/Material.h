#ifndef MATERIAL_H
#define MATERIAL_H

#include "Core/Vector.h"
#include "Core/Ray.h"
#include "Utils/Color.h"
#include "Core/Material.h"

#include <SFML/Graphics.hpp>

#include <random>

class Material {

public:
    Color baseColor;
    float roughness;
    float emissivity;

    bool hasTexture;
    bool hasNormalMap;

    sf::Image texture;
    sf::Image normalMap;

    // Constructor
    Material()
        : baseColor(Color(.5f, .5f, .5f)),
          roughness(0.5f),
          emissivity(0.0f),
          hasTexture(false),
          hasNormalMap(false)
    {}

    Color getTexture(
        int x,
        int y
    );
};



#endif //MATERIAL_H