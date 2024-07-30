#include "NavMeshManager.h"
#include "NavMeshProp.h"
#include "Shader.h"
#include "GameInstance.h"
#include "Cell.h"

Engine::CNavMeshManager::CNavMeshManager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT Engine::CNavMeshManager::Initialize(_uint iLevelNum)
{
	m_NavMeshProps.resize(iLevelNum);

	return S_OK;
}

_bool Engine::CNavMeshManager::isMove(_uint iLevelIndex, const Vector3& vPosition, _int& iCurrentIndex)
{
	if (m_NavMeshProps[iLevelIndex] == nullptr)
		return false;

	return m_NavMeshProps[iLevelIndex]->isMove(vPosition, iCurrentIndex);
}

_int Engine::CNavMeshManager::GetIndexByPosition(_uint iLevelIndex, const Vector3& vPosition)
{
	if (m_NavMeshProps[iLevelIndex] == nullptr)
		return -1;

	return m_NavMeshProps[iLevelIndex]->Get_IndexFromPos(vPosition);
}

void Engine::CNavMeshManager::Tick(_uint iLevelIndex)
{
	if (m_NavMeshProps[iLevelIndex] != nullptr)
		m_NavMeshProps[iLevelIndex]->Tick();
}

HRESULT Engine::CNavMeshManager::Set_NavMesh(_uint iLevelIndex, shared_ptr<CNavMeshProp> pNavMeshProp, shared_ptr<CGameObject> pCombinedObject)
{
	if (pNavMeshProp == nullptr || pCombinedObject == nullptr)
		return E_FAIL;

	m_NavMeshProps[iLevelIndex] = pNavMeshProp;
	if (FAILED(m_NavMeshProps[iLevelIndex]->Setting_AttachedObject(pCombinedObject)))
		return E_FAIL;

	return S_OK;
}

_float Engine::CNavMeshManager::Get_HeightByIndex(_uint iLevelIndex, _int currentIndex, const Vector3& vPosition)
{
	auto cell = m_NavMeshProps[iLevelIndex]->Get_Cells()[currentIndex];

	Plane plane(cell->Get_Point(CCell::POINT_A), cell->Get_Point(CCell::POINT_B), cell->Get_Point(CCell::POINT_C));
	
	return (-plane.x * vPosition.x - plane.z * vPosition.z - plane.w) / plane.y;
}

void Engine::CNavMeshManager::Clear(_uint iLevelIndex)
{
	m_NavMeshProps[iLevelIndex].reset();
}

#ifdef _DEBUG
HRESULT Engine::CNavMeshManager::Render(_uint iLevelIndex)
{
	if (m_NavMeshProps[iLevelIndex] == nullptr)
		return S_OK;

	m_NavMeshProps[iLevelIndex]->Render();

	return S_OK;
}
#endif

shared_ptr<CNavMeshManager> Engine::CNavMeshManager::Create(_uint iLevelNum)
{
	shared_ptr<CNavMeshManager> pInstance = make_shared<CNavMeshManager>();

	if (FAILED(pInstance->Initialize(iLevelNum)))
	{
		MSG_BOX("Failed to Created : CNavMeshProp");
		pInstance.reset();
	}

	return pInstance;
}
