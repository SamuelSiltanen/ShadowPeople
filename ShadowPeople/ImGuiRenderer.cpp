#include "ImGuiRenderer.hpp"
#include "shaders/ImGuiRenderer.if.h"
#include "imgui/imgui.h"
#include "Types.hpp"

using namespace graphics;

namespace rendering
{
	ImGuiRenderer::ImGuiRenderer(Device& device) :
		m_device(device)
	{
		ImGuiContext* imGuiContext = ImGui::CreateContext();
		ImGui::SetCurrentContext(imGuiContext);

		int2 screenSize = device.swapChainSize();

		m_imGuiRenderingPipeline = device.createGraphicsPipeline(desc::GraphicsPipeline()
			.binding<shaders::ImGuiRendererGS>()
			.numRenderTargets(1)
			.inputLayoutElement(desc::InputElement("POSITION", 0,
				{ desc::FormatChannels::RG, desc::FormatBytesPerChannel::B32, desc::FormatType::Float }, 0))
			.inputLayoutElement(desc::InputElement("TEXCOORD", 0,
				{ desc::FormatChannels::RG, desc::FormatBytesPerChannel::B32, desc::FormatType::Float }, 0))
			.inputLayoutElement(desc::InputElement("COLOR", 0,
				{ desc::FormatChannels::RGBA, desc::FormatBytesPerChannel::B8, desc::FormatType::UNorm }, 0))
			.setPrimitiveTopology(desc::PrimitiveTopology::TriangleList)
			.blendState(desc::BlendState().enabled(true)
				.source(desc::BlendMode::SrcAlpha)
				.destination(desc::BlendMode::InvSrcAlpha)
				.blendOp(desc::BlendOp::Add)
				.sourceAlpha(desc::BlendMode::InvSrcAlpha)
				.destinationAlpha(desc::BlendMode::Zero)
				.blendOpAlpha(desc::BlendOp::Add))
			.rasterizerState(desc::RasterizerState()
				.cullMode(desc::CullMode::None)
				.enableScissors(true))
			.depthStencilState(desc::DepthStencilState().depthTestingEnable(false)));

		// TODO: Create input assembler layout and set in rendering

		createFontsTexture();
		createVertexBuffer(0);
		createIndexBuffer(0);

		// TODO: Set vertex and index buffers in rendering
	}

	ImGuiRenderer::~ImGuiRenderer()
	{
		ImGui::DestroyContext();
	}

	void ImGuiRenderer::createFontsTexture()
	{
		ImGuiIO& io = ImGui::GetIO();

		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

		const uint32_t BytesPerPixel = 4;
		m_fontTexture		= m_device.createTexture(desc::Texture()
								.width(width).height(height)
								.initialData(desc::InitialData(pixels, width * BytesPerPixel)));
		m_fontTextureSRV	= m_device.createTextureView(m_fontTexture.descriptor(), m_fontTexture);

		io.Fonts->TexID		= (void *)&m_fontTextureSRV;

		m_fontSampler		= m_device.createSampler(desc::Sampler());
	}

	void ImGuiRenderer::createVertexBuffer(uint32_t minVertices)
	{
		const uint32_t VertexBufferResizeIncrement = 5000;

		m_vertexBuffer = m_device.createBuffer(desc::Buffer()
			.elements(minVertices + VertexBufferResizeIncrement)
			.format<ImDrawVert>()
			.type(desc::BufferType::Vertex)
			.usage(desc::Usage::CpuToGpuFrequent));
	}

	void ImGuiRenderer::createIndexBuffer(uint32_t minVertices)
	{
		const uint32_t IndexBufferResizeIncrement = 10000;

		m_indexBuffer = m_device.createBuffer(desc::Buffer()
			.elements(minVertices + IndexBufferResizeIncrement)
			.format<ImDrawIdx>()
			.type(desc::BufferType::Index)
			.usage(desc::Usage::CpuToGpuFrequent));
	}

	void ImGuiRenderer::render(graphics::CommandBuffer& gfx, ImDrawData* drawData)
	{
		if (m_vertexBuffer.descriptor().elements < drawData->TotalVtxCount)
		{
			createVertexBuffer(drawData->TotalVtxCount);
		}
		if (m_indexBuffer.descriptor().elements < drawData->TotalIdxCount)
		{
			createIndexBuffer(drawData->TotalIdxCount);
		}

		copyVertexAndIndexBufferData(gfx, drawData);

		gfx.setIndexBuffer(m_indexBuffer);
		gfx.setVertexBuffer(m_vertexBuffer, m_imGuiRenderingPipeline);

		Matrix4x4 projectionMatrix = constructProjectionMatrix(drawData);		

		int vtxOffset = 0;
		int idxOffset = 0;
		float2 pos{ drawData->DisplayPos.x, drawData->DisplayPos.y };
		for (int n = 0; n < drawData->CmdListsCount; n++)
		{
			const ImDrawList* cmdList = drawData->CmdLists[n];
			for (int i = 0; i < cmdList->CmdBuffer.Size; i++)
			{
				const ImDrawCmd* cmd = &cmdList->CmdBuffer[i];
				if (cmd->UserCallback)
				{
					cmd->UserCallback(cmdList, cmd);
				}
				else
				{
					setScissorRect(cmd, pos);
					renderCommand(gfx, cmd, projectionMatrix, idxOffset, vtxOffset);
				}

				idxOffset += cmd->ElemCount;
			}
			vtxOffset += cmdList->VtxBuffer.Size;
		}

		gfx.setIndexBuffer();
		gfx.setVertexBuffer();
	}

	void ImGuiRenderer::copyVertexAndIndexBufferData(graphics::CommandBuffer& gfx, ImDrawData* drawData)
	{
		Mapping vertexMapping	= gfx.map(m_vertexBuffer);
		Mapping indexMapping	= gfx.map(m_indexBuffer);
		ImDrawVert* vertexData	= reinterpret_cast<ImDrawVert *>(vertexMapping.data());
		ImDrawIdx* indexData	= reinterpret_cast<ImDrawIdx *>(indexMapping.data());

		for (int n = 0; n < drawData->CmdListsCount; n++)
		{
			const ImDrawList* cmdList = drawData->CmdLists[n];

			memcpy(vertexData, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
			memcpy(indexData, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));

			vertexData += cmdList->VtxBuffer.Size;
			indexData += cmdList->IdxBuffer.Size;
		}
	}

	Matrix4x4 ImGuiRenderer::constructProjectionMatrix(ImDrawData* drawData)
	{
		Matrix4x4 projectionMatrix;

		float L = drawData->DisplayPos.x;
		float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
		float T = drawData->DisplayPos.y;
		float B = drawData->DisplayPos.y + drawData->DisplaySize.y;

		projectionMatrix(0, 0) = 2.f / (R - L);
		projectionMatrix(1, 1) = 2.f / (T - B);
		projectionMatrix(2, 2) = 0.5f;
		projectionMatrix(2, 3) = 0.5f;
		projectionMatrix(0, 3) = (R + L) / (L - R);
		projectionMatrix(1, 3) = (T + B) / (B - T);

		return projectionMatrix;
	}

	void ImGuiRenderer::setScissorRect(const ImDrawCmd* cmd, float2 pos)
	{
		int2 leftTop{ static_cast<int>(cmd->ClipRect.x - pos[0]),
					  static_cast<int>(cmd->ClipRect.y - pos[1]) };
		int2 rectSize{ static_cast<int>(cmd->ClipRect.z - cmd->ClipRect.x),
					   static_cast<int>(cmd->ClipRect.w - cmd->ClipRect.y) };
		Rect<int, 2> rect(leftTop, rectSize);
		m_imGuiRenderingPipeline.setScissorRect(rect);
	}

	void ImGuiRenderer::renderCommand(graphics::CommandBuffer& gfx, const ImDrawCmd* cmd,
									  Matrix4x4& projectionMatrix, int idxOffset, int vtxOffset)
	{
		auto binding = m_imGuiRenderingPipeline.bind<shaders::ImGuiRendererGS>(gfx);

		binding->constants.proj	= projectionMatrix;
		binding->fonts			= m_fontTextureSRV;
		binding->fontSampler	= m_fontSampler;

		gfx.drawIndexed(*binding, cmd->ElemCount, idxOffset, vtxOffset);
	}
}
