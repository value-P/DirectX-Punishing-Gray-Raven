#pragma once

#include "GameObject.h"

namespace Engine
{
	class CSpriteCom;
}

namespace Tool
{
	class CTool_BackGround final: public CGameObject
	{
	public:
		CTool_BackGround(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CTool_BackGround() = default;

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
		static shared_ptr<CTool_BackGround> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureTag, Vector3 vInitPos);
	};
}
