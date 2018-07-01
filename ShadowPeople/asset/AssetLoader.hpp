#pragma once

#include <string>
#include <vector>

#include "../Types.hpp"

namespace rendering
{
	class Mesh;
}

namespace asset
{
	class AssetLoader
	{
	public:
		bool load(const std::string& filename, rendering::Mesh& mesh);
	private:
		struct Face 
		{
			std::vector<uint3> indices;
		};

		bool parseObj(char* buffer, uint32_t bytes, rendering::Mesh& mesh);
		bool constructMesh(Range<float3> positions, Range<float2> texcoords, Range<float3> normals,
						   Range<Face> faces, rendering::Mesh& mesh);
	};
}
