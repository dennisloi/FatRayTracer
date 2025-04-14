#include "Render/PixelBuffer.h"

PixelBuffer::PixelBuffer(unsigned int width_, unsigned int height_)
    : width(width_), height(height_), pixels(width * height) {}

// Get dimensions
unsigned int PixelBuffer::getWidth() const
{
    return width;
}
unsigned int PixelBuffer::getHeight() const
{
    return height;
}

// Get pixels
const std::vector<Color> &PixelBuffer::getPixels() const
{
    return pixels;
}

// Set pixel
void PixelBuffer::setPixel(unsigned int x, unsigned int y, Color color)
{
    if (x < width && y < height)
    {
        pixels[y * width + x] = color; // Set pixel color at (x, y)
    }
}

// Clear buffer
void PixelBuffer::clearBuffer(){
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            pixels[y * width + x] = Color(0, 0, 0, 0);
        }
    }
}