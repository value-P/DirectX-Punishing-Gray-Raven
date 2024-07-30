#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CGameInstance;
}

namespace Tool
{
	class CTool_Loader final : public enable_shared_from_this<CTool_Loader>
	{
	public:
		CTool_Loader(LEVEL eNextLevel, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CTool_Loader();

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
		// 로고 진입 전 한번 모든 리소스 로드
		HRESULT Loading_Resources();
		// 해당 레벨 진입 전 오브젝트 로드
		HRESULT Loading_For_LogoLevel();
		HRESULT Loading_For_GamePlayLevel();

	public:
		static shared_ptr<CTool_Loader> Create(LEVEL eNextLevel, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	};
}
