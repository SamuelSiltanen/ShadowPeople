/*
    Copyright 2018 Samuel Siltanen
    Scene.hpp
*/

#pragma once

#include <vector>

#include "../Types.hpp"

namespace rendering
{
    class Camera;

    struct Transform
    {
        float3      position;
        float       scale;
        Quaternion  rotation;
    };

    struct Object
    {
        int2        meshStartSize;
        Transform   transform;

        Object(int2 meshStartSize, Transform transform) :
            meshStartSize(meshStartSize),
            transform(transform)
        {}
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

        int addObject(Object& object);

        const std::vector<Object> objects() const { return m_geometry; }
    private:
        std::vector<Object>     m_geometry;
        std::vector<Material>   m_materials;
        std::vector<Light>      m_lights;
        const Camera&           m_camera;
    };
}
