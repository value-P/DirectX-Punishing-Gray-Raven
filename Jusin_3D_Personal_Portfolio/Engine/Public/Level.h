#pragma once
#include "Engine_Defines.h"
#include "GameInstance.h"

namespace Engine
{
	class CNavMeshProp;

	class ENGINE_DLL CLevel abstract
	{
	public:
		CLevel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CLevel() = default;

	public:
		virtual HRESULT Initialize();
		virtual void Tick(_float fTimeDelta);
		virtual HRESULT Render();

	protected:
		ComPtr<ID3D11Device> m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext> m_pContext = nullptr;
		weak_ptr<CGameInstance> m_pGameInstance;
		list<shared_ptr<CNavMeshProp>> m_pNavMeshProps;
	};
}
