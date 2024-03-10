#pragma once

#include "Engine_Defines.h"

namespace Engine
{
	class CGameObject;

	class CLayer final
	{
	public:
		CLayer() = default;
		virtual ~CLayer() = default;

	public:
		HRESULT Initialize(const _float* pTimeScale);
		HRESULT Add_GameObject(shared_ptr<CGameObject> pGameObject);
		void Priority_Tick(_float fTimeDelta);
		void Tick(_float fTimeDelta);
		void Late_Tick(_float fTimeDelta);

	public:
		void Clear();

	public:
		list<shared_ptr<CGameObject>>& Get_ObjList() { return m_GameObjectList; }

	private:
		list<shared_ptr<CGameObject>>		m_GameObjectList;
		const _float*								m_pTimeScale = nullptr;

	public:
		static shared_ptr<CLayer> Create(const _float* pTimeScale);
	};
}
