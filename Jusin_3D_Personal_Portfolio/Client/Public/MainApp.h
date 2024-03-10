#pragma once

#include "Engine_Defines.h"

namespace Client
{
	class CMainApp final
	{
	public:
		CMainApp();
		virtual ~CMainApp();

	public:
		HRESULT Initialize();
		void	Tick(_float fTimeDelta);
		HRESULT Render();

	private:
		HRESULT Open_Level(LEVEL eStartLevel);
		void Make_Layer_Collision_Matrix(vector<_uint>& collisionMatrix);

	private:
		ComPtr<ID3D11Device> m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext> m_pContext = nullptr;
		weak_ptr<CGameInstance> m_pGameInstance;

#ifdef _DEBUG
	private:
		wstring					m_strFPS = L"";
		_uint					m_iNumRender = { 0 };
		_float					m_fTimeAcc = { 0.0f };
#endif // _DEBUG

	public:
		static shared_ptr<CMainApp> Create();

	private:
		void Release();
	};
}

