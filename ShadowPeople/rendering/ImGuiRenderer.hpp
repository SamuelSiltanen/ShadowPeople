#pragma once

#include "../Graphics.hpp"

struct ImDrawData;
struct ImDrawCmd;
struct ImGuiContext;

namespace rendering
{
	class ImGuiRenderer
	{
	public:
		ImGuiRenderer(graphics::Device& device);
		~ImGuiRenderer();

		void render(graphics::CommandBuffer& gfx, ImDrawData* drawData);

	private:
		void createFontsTexture();
		void createVertexBuffer(uint32_t minVertices);
		void createIndexBuffer(uint32_t minVertices);

		void copyVertexAndIndexBufferData(graphics::CommandBuffer& gfx, ImDrawData* drawData);
		Matrix4x4 constructProjectionMatrix(ImDrawData* drawData);
		void setScissorRect(const ImDrawCmd* cmd, float2 pos);
		void renderCommand(graphics::CommandBuffer& gfx, const ImDrawCmd* cmd,
						   Matrix4x4& projectionMatrix, int idxOffset, int vtxOffset);

		graphics::Device&			m_device;

		graphics::GraphicsPipeline	m_imGuiRenderingPipeline;
		graphics::Sampler			m_fontSampler;
		graphics::Texture			m_fontTexture;
		graphics::TextureView		m_fontTextureSRV;
		graphics::Buffer			m_indexBuffer;
		graphics::Buffer			m_vertexBuffer;
	};
}
