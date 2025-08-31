#include "Core/Material.h"

Color Material::getTexture(
    int x,
    int y
){
    // Check bounds
    if (x<0) x = 0;
    if (x>texture.getSize().x - 1) x = texture.getSize().x - 1;
    if (y<0) y = 0;
    if (y>texture.getSize().y - 1) y = texture.getSize().y - 1;

    // Get the color
    sf::Color color = texture.getPixel(x, y);

    // Transform the color from SFML
    return Color((float)color.r/255, (float)color.g/255, (float)color.b/255);

}