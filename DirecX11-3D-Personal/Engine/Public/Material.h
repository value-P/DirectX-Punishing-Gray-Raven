#pragma once
#include "Property.h"

namespace Engine
{
	class CTexture;

	class CMaterial final : public CProperty
    {
	public:
		CMaterial(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CMaterial() = default;

	public:
		virtual HRESULT Initialize(const wstring& filePath, _uint& outFilePointer);
		ComPtr<ID3D11ShaderResourceView> Get_MatrialSRV(MaterialTexType eMatrialType);

	private:
		shared_ptr<CTexture> m_Matrials[MATERIAL_TEXTURE_TYPE_MAX];

	public:
		static shared_ptr<CMaterial> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& filePath, _uint& outFilePointer);
	};
}


