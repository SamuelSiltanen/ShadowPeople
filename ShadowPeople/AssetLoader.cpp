#include "AssetLoader.hpp"
#include "Mesh.hpp"

// File operations requires OS specific stuff, move it later to a separate file
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <memory>

namespace asset
{
	bool AssetLoader::load(const std::string& filename, rendering::Mesh& mesh)
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
			return false;
		}

		LARGE_INTEGER fileSize;
		if (!GetFileSizeEx(file, &fileSize))
		{
			OutputDebugString("Unable to read file size\n");
			CloseHandle(file);
			return false;
		}

		if (fileSize.QuadPart > (1 << 30))
		{
			OutputDebugString("File too big to read\n");
			CloseHandle(file);
			return false;
		}

		DWORD bytesToRead = fileSize.LowPart;

		char* buffer = new char[bytesToRead];
		
		DWORD bytesActualRead;
		if (!ReadFile(file, buffer, bytesToRead, &bytesActualRead, NULL))
		{
			OutputDebugString("Could not read file contents\n");
			CloseHandle(file);
			delete[] buffer;
			return false;
		}

		CloseHandle(file);

		if (!parseObj(buffer, bytesActualRead, mesh))
		{			
			delete[] buffer;
			return false;
		}

		delete[] buffer;

		return true;
	}

	bool AssetLoader::parseObj(char* buffer, uint32_t bytes, rendering::Mesh& mesh)
	{
		std::vector<float3>	vtxPositions;
		std::vector<float2>	vtxTexCoords;
		std::vector<float3>	vtxNormals;
		std::vector<Face>	faces;
		std::string			materialName;

		char line[256];

		uint32_t index = 0;
		while (index < bytes)
		{
			// Copy line
			int i = 0;
			do
			{
				line[i] = buffer[index + i];
				i++;
			} while ((buffer[index + i - 1] != '\n') && (index + i < bytes));
			line[i] = '\0';
			index += i;

			OutputDebugString(line);

			// Parse line
			char *next_token = NULL;
			char *token = strtok_s(line, " \t\n\r", &next_token);
			if (token == NULL) continue;		// Empty line
			if (token[0] == '#') continue;		// Skip comments
			if (strcmp(token, "v") == 0)				// Vertex position
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
				vtxPositions.emplace_back(position);
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
					if (token[j] != '\0')
					{
						j++;
						while ((token[j] != '/') && (token[j] != '\0'))
						{
							indices[1] *= 10;
							indices[1] += token[j] - '0';
							j++;
						}
						if (token[j] != '\0')
						{
							j++;
							while(token[j] != '\0')
							{
								indices[2] *= 10;
								indices[2] += token[j] - '0';
								j++;
							}
						}
					}
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
			else if (strcmp(token, "usemt") == 0)	// Use material
			{

			}
			else
			{
				// Unknown token, skip it
			}
		}

		std::string msg("Read ");
		msg.append(std::to_string(vtxPositions.size())).append(" vertex positions, ");
		msg.append(std::to_string(vtxTexCoords.size())).append(" vertex texture coordinates, ");
		msg.append(std::to_string(vtxNormals.size())).append(" vertex normals, ");
		msg.append(std::to_string(faces.size())).append(" polygons\n");
		msg.append(std::string("Using material ").append(materialName).append("\n"));
		OutputDebugString(msg.c_str());

		return constructMesh(vtxPositions, vtxTexCoords, vtxNormals, mesh);
	}

	bool AssetLoader::constructMesh(Range<float3> positions, Range<float2> texcoords, Range<float3> normals,
									rendering::Mesh& mesh)
	{


		return true;
	}
}
