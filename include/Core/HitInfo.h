#ifndef HITINFO_H
#define HITINFO_H

#include "Core/Ray.h"

struct hitInfo {
    Vector3 hitPoint;
    // Ray3 reflection;
    int hitObjectIndex;
    float distance;
};

#endif