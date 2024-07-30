#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CShader;
	class CRectProp;

	class CRenderTarget final
	{
	public:
		CRenderTarget(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual  ~CRenderTarget() = default;

	public:
		ComPtr<ID3D11RenderTargetView> Get_RTV() const { return m_pRTV; }

	public:
		HRESULT Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const Vector4& vClearColor);
		HRESULT Clear();
		HRESULT Bind_SRV(shared_ptr<CShader> pShader, const _char* pConstantName);

#ifdef _DEBUG
		HRESULT Init_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
		HRESULT Render(shared_ptr<CShader> pShader, shared_ptr<CRectProp> pRectBuffer);
#endif // _DEBUG


	private:
		ComPtr<ID3D11Device>				m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext>			m_pContext = nullptr;

		ComPtr<ID3D11Texture2D>				m_pTexture2D = nullptr;
		ComPtr<ID3D11RenderTargetView>		m_pRTV = nullptr;
		ComPtr<ID3D11ShaderResourceView>	m_pSRV = nullptr;

		Vector4 m_vClearColor = {};

#ifdef _DEBUG
	private:
		Matrix m_WorldMatrix = {};
#endif

	public:
		static shared_ptr<CRenderTarget> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const Vector4& vClearColor);
	};
}


