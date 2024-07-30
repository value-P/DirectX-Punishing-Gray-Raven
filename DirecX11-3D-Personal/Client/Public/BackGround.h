#pragma once

#include "GameObject.h"

namespace Engine
{
	class CSpriteCom;
}

namespace Client
{
	class CBackGround final: public CGameObject
	{
	public:
		CBackGround(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CBackGround() = default;

	public:
		virtual HRESULT Initialize(const wstring& strTextureTag, Vector3 vInitPos);
		virtual void Priority_Tick(_float fTimeDelta);
		virtual _int Tick(_float fTimeDelta);
		virtual void Late_Tick(_float fTimeDelta);
		virtual HRESULT Render();

	private:
		HRESULT Add_Component(const wstring& strTextureTag);

	private:
		shared_ptr<CSpriteCom> m_pRectCom = nullptr;
		_float				m_fX, m_fY, m_fSizeX, m_fSizeY;

	public:
		static shared_ptr<CBackGround> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureTag, Vector3 vInitPos);
	};
}
