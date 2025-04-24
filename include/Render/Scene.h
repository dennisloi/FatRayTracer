#ifndef SCENE_H
#define SCENE_H

#include "Render/Camera.h"

class scene3
{
public:
    Camera3 camera;
    std::vector<std::shared_ptr<SceneObject>> objects;
}

#endif // SCENE_H