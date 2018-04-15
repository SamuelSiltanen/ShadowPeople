#pragma once

#include <d3d11.h>

#include "Descriptors.hpp"

namespace graphics
{
	void setUsageFlags(const desc::Usage& descUsage,
					   D3D11_USAGE& usage, UINT& cpuAccess, UINT& bind);

	DXGI_FORMAT dxgiFormat(const desc::Format& format);
}
