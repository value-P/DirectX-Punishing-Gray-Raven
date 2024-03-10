#pragma once
#include "Property.h"

namespace Engine
{
	class ENGINE_DLL CShader final : public CProperty
	{
	public:
		CShader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CShader() = default;

	public:
		virtual HRESULT Initialize(const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);

	public:
		HRESULT Begin(_uint iPassIndex, _uint iTechniqueIndex);
		HRESULT Bind_SRV(const _char* pConstantName, ComPtr<ID3D11ShaderResourceView> pSRV);
		HRESULT Bind_SRVs(const _char* pConstantName, const vector<ComPtr<ID3D11ShaderResourceView>>& vecSRVs);
		HRESULT Bind_Matrix(const _char* pConstantName, const Matrix* pMatrix);
		HRESULT Bind_Matrices(const _char* pConstantName, const Matrix* pMatrices, _uint iNumMatrices);
		HRESULT Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength);

	private:
		ComPtr<ID3DX11Effect>				m_pEffect = nullptr;
		vector<ComPtr<ID3D11InputLayout>>	m_InputLayouts;

	public:
		static shared_ptr<CShader> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strShaderFilePath,	const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	};
}
