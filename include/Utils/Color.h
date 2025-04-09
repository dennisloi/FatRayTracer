#ifndef COLOR_H
#define COLOR_H

class Color {
    public:

        unsigned char r, g, b, a;

        // Default constructor
        Color();

        // Parametric constructor
        Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);

        Color(int red, int green, int blue, int alpha);

        Color operator+(const Color& other) const;

        Color operator*(const Color& other) const;

        // bool operator=(const Color& other) const;

        Color operator*(const float& val) const;
        
};

Color averageColors(const Color& color0, const Color& color1);

// Color randomColor(); TODO

#endif // COLOR_H