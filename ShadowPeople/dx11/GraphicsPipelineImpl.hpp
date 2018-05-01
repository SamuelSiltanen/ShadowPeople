#pragma once

#include <d3d11.h>

#include "DX11Utils.hpp"

namespace graphics
{
	class DeviceImpl;

	class GraphicsPipelineImpl
	{
	public:
		GraphicsPipelineImpl(DeviceImpl& device, const desc::GraphicsPipeline& desc);
		~GraphicsPipelineImpl();

		NO_COPY_CLASS(GraphicsPipelineImpl)

		const desc::GraphicsPipeline& descriptor() const { return m_descriptor; }
	private:
		ID3D11VertexShader*		m_vertexShader;
		ID3D11PixelShader*		m_pixelShader;
		
		desc::GraphicsPipeline	m_descriptor;
	};
}
