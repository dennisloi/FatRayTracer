#ifndef PixelBuffer_H
#define PixelBuffer_H

#include <vector>
#include "Utils/Color.h"
#include <mutex>
#include <atomic>

class Pixel
{
public:

    int x, y;

    void setColor(Color color);
    Color getColor();
    void addSample(Color color);
    int getSample();

private:

    Color colorAcc;
    Color colorLast;
    unsigned int samples;
};

// TODO change name to renderBuffer
class PixelBuffer
{
public:
    unsigned int width, height;

    // Stores the accumulated pixels values
    std::vector<Pixel> pixels;

    // Filled used fillQueue, and emptied during the rendering
    std::vector<Pixel*> pixelsQueue;
    std::atomic<unsigned int> index{0};
    std::mutex pixelsQueueMutex;

    bool done;  

    // Rendering options
    int samplesLimit;
    float varianceThreshold;

    // Constructor
    PixelBuffer(unsigned int width_, unsigned int height_);

    // Get dimenstions
    unsigned int getWidth() const;
    unsigned int getHeight() const;

    // Get pixels (deprecated)
    const std::vector<Color> &getPixels() const;

    // Set pixel (deprecated)
    void setPixel(unsigned int x, unsigned int y, Color color);

    void fillQueue();

    // Clear buffer
    void clearBuffer();

    // Check if the render has finished
    bool checkDone();
  
};

#endif // PixelBuffer_H