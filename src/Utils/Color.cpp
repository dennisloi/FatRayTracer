
#include "Utils/Color.h"

// Default constructor initialize a black color
Color::Color()
    : r(0), g(0), b(0), a(255) {}

// Parametric constructor
Color::Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
    : r(red), g(green), b(blue), a(alpha) {}

Color::Color(int red, int green, int blue, int alpha)
    : r((unsigned char)red), g((unsigned char)green), b((unsigned char)blue), a((unsigned char)alpha) {}

Color Color::operator+(const Color &other) const
{
    unsigned char red = (r + other.r > 255) ? 255 : r + other.r;
    unsigned char green = (g + other.g > 255) ? 255 : g + other.g;
    unsigned char blue = (b + other.b > 255) ? 255 : b + other.b;
    unsigned char alpha = (a + other.a > 255) ? 255 : a + other.a;
    return Color(red, green, blue, alpha);
}

Color Color::operator*(const Color &other) const
{
    unsigned char red   = (unsigned char)r * other.r / 255;
    unsigned char green = (unsigned char)g * other.g / 255;
    unsigned char blue  = (unsigned char)b * other.b / 255;
    unsigned char alpha = (unsigned char)a * other.a / 255;

    return Color(red, green, blue, alpha);
}

Color Color::operator*(const float& val) const
{
    unsigned char red   = (unsigned char)r * val;
    unsigned char green = (unsigned char)g * val;
    unsigned char blue  = (unsigned char)b * val;
    unsigned char alpha = (unsigned char)a * val;

    return Color(red, green, blue, alpha);
}

// bool Color::operator=(const Color& other) const {
//     if  (r != other.r) return false;
//     if  (g != other.g) return false;
//     if  (b != other.b) return false;
//     if  (a != other.a) return false;
//     return true;
// }

Color averageColors(const Color& color0, const Color& color1){
    int red   = (int)color0.r + (int)color1.r;
    int green = (int)color0.g + (int)color1.g;
    int blue  = (int)color0.b + (int)color1.b;
    int alpha = (int)color0.a + (int)color1.a;

    return Color(
        (unsigned char) (float) (red / 2),
        (unsigned char) (float) (green / 2),
        (unsigned char) (float) (blue / 2),
        (unsigned char) (float) (alpha / 2)
    );
}

