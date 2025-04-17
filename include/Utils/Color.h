#ifndef COLOR_H
#define COLOR_H

class Color {
    public:

        float r, g, b, a;

        // Default constructor
        Color();

        // Parametric constructor
        Color(float red, float green, float blue);

        Color operator+(const Color& other) const;

        Color operator*(const Color& other) const;

        // bool operator=(const Color& other) const;

        Color operator*(const float& val) const;

        Color operator/(const float& val) const;
        
};

Color averageColors(const Color& color0, const Color& color1);

// Color randomColor(); TODO

#endif // COLOR_H