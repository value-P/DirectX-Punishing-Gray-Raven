#pragma once

#include "Engine_Defines.h"

namespace Engine
{
	class CLayer;
	class CGameObject;

	class CObject_Manager final
	{
	public:
		CObject_Manager(_uint iNumLevels);
		virtual ~CObject_Manager() = default;

	public:
		HRESULT Initialize();
		void Priority_Tick(_float fTimeDelta);
		void Tick(_float fTimeDelta);
		void Late_Tick(_float fTimeDelta);

	public:
		shared_ptr<CLayer> Get_Layer(_uint iLevelIndex, const wstring& strLayerTag);
		HRESULT Add_Object(_uint iLevelIndex, const wstring& strLayerTag, shared_ptr<CGameObject> pGameObject, const _float* pTimeScale);
		void Clear(_uint iLevelIndex);
		void Clear_Layer(_uint iLevelIndex, const wstring& strLayerTag);

		_bool RayPicking(_uint iLevelIndex, const Ray& ray, _float& fOutDist);

	private:
		shared_ptr<CLayer> Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);

	private:
		_uint	m_iLevelNum = 0;
		
		typedef map<const wstring, shared_ptr<CLayer>>	LAYERS;
		vector<LAYERS> m_vecLayers;

	public:
		static shared_ptr<CObject_Manager> Create(_uint iNumLevels);
	};

}

