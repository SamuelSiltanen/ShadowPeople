/*
    Copyright 2018 Samuel Siltanen
    AssetLoader.cpp
*/

#include "AssetLoader.hpp"

#include "../rendering/Mesh.hpp"
#include "../rendering/Scene.hpp"

#include "../rendering/GeometryCache.hpp"
#include "../rendering/MaterialCache.hpp"

#include "../Math.hpp"

// File operations requires OS specific stuff, move it later to a separate file
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <unordered_map>

#define VERBOSE_MODE

namespace asset
{
    AssetLoader::AssetLoader(rendering::GeometryCache& geometry, rendering::MaterialCache& materials) :
        m_geometry(geometry),
        m_materials(materials)
    {}

    AssetLoader::DataBlob AssetLoader::fileToBlob(const std::string& filename)
    {
        HANDLE file = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
                                 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file == INVALID_HANDLE_VALUE)
        {
            if (GetLastError() == ERROR_FILE_NOT_FOUND)
            {
                std::string err("Could not find ");
                err.append(filename).append("\n");
                OutputDebugString(err.c_str());
            }
            return nullptr;
        }

        LARGE_INTEGER fileSize;
        if (!GetFileSizeEx(file, &fileSize))
        {
            OutputDebugString("Unable to read file size\n");
            CloseHandle(file);
            return nullptr;
        }

        if (fileSize.QuadPart > (1 << 30))
        {
            OutputDebugString("File too big to read\n");
            CloseHandle(file);
            return nullptr;
        }

        DWORD bytesToRead = fileSize.LowPart;

        std::shared_ptr<std::vector<char>> buffer = std::make_shared<std::vector<char>>(bytesToRead);

        DWORD bytesActualRead;
        if (!ReadFile(file, buffer->data(), bytesToRead, &bytesActualRead, NULL))
        {
            OutputDebugString("Could not read file contents\n");
            CloseHandle(file);
            return nullptr;
        }

        CloseHandle(file);

        buffer->resize(bytesActualRead);

        return buffer;
    }

    bool AssetLoader::loadModel(const std::string& filename, rendering::Mesh& mesh)
    {
        DataBlob buffer = fileToBlob(filename);
        if (buffer == nullptr) return false;

        if (!parseObj(buffer, mesh)) return false;

        return true;
    }

    bool AssetLoader::loadScene(const std::string& filename, rendering::Scene& scene)
    {
        DataBlob buffer = fileToBlob(filename);
        if (buffer == nullptr) return false;

        char line[256];

        uint32_t index = 0;
        while(index < buffer->size())
        {
            int bytesRead = getLine(line, Range<const char>(buffer->data(), buffer->size()), index);
            index += bytesRead;

            // Parse line
			char *next_token = NULL;
			char *token = strtok_s(line, " \t\n\r", &next_token);
			if (token == NULL) continue;		// Empty line

            std::string modelFileName(token);
            rendering::Mesh mesh;
            if (!loadModel(modelFileName, mesh))
            {
                std::string err("File not found: ");
                err.append(modelFileName).append("\n");
                OutputDebugString(err.c_str());
                return false;
            }

            // Pre-load the mesh now - later, implement proper streaming and only give a virtual offset here
            int2 meshStartSize = m_geometry.preloadMesh(mesh);
            rendering::Transform transform; // TODO: Fill
            int objectOffset = scene.addObject(rendering::Object(meshStartSize, transform));
        }

        
        rendering::Image image;
        if (!loadImage("data/models/house/house_diffuse.tga", image)) return false;

        int2 size{ image.width(), image.height() };
        Rect<int, 2> materialRect = m_materials.allocate(size);

        m_materials.preloadMaterial(image, materialRect, rendering::MaterialChannel::Albedo);

        if (!loadImage("data/models/house/house_spec.tga", image)) return false;
        m_materials.preloadMaterial(image, materialRect, rendering::MaterialChannel::Roughness);

        if (!loadImage("data/models/house/house_normal.tga", image)) return false;
        m_materials.preloadMaterial(image, materialRect, rendering::MaterialChannel::Normal);

        int materialOffset = scene.addMaterial(rendering::Material(materialRect));

        return true;
    }

    bool AssetLoader::loadImage(const std::string& filename, rendering::Image& image)
    {
        DataBlob buffer = fileToBlob(filename);
        if (buffer == nullptr) return false;

        if (!parseTga(buffer, image)) return false;

        return true;
    }

    bool AssetLoader::loadMaterial(const std::string& filename, rendering::Material& material)
    {
        // TODO
        return true;
    }

	bool AssetLoader::parseObj(DataBlob buffer, rendering::Mesh& mesh)
	{
		std::vector<float3>	vtxPositions;
		std::vector<float2>	vtxTexCoords;
		std::vector<float3>	vtxNormals;
		std::vector<Face>	faces;
		std::string			materialName;

		char line[256];

		uint32_t index = 0;
		while (index < buffer->size())
		{
            int bytesRead = getLine(line, Range<const char>(buffer->data(), buffer->size()), index);
            index += bytesRead;

			// Parse line
			char *next_token = NULL;
			char *token = strtok_s(line, " \t\n\r", &next_token);
			if (token == NULL) continue;		// Empty line
			if (token[0] == '#') continue;		// Skip comments
			if (strcmp(token, "v") == 0)		// Vertex position
			{
				token = strtok_s(NULL, " \t\n\r", &next_token);
				float x = static_cast<float>(atof(token));
				token = strtok_s(NULL, " \t\n\r", &next_token);
				float y = static_cast<float>(atof(token));
				token = strtok_s(NULL, " \t\n\r", &next_token);
				float z = static_cast<float>(atof(token));
				token = strtok_s(NULL, " \t\n\r", &next_token);
				if (token)
				{
					// If there is w coordinate, normalize the position, so that w == 1
					float w = static_cast<float>(atof(token));
					x /= w;
					y /= w;
					z /= w;
				}
				float3 position{ x, y, z };
				vtxPositions.emplace_back(position * 0.01f); // Obj units are centimeters, but ours are meters
			}
			else if (strcmp(token, "vt") == 0)		// Vertex texture coordinates
			{
				token = strtok_s(NULL, " \t\n\r", &next_token);
				float u = static_cast<float>(atof(token));
				token = strtok_s(NULL, " \t\n\r", &next_token);
				float v = static_cast<float>(atof(token));
				float2 texcoords{ u, v };
				vtxTexCoords.emplace_back(texcoords);
			}
			else if (strcmp(token, "vn") == 0)		// Vertex normals
			{
				token = strtok_s(NULL, " \t\n\r", &next_token);
				float x = static_cast<float>(atof(token));
				token = strtok_s(NULL, " \t\n\r", &next_token);
				float y = static_cast<float>(atof(token));
				token = strtok_s(NULL, " \t\n\r", &next_token);
				float z = static_cast<float>(atof(token));
				float3 normals{ x, y, z };
				vtxNormals.emplace_back(normals);
			}
			else if (strcmp(token, "f") == 0)		// Face - not necessarily a triangle
			{
				Face f;
				token = strtok_s(NULL, " \t\n\r", &next_token);
				while (token)
				{
					uint3 indices{ 0, 0, 0 };
					int j = 0;
					while ((token[j] != '/') && (token[j] != '\0'))
					{
						indices[0] *= 10;
						indices[0] += token[j] - '0';
						j++;
					}
                    indices[0]--;   // Obj-indices are 1-based, while ours are 0-based
					if (token[j] != '\0')
					{
						j++;
						while ((token[j] != '/') && (token[j] != '\0'))
						{
							indices[1] *= 10;
							indices[1] += token[j] - '0';
							j++;
						}
                        indices[1]--;   // Obj-indices are 1-based, while ours are 0-based
						if (token[j] != '\0')
						{
							j++;
							while(token[j] != '\0')
							{
								indices[2] *= 10;
								indices[2] += token[j] - '0';
								j++;
							}
                            indices[2]--;   // Obj-indices are 1-based, while ours are 0-based
						}
					}
					f.indices.emplace_back(indices);
					token = strtok_s(NULL, " \t\n\r", &next_token);
				}
				faces.emplace_back(f);
			}
			else if (strcmp(token, "l") == 0)		// Line
			{

			}
			else if (strcmp(token, "s") == 0)		// Smoothing group
			{

			}
			else if (strcmp(token, "o") == 0)		// Object name
			{

			}
			else if (strcmp(token, "g") == 0)		// Group name
			{

			}
			else if (strcmp(token, "mtllib") == 0)	// Material library
			{
				token = strtok_s(NULL, " \t\n\r", &next_token);
				materialName = token;
			}
			else if (strcmp(token, "usemtl") == 0)	// Use material
			{

			}
			else
			{
				// Unknown token, skip it
			}
		}

#ifdef VERBOSE_MODE
		std::string msg("Read ");
		msg.append(std::to_string(vtxPositions.size())).append(" vertex positions, ");
		msg.append(std::to_string(vtxTexCoords.size())).append(" vertex texture coordinates, ");
		msg.append(std::to_string(vtxNormals.size())).append(" vertex normals, ");
		msg.append(std::to_string(faces.size())).append(" polygons\n");
		msg.append(std::string("Using material ").append(materialName).append("\n"));
		OutputDebugString(msg.c_str());
#endif

		return constructMesh(vtxPositions, vtxTexCoords, vtxNormals, faces, mesh);
	}

	bool AssetLoader::constructMesh(Range<float3> positions, Range<float2> texcoords, Range<float3> normals,
									Range<Face> faces, rendering::Mesh& mesh)
	{
		std::unordered_map<uint3, uint32_t> uniqueIndices;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<uint32_t> faceIndices;
		for (auto &face : faces)
		{
			faceIndices.clear();
			for (auto indexTrio : face.indices)
			{
				uint32_t index;
				auto it = uniqueIndices.find(indexTrio);
				if (it == uniqueIndices.end())
				{
					Vertex vtx;
					vtx.position	= positions[indexTrio[0]];
					vtx.uv			= texcoords[indexTrio[1]];
					vtx.normal		= normals[indexTrio[2]];

					index = static_cast<uint32_t>(vertices.size());
					uniqueIndices[indexTrio] = index;

					vertices.emplace_back(vtx);
				}
				else
				{
					index = it->second;
				}
				faceIndices.emplace_back(index);
			}

            // Note: Obj-files store vertices in counter-clockwise order by default, and
            //       may have arbitrary polygons.
			if (faceIndices.size() == 3)
			{
                indices.emplace_back(faceIndices[0]);
				indices.emplace_back(faceIndices[2]);
				indices.emplace_back(faceIndices[1]);
			}
			else if (faceIndices.size() == 4)
			{
				indices.emplace_back(faceIndices[0]);
				indices.emplace_back(faceIndices[2]);
				indices.emplace_back(faceIndices[1]);

				indices.emplace_back(faceIndices[0]);
				indices.emplace_back(faceIndices[3]);
				indices.emplace_back(faceIndices[2]);
			}
			else
			{
				std::string err("Complex polygons not yet supported: ");
				err.append(std::to_string(faceIndices.size())).append(" vertices\n");
				OutputDebugString(err.c_str());
				return false;
			}
		}

		mesh.fill(vertices, indices);

#ifdef VERBOSE_MODE
		std::string msg("Loaded model with ");
		msg.append(std::to_string(vertices.size())).append(" unique vertices and ");
		msg.append(std::to_string(indices.size())).append(" triangles\n");
		OutputDebugString(msg.c_str());
#endif

		return true;
	}

    int AssetLoader::getLine(char *lineBuffer, Range<const char> sourceBuffer, int pos)
    {
        int i = 0;
		do
		{
			lineBuffer[i] = sourceBuffer[pos + i];
			i++;
		} while ((sourceBuffer[pos + i - 1] != '\n') && (pos + i < sourceBuffer.byteSize()));

		lineBuffer[i] = '\0';

		return i;
    }

    bool AssetLoader::parseTga(DataBlob buffer, rendering::Image& image)
    {
        TgaHeader header;
        memcpy(&header, buffer->data(), sizeof(TgaHeader));

        bool imageIDExists   = (header.idLength != 0);
        bool colorMapExists  = (header.colorMapType != 0);
        bool imageDataExists = (header.imageType != 0);

        TgaFooter footer;
        memcpy(&footer, buffer->data() + buffer->size() - sizeof(TgaFooter), sizeof(TgaFooter));

        bool footerValid = (strncmp(footer.signature, "TRUEVISION-XFILE", 16) == 0);        

        uint32_t index = sizeof(TgaHeader);

        // Image ID
        if (imageIDExists)
        {
            index += header.idLength;
        }

        // Color map
        if (colorMapExists)
        {
            uint8_t bytesPerEntry   = math::divRoundUp<uint8_t>(header.colorMapEntrySize, 8);
            uint32_t colorMapBytes  = header.colorMapLength * bytesPerEntry;
            index += colorMapBytes;
        }

        // Image data
        if (imageDataExists)
        {
            uint8_t bytesPerPixel   = math::divRoundUp<uint8_t>(header.bitsPerPixel, 8);
            size_t imageDataSize  = header.widthInPixels * header.heightInPixels * bytesPerPixel;

#ifdef VERBOSE_MODE
            std::string msg("Image data: ");
            msg.append(std::to_string(header.widthInPixels)).append(" x ");
            msg.append(std::to_string(header.heightInPixels)).append(" @ ");
            msg.append(std::to_string(header.bitsPerPixel)).append(" bpp\n");
            OutputDebugString(msg.c_str());
#endif
            image.setDimensions(header.widthInPixels, header.heightInPixels, header.bitsPerPixel);
            auto bytes = vectorAsByteRange(*buffer);
            image.fillData(Range<const uint8_t>(bytes.begin() + index, imageDataSize));
        }

        if (footerValid)
        {
#ifdef VERBOSE_MODE
            OutputDebugString("Valid TGA 2.0 file\n");
#endif
            // Developer area

            // Extension area
            TgaExtension extension;
            memcpy(&extension, buffer->data() + footer.extensionOffset, sizeof(TgaExtension));
        }

        return true;
    }
}
