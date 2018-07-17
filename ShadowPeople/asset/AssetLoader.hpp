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
    class Image;
    struct Material;
    class MaterialCache;
}

namespace asset
{
    class AssetLoader
    {
    public:
        AssetLoader(rendering::GeometryCache& geometry, rendering::MaterialCache& materials);

        bool loadModel(const std::string& filename, rendering::Mesh& mesh);
        bool loadScene(const std::string& filename, rendering::Scene& scene);
        bool loadImage(const std::string& filename, rendering::Image& image);
        bool loadMaterial(const std::string& filename, rendering::Material& material);
    private:
        struct Face 
        {
            std::vector<uint3> indices;
        };

        bool parseObj(DataBlob<> buffer, rendering::Mesh& mesh);
        bool constructMesh(Range<float3> positions, Range<float2> texcoords, Range<float3> normals,
                           Range<Face> faces, rendering::Mesh& mesh);

        DataBlob<> fileToBlob(const std::string& filename);
        int getLine(char *lineBuffer, Range<const char> sourceBuffer, int pos);

#pragma pack(push, 1)   // This is required, because the TGA fields are not aligned
        struct TgaHeader
        {
            uint8_t     idLength;
            uint8_t     colorMapType;
            uint8_t     imageType;

            uint16_t    firstColorMapIndex;
            uint16_t    colorMapLength;
            uint8_t     colorMapEntrySize;

            uint16_t    xOrigin;
            uint16_t    yOrigin;
            uint16_t    widthInPixels;
            uint16_t    heightInPixels;
            uint8_t     bitsPerPixel;
            uint8_t     imageDesc;
        };
        struct TgaFooter
        {
            uint32_t    extensionOffset;
            uint32_t    developerAreaOffset;
            char        signature[16];
            char        dot;
            char        null;
        };
        struct TgaExtension
        {
            uint16_t    extensionSize;
            char        authorName[41];
            char        authorComment[324];
            char        dataAndTime[12];
            char        jobID[41];
            char        jobTime[6];
            char        softwareID[41];
            char        softwareVersion[3];
            uint32_t    keyColor;
            uint32_t    pixelAspectRatio;
            uint32_t    gammaValue;
            uint32_t    colorCorrectionOffset;
            uint32_t    postageStampOffset;
            uint32_t    scanlineOffset;
            uint8_t     attributesType;
        };
#pragma pack(pop)
        bool parseTga(DataBlob<> buffer, rendering::Image& image);

        rendering::GeometryCache& m_geometry;
        rendering::MaterialCache& m_materials;
    };
}
