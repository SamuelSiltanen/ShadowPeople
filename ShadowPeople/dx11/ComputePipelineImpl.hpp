#pragma once

#include <d3d11.h>

#include "DX11Utils.hpp"
#include "ShaderResourcesImpl.hpp"
#include "ShaderImpl.hpp"

namespace graphics
{
    class DeviceImpl;
    class BufferImpl;
    class ShaderManagerImpl;

    class ComputePipelineImpl
    {
    public:
        ComputePipelineImpl(DeviceImpl&                     device,
                            ShaderManagerImpl&              shaderManager,
                            const desc::ComputePipeline&    desc);

        NO_COPY_CLASS(ComputePipelineImpl);

        const desc::ComputePipeline& descriptor() const { return m_descriptor; }
        const ShaderImpl& shader() const                { return m_shader; }
        ShaderResourcesImpl& resources()                { return m_resources; }
    private:
        ShaderImpl              m_shader;
        ShaderResourcesImpl     m_resources;
        desc::ComputePipeline   m_descriptor;
    };
}
