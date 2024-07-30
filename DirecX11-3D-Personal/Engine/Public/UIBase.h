#pragma once
#include "GameObject.h"

namespace Engine
{
	class CPointProp;
	class CTexture;
	class CShader;

	class ENGINE_DLL CUIBase abstract : public CGameObject
	{
	public:
		struct INIT_DESC
		{
			wstring strTexKey;
			Vector2 vSize;
			Vector2 vPos;
		};

	public:
		CUIBase(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CUIBase() = default;

	public:
		void Activate() { m_bActivate = true; }
		virtual void InActivate() { m_bActivate = false; }
		_bool isActivate() { return m_bActivate; }

		_bool isMouseIn(HWND clientHandle);

	public:
		virtual HRESULT Initialize(const INIT_DESC& initDesc);

	public:
		virtual void OnClickEnter() {}
		virtual void OnClickStay() {}
		virtual void OnClickExit() {}

	protected:
		_bool m_bActivate = true;
		shared_ptr<CPointProp> m_pPointBuffer = nullptr;
		shared_ptr<CTexture> m_pTexture = nullptr;
		shared_ptr<CShader> m_pShader = nullptr;
		Vector2 m_vPosition = {};
		Vector2 m_vSize = {};

		Vector2 m_vViewPortSize = {};

		Matrix m_ViewMatrix = XMMatrixIdentity();
		Matrix m_ProjMatrix = XMMatrixIdentity();
	};
}


