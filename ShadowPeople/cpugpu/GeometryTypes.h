/*
    Copyright 2018 Samuel Siltanen
    GeometryTypes.h
*/

#ifndef SP_GEOMETRY_TYPES_H
#define SP_GEOMETRY_TYPES_H

#ifdef __cplusplus
#include "../Types.hpp"
#endif

    struct Vertex
	{
        float4  orientation;    // Note: This is the geometric orientation quaternion
        float   bitangentSign;
		float3	position;
        float3  normal;         // Note: This might be non-geometric, i.e. bent, for artistic purposes
		float2	uv;
    };

#endif
