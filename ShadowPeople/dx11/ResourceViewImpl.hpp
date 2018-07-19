/*
    Copyright 2018 Samuel Siltanen
    ResourceViewImpl.hpp
*/

#pragma once

#include <d3d11.h>

namespace graphics
{
	class ResourceViewImpl
	{
	public:
		virtual const ID3D11View* view() const = 0;
	};
}
