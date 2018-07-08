#include "Scene.hpp"

namespace rendering
{
    int Scene::addObject(Object& object)
    {
        int index = static_cast<int>(m_geometry.size());
        m_geometry.emplace_back(object);
        return index;
    }
}
