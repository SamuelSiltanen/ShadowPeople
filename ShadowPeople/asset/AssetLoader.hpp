/*
    Copyright 2018 Samuel Siltanen
    AssetLoader.hpp
*/

#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../Types.hpp"

namespace rendering
{
    class Mesh;
    class Scene;
    class GeometryCache;
}

namespace asset
{
    class AssetLoader
    {
    public:
        AssetLoader(rendering::GeometryCache& geometry);

        bool loadModel(const std::string& filename, rendering::Mesh& mesh);
        bool loadScene(const std::string& filename, rendering::Scene& scene);
    private:
        struct Face 
        {
            std::vector<uint3> indices;
        };

        using DataBlob = std::shared_ptr<std::vector<char>>;

        bool parseObj(DataBlob buffer, rendering::Mesh& mesh);
        bool constructMesh(Range<float3> positions, Range<float2> texcoords, Range<float3> normals,
                           Range<Face> faces, rendering::Mesh& mesh);

        DataBlob fileToBlob(const std::string& filename);
        int getLine(char *lineBuffer, Range<const char> sourceBuffer, int pos);

        rendering::GeometryCache& m_geometry;
        //rendering::MaterialCache& m_materials;
    };
}
