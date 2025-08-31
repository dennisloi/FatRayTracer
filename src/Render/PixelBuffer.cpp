#include "Render/PixelBuffer.h"

#include <random>
#include <algorithm>

void Pixel::setColor(Color color){
    colorAcc = color;
}

Color Pixel::getColor(){
    if (samples == 0) return colorAcc;
    return colorAcc/samples;
}

void Pixel::addSample(Color color){
    colorAcc = colorAcc + color;
    samples++;
}

int Pixel::getSample(){
    return samples;
}

PixelBuffer::PixelBuffer(unsigned int width_, unsigned int height_)
    : width(width_), height(height_), pixels(width * height),
      pixelsQueue(), done(false)
{
}

// Get dimensions
unsigned int PixelBuffer::getWidth() const
{
    return width;
}
unsigned int PixelBuffer::getHeight() const
{
    return height;
}

// Set pixel
void PixelBuffer::setPixel(unsigned int x, unsigned int y, Color color)
{
    if (x < width && y < height)
    {
        pixels[y * width + x].setColor(color);
    }
}

// Clear buffer
void PixelBuffer::clearBuffer()
{
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            pixels[y * width + x].setColor(Color());
        }
    }
}

bool PixelBuffer::checkDone()
{
    if (pixelsQueue.size() == 0) return true;
    return false;
}

void PixelBuffer::fillQueue(){

    // Clean up the buffer
    pixelsQueue.clear();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Pixel *p = &pixels[x + y*width];
            p->x = x;
            p->y = y;
            pixelsQueue.push_back(p);
        }
    }

    // Shuffle the queue to randomize the order
    std::random_device rd;
    std::mt19937 rng(rd()); // or use std::random_device for nondeterministic seed
    std::shuffle(pixelsQueue.begin(), pixelsQueue.end(), rng);

}