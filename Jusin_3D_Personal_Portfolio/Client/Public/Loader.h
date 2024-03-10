#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CGameInstance;
}

namespace Client
{
	class CLoader final : public enable_shared_from_this<CLoader>
	{
	public:
		CLoader(LEVEL eNextLevel, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CLoader();

	public:
		HRESULT Initialize();
		HRESULT Loading_Selector();
			
	public:
		_bool isFinished() const { return m_isFinished; }

#ifdef _DEBUG
	public:
		void Output_LoadingText();
#endif

	private:
		wstring	m_strLoadingText = {};
		_bool			m_isFinished = false;
		LEVEL			m_eNextLevel = LEVEL_END;
		mutex			m_Mutex;
		thread          m_thread;

		ComPtr<ID3D11Device> m_pDevice	= nullptr;
		ComPtr<ID3D11DeviceContext> m_pContext = nullptr;
		weak_ptr<CGameInstance> m_pGameInstance;

	private:
		// 해당 레벨 진입 전 오브젝트 로드
		HRESULT Loading_For_LogoLevel();
		HRESULT Loading_For_GamePlayLevel();
		HRESULT Loading_For_BossLevel();
		HRESULT Loading_For_BossLevel2();

	public:
		static shared_ptr<CLoader> Create(LEVEL eNextLevel, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	};
}
