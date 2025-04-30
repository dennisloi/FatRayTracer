
#include "Utils/Color.h"
#include <cmath>

// Default constructor initialize a black color
Color::Color()
    : r(0), g(0), b(0), a(255) {}

// Parametric constructor
Color::Color(float red, float green, float blue)
    : r(red), g(green), b(blue) {}

Color Color::operator+(const Color &other) const
{
    float red = (r + other.r > 255) ? 255 : r + other.r;
    float green = (g + other.g > 255) ? 255 : g + other.g;
    float blue = (b + other.b > 255) ? 255 : b + other.b;
    return Color(red, green, blue);
}

Color Color::operator*(const Color &other) const
{
    float red   = r * other.r;
    float green = g * other.g;
    float blue  = b * other.b;

    return Color(red, green, blue);
}

Color Color::operator*(const float& val) const
{
    float red   = r * val;
    float green = g * val;
    float blue  = b * val;

    return Color(red, green, blue);
}

Color Color::operator/(const float& val) const
{
    float red   = r / val;
    float green = g / val;
    float blue  = b / val;

    return Color(red, green, blue);
}


Color averageColors(const Color& color0, const Color& color1){
    float red   = color0.r + color1.r;
    float green = color0.g + color1.g;
    float blue  = color0.b + color1.b;

    return Color(
        (float) (red / 2),
        (float) (green / 2),
        (float) (blue / 2)
    );
}

float colorDistance(const Color& color0, const Color& color1){
    float red   = color0.r - color1.r;
    float green = color0.g - color1.g;
    float blue  = color0.b - color1.b;

    return std::sqrt(red*red + green*green + blue*blue);
}