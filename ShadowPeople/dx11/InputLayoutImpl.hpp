#pragma once

#include <d3d11.h>

#include "DX11Utils.hpp"

namespace graphics
{
	class DeviceImpl;

	class InputLayoutImpl
	{
	public:
		InputLayoutImpl(graphics::DeviceImpl& device, const desc::InputLayout& desc);

		NO_COPY_CLASS(InputLayoutImpl);
	private:
		ID3D11InputLayout* inputLayout;
	};
}
