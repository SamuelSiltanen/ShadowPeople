#pragma once

#include <vector>

#include "../Types.hpp"

namespace rendering
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

    struct Material
    {
    };

    struct Light
    {
    };

    class Scene
    {
    public:
        Scene(const Camera& camera) : m_camera(camera) {}

        const Camera& camera() const { return m_camera; }
    private:
        std::vector<Object>     m_geometry;
        std::vector<Material>   m_materials;
        std::vector<Light>      m_lights;
        const Camera&           m_camera;
    };
}
