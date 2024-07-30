#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CShader;
	class CRenderTarget;

	class CTarget_Manager final
	{
	public:
		CTarget_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CTarget_Manager();

	public:
		HRESULT Initialize();

	public:
		HRESULT Add_RenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const Vector4& vClearColor);
		HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag);
		HRESULT Begin_MRT(const wstring& strMRTTag, ComPtr<ID3D11DepthStencilView> pDSView);
		HRESULT End_MRT();
		HRESULT Bind_SRV(const wstring& strTargetTag, const shared_ptr<CShader> pShader, const _char* pConstantName);

#ifdef _DEBUG
		HRESULT Init_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
		HRESULT Render_MRT(const wstring& strMRTTag, const shared_ptr<CShader> pShader);
#endif // _DEBUG

	private:
		shared_ptr<CRenderTarget> Find_RenderTarget(const wstring& strTargetTag);
		list <shared_ptr<CRenderTarget>>* Find_MRT(const wstring& strMRTTag);

	private:
		ComPtr<ID3D11Device>				m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext>			m_pContext = nullptr;

		ID3D11RenderTargetView*		m_pBackBufferView = nullptr;
		ID3D11DepthStencilView*		m_pDepthStencilView = nullptr;

	private:
		map<const wstring, shared_ptr<CRenderTarget>> m_RenderTargets;
		map <const wstring, list<shared_ptr<CRenderTarget>>> m_MRTs;

	public:
		static shared_ptr<CTarget_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

	};
}


