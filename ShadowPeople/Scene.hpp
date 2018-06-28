#pragma once

#include <vector>

#include "Types.hpp"

namespace asset
{
    struct Transform
    {
        float3      position;
        float       scale;
        Quaternion  rotation;
    };

    struct Object
    {
        int         meshIndex;
        Transform   transform;
    };

    struct Scene
    {
        std::vector<Object> objects;
    };
}
