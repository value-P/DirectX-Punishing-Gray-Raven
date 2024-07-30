#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CGameObject;
	class CComponent;
	class CShader;
	class CRectProp;
	class CGameInstance;

	class CRenderer final
	{
	public:
		struct INIT_DESC
		{
			_float iUpScalingDepthStencilSizeX;
			_float iUpScalingDepthStencilSizeY;
		};

	public:
		enum RENDERGROUP { RENDER_PRIORITY, RENDER_SHADOW, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND, RENDER_DISTORT, RENDER_EXCEPTDARK, RENDER_UI, RENDER_END };

	public:
		CRenderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CRenderer() = default;

	public:
		void DarkWindowActivate(_float fDarkDuration);
		_bool isDarkMode() { return m_bUseDarkMode; }

	public:
		HRESULT Initialize(const INIT_DESC& initDesc);
		void Tick(_float fTimeDelta);
		HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, shared_ptr<CGameObject> pGameObject);
		HRESULT Render();
		void Clear();

#ifdef _DEBUG
	public:
		HRESULT Add_DebugComponent(shared_ptr<CComponent> pComponent);
#endif // _DEBUG

	private:
		HRESULT Render_Priority();
		HRESULT Render_NonBlend();
		HRESULT Render_Lights();
		HRESULT Render_Final();
		HRESULT Render_NonLight();
		HRESULT Render_Blend();
		HRESULT Render_UI();
		HRESULT Render_Shadow();

		HRESULT Render_Distortion();
		HRESULT Render_Outline();
		HRESULT Render_DarkMode();

#ifdef _DEBUG
		HRESULT Render_Debug();
#endif // _DEBUG

		HRESULT Copy_BackBufferSRV();

	private:
		ComPtr<ID3D11Device>			m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext>		m_pContext = nullptr;
		weak_ptr<CGameInstance>			m_pGameInstance;
		ComPtr<ID3D11DepthStencilView>	m_pShadowDepthStencilView = nullptr;

		// ����� ���纻 SRV
		ComPtr<ID3D11ShaderResourceView> m_pBackBufferCopySRV = nullptr;
		ComPtr<ID3D11Texture2D>			 m_pBackBufferCopyTex2D = nullptr;

	private:
		list<shared_ptr<CGameObject>>	m_RenderObjectList[RENDER_END];

	private:
		shared_ptr<CShader>				m_pShader = nullptr;
		shared_ptr<CShader>				m_pPostProcessShader = nullptr;
		shared_ptr<CRectProp>			m_pRectProp = nullptr;

		Matrix m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

		_float		m_fUpScalingSizeX = 8192.f;
		_float		m_fUpScalingSizeY = 4608.f;
		_float		m_fScreenSizeX = 0.f;
		_float		m_fScreenSizeY = 0.f;

		// ȭ�� ��Ӱ� �Ҷ� ����ϴ� ����
		_float		m_fDarkWindowDurationCheck = 0.f;
		_float		m_fDarkWindowDurationOrigin = 0.f;
		_bool		m_bUseDarkMode = false;

#ifdef _DEBUG
		list<shared_ptr<CComponent>> m_DebugCom;
#endif // _DEBUG

	public:
		static shared_ptr<CRenderer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc);
	};
}
