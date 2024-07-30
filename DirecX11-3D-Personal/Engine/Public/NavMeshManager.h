#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CNavMeshProp;
	class CGameObject;
	class CShader;
	class CGameInstance;

	class CNavMeshManager final
	{
	public:
		CNavMeshManager();
		virtual ~CNavMeshManager() = default;

	public:
		HRESULT Initialize(_uint iLevelNum);

		_bool isMove(_uint iLevelIndex, const Vector3& vPosition, _int& iCurrentIndex);
		_int GetIndexByPosition(_uint iLevelIndex, const Vector3& vPosition);

		HRESULT Set_NavMesh(_uint iLevelIndex, shared_ptr<CNavMeshProp> pNavMeshProp, shared_ptr<CGameObject> pCombinedObject);
		_float Get_HeightByIndex(_uint iLevelIndex, _int currentIndex, const Vector3& vPosition);;

	public:
		void Tick(_uint iLevelIndex);
		void Clear(_uint iLevelIndex);

#ifdef _DEBUG
	public:
		HRESULT Render(_uint iLevelIndex);
#endif

	private:
		weak_ptr<CGameInstance>		m_pGameInstance;
		vector<shared_ptr<CNavMeshProp>> m_NavMeshProps;

	public:
		static shared_ptr<CNavMeshManager> Create(_uint iLevelNum);
	};
}


