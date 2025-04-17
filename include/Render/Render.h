#ifndef RENDER_H
#define RENDER_H

#include "Utils/Color.h"
#include "Core/Ray.h"
#include "Core/SceneObject.h"
#include "Render/Camera.h"

#include <memory>
#include <thread>
#include <future>

class renderRectangle3
{
    public:
        int startX, endX;
        int startY, endY;

        bool done;
        bool rendering;

        // Constructor
        renderRectangle3(int sx, int ex, int sy, int ey)
            : startX(sx), endX(ex), startY(sy), endY(ey), done(false), rendering(false){}

};

enum class RenderQueueType {
    Grid,
    Spiral
};

class Render3
{
    public:

        int resX, resY;
        float antialiasing;

        std::vector<std::future<void>> renderingThreads;
        std::vector<renderRectangle3> renderingQueue;

        // Scene
        Camera3 camera;
        const std::vector<std::shared_ptr<SceneObject>> &objects;

        // Render settings
        int averages;
        int maxReflections;

        // Constructor
        Render3(
            Camera3 camera_,
            const std::vector<std::shared_ptr<SceneObject>> &objects_,
            int averages_ = 5,
            int maxReflections_ = 3,
            int resX_ = 400,
            int resY_ = 400,
            float antialiasing_ = 0.01f
        );

        void createRenderQueue(int divs, RenderQueueType type = RenderQueueType::Grid);


        // Takes a ray and a scene, and outputs a color
        Color renderRay(
            Ray3 ray);

        // Takes the camera settings and generates rays
        Ray3 createRay(
            int x, int y
        );

        void render(
            PixelBuffer &pixelBuffer,
            renderRectangle3 &rectangle
        );

        bool renderLoop(
            PixelBuffer &pixelBuffer,
            int numThreads
        );

};

#endif // RENDER_H