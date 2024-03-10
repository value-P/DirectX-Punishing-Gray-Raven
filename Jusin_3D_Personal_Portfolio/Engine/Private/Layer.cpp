#include "Layer.h"
#include "GameObject.h"

HRESULT Engine::CLayer::Initialize(const _float* pTimeScale)
{
	if (nullptr == pTimeScale)
		return E_FAIL;

	m_pTimeScale = pTimeScale;

	return S_OK;
}

HRESULT Engine::CLayer::Add_GameObject(shared_ptr<CGameObject> pGameObject)
{
	if (pGameObject == nullptr)
		return E_FAIL;

	m_GameObjectList.push_back(pGameObject);

	return S_OK;
}

void Engine::CLayer::Priority_Tick(_float fTimeDelta)
{
	for (auto& iter : m_GameObjectList)
	{
		if (iter != nullptr)
			iter->Priority_Tick(fTimeDelta * (*m_pTimeScale));
	}
}

void Engine::CLayer::Tick(_float fTimeDelta)
{
	for (auto iter = m_GameObjectList.begin(); iter != m_GameObjectList.end();)
	{
		if ((*iter) != nullptr)
		{
			_int iResult = (*iter)->Tick(fTimeDelta * (*m_pTimeScale));
			if (iResult == OBJ_DESTROY)
			{
				iter->reset();
				iter = m_GameObjectList.erase(iter);
				continue;
			}
		}
		
		++iter;
	}
}

void Engine::CLayer::Late_Tick(_float fTimeDelta)
{
	for (auto& iter : m_GameObjectList)
	{
		if (iter != nullptr)
			iter->Late_Tick(fTimeDelta * (*m_pTimeScale));
	}
}

void Engine::CLayer::Clear()
{
	m_GameObjectList.clear();
}

shared_ptr<CLayer> Engine::CLayer::Create(const _float* pTimeScale)
{
	shared_ptr<CLayer> pInstance = make_shared<CLayer>();

	if (FAILED(pInstance->Initialize(pTimeScale)))
	{
		MSG_BOX("Failed to Created : CLayer");
		pInstance.reset();
	}

	return pInstance;
}
