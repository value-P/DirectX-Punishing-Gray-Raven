#pragma once
#include "GameObject.h"

namespace Client
{
	class CMonsterBase;
	class CParticleEffect;
	class CMovableObject;

    class CMonsterSpawner final : public CGameObject
    {
	public:
		struct INIT_DESC
		{
			LEVEL eCreateLevel;
			Vector3 vInitPos;
			shared_ptr<CMovableObject> pTargetChar;
			vector<Vector3> vSawManPositions;
			vector<Vector3> vRobotPositions;
		};

	public:
		CMonsterSpawner(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CMonsterSpawner() = default;

	public:
		virtual HRESULT Initialize(const INIT_DESC& initDesc);
		virtual _int Tick(_float fTimeDelta) override;
		virtual void Late_Tick(_float fTimeDelta) override;

	private:
		HRESULT Add_Component();
		void Spawn();

		virtual void OnCollisionEnter(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider);

	private:
		list<shared_ptr<CMonsterBase>> m_MonsterList;
		list<shared_ptr<CParticleEffect>> m_SpawnParticleList;
		shared_ptr<CCollider> m_pTrigger = nullptr;

	private:
		LEVEL eCreateLevel = LEVEL_END;

	public:
		static shared_ptr<CMonsterSpawner> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc);
    };
}


