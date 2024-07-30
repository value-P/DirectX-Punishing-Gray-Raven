#include "Object_Manager.h"
#include "Layer.h"
#include "GameObject.h"

Engine::CObject_Manager::CObject_Manager(_uint iNumLevels)
	:m_iLevelNum(iNumLevels)
{
	m_vecLayers.resize(iNumLevels);
}

HRESULT Engine::CObject_Manager::Initialize()
{
	return S_OK;
}

void Engine::CObject_Manager::Priority_Tick(_float fTimeDelta)
{
	for(auto& iter : m_vecLayers)
	{
		for (auto& iter2 : iter)
			iter2.second->Priority_Tick(fTimeDelta);
	}
}

void Engine::CObject_Manager::Tick(_float fTimeDelta)
{
	for (auto& iter : m_vecLayers)
	{
		for (auto& iter2 : iter)
			iter2.second->Tick(fTimeDelta);
	}
}

void Engine::CObject_Manager::Late_Tick(_float fTimeDelta)
{
	for (auto& iter : m_vecLayers)
	{
		for (auto& iter2 : iter)
			iter2.second->Late_Tick(fTimeDelta);
	}
}

shared_ptr<CLayer> Engine::CObject_Manager::Get_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	return Find_Layer(iLevelIndex, strLayerTag);
}

HRESULT Engine::CObject_Manager::Add_Object(_uint iLevelIndex, const wstring& strLayerTag, shared_ptr<CGameObject> pGameObject, const _float* pTimeScale)
{
	shared_ptr<CLayer> pLayer = Find_Layer(iLevelIndex, strLayerTag);
	
	if (pLayer == nullptr)
	{
		pLayer = CLayer::Create(pTimeScale);
		if (pLayer == nullptr)
			return E_FAIL;

		pLayer->Add_GameObject(pGameObject);

		m_vecLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	else
	{
		pLayer->Add_GameObject(pGameObject);
	}

	return S_OK;
}

void Engine::CObject_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_vecLayers.size()) return;

	m_vecLayers[iLevelIndex].clear();
}

void Engine::CObject_Manager::Clear_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (iLevelIndex >= m_vecLayers.size()) return;

	auto Pair = m_vecLayers[iLevelIndex].find(strLayerTag);

	if (Pair != m_vecLayers[iLevelIndex].end())
		Pair->second->Clear();
}

_bool Engine::CObject_Manager::RayPicking(_uint iLevelIndex, const Ray& ray, _float& fOutDist)
{
	if (iLevelIndex >= m_vecLayers.size()) return false;

	_bool result = false;
	_float nearestDist = 10000.f;
	_float tempDist = 0.f;
	for (auto& Pair : m_vecLayers[iLevelIndex])
	{		
		for (auto& iter : Pair.second->Get_ObjList())
		{
			if (iter->RayPicking(ray, tempDist))
			{
				if (!result) result = true;

				if (tempDist < nearestDist)
					nearestDist = tempDist;
			}
		}
	}

	fOutDist = tempDist;

	return result;
}

shared_ptr<CLayer> Engine::CObject_Manager::Find_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (iLevelIndex >= m_iLevelNum)
		return nullptr;

	auto iter = m_vecLayers[iLevelIndex].find(strLayerTag);

	if (iter == m_vecLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

shared_ptr<CObject_Manager> Engine::CObject_Manager::Create(_uint iNumLevels)
{
	shared_ptr<CObject_Manager> pInstance = make_shared<CObject_Manager>(iNumLevels);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
		pInstance.reset();
	}

	return pInstance;
}
