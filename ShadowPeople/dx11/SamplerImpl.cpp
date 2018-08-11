#include "SamplerImpl.hpp"
#include "DeviceImpl.hpp"
#include "../Errors.hpp"

namespace graphics
{
	SamplerImpl::SamplerImpl(DeviceImpl& device, const desc::Sampler& desc) :
		m_descriptor(desc)
	{
		D3D11_SAMPLER_DESC dxdesc;

		switch (desc.descriptor().type)
		{
		case desc::SamplerType::Point:
			dxdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			break;
		case desc::SamplerType::Bilinear:
			dxdesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			break;
		case desc::SamplerType::Trilinear:
			dxdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			break;
		case desc::SamplerType::Anisotropic:
			dxdesc.Filter = D3D11_FILTER_ANISOTROPIC;
			break;
		case desc::SamplerType::PointComparison:
			dxdesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
			break;
		case desc::SamplerType::BilinearComparison:
			dxdesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
			break;
		case desc::SamplerType::TrilinearComparison:
			dxdesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
			break;
		case desc::SamplerType::AnisotropicComparison:
			dxdesc.Filter = D3D11_FILTER_COMPARISON_ANISOTROPIC;
			break;
		default:
			SP_ASSERT(false, "Unsupported filter type");
			break;
		}

		dxdesc.AddressU = textureAddressMode(desc.descriptor().uMode);
		dxdesc.AddressV = textureAddressMode(desc.descriptor().vMode);
		dxdesc.AddressW = textureAddressMode(desc.descriptor().wMode);

		dxdesc.MipLODBias	= desc.descriptor().mipBias;
		dxdesc.MinLOD		= desc.descriptor().mipMin;
		dxdesc.MaxLOD		= desc.descriptor().mipMax;

		SP_ASSERT((desc.descriptor().anisotropy >= 1) && (desc.descriptor().anisotropy <= 16),
				  "Sampler anisotropy must be in range 1-16");
		dxdesc.MaxAnisotropy = desc.descriptor().anisotropy;

		dxdesc.ComparisonFunc = comparisonFunc(desc.descriptor().compFunc);

		dxdesc.BorderColor[0] = desc.descriptor().borderColor[0];
		dxdesc.BorderColor[1] = desc.descriptor().borderColor[1];
		dxdesc.BorderColor[2] = desc.descriptor().borderColor[2];
		dxdesc.BorderColor[3] = desc.descriptor().borderColor[3];

		HRESULT hr = device.device()->CreateSamplerState(&dxdesc, &m_sampler);
		if (hr != S_OK)
		{
			MessageBox(NULL, _T("CreateSamplerState() failed!"), _T("Error"), NULL);
			m_sampler = nullptr;
		}

        auto name = desc.descriptor().name;
        m_sampler->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());
	}

    SamplerImpl::~SamplerImpl()
    {
        SAFE_RELEASE(m_sampler);
    }
}
