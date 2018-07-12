#include "Scene.hpp"

namespace rendering
{
    int Scene::addObject(Object& object)
    {
        int index = static_cast<int>(m_geometry.size());
        m_geometry.emplace_back(object);
        return index;
    }

    int Scene::addMaterial(Material& material)
    {
        int index = static_cast<int>(m_materials.size());
        m_materials.emplace_back(material);
        return index;
    }
}
