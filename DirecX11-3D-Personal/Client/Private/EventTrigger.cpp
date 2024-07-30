#include "pch.h"
#include "EventTrigger.h"
#include "PlayableVera.h"

Client::CEventTrigger::CEventTrigger(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice, pContext)
{
}

HRESULT Client::CEventTrigger::Initialize(const INIT_DESC& initDesc)
{
	if (FAILED(__super::Initialize(initDesc.vInitPos)))
		return E_FAIL;

	eCurrentLevel = initDesc.eLevel;

	CBounding_Sphere::INIT_DESC sphereDesc;
	sphereDesc.fRadius = 5.f;
	sphereDesc.vCenter = Vector3(0.f, 0.f, 0.f);

	CCollider::INIT_DESC colliderDesc;
	colliderDesc.eType = CCollider::TYPE_SPHERE;
	colliderDesc.iCreateLevelIndex = initDesc.eLevel;
	colliderDesc.iLayerType = Col_Environment;
	colliderDesc.pInitDesc = make_shared<CBounding_Sphere::INIT_DESC>(sphereDesc);
	colliderDesc.pOwner = shared_from_this();

	m_pTrigger = CCollider::Create(m_pDevice, m_pContext, colliderDesc);
	if (nullptr == m_pTrigger)
		return E_FAIL;

	return S_OK;
}

_int Client::CEventTrigger::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	m_pTrigger->Tick(matWorld);

	return OBJ_ALIVE;
}

void Client::CEventTrigger::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	if (m_pTrigger->IsActivate())
		m_pGameInstance.lock()->Add_DebugComponent(m_pTrigger);
#endif
}

void Client::CEventTrigger::OnCollisionEnter(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider)
{
	if (Col_Player == pCollider->Get_ColliderLayer())
	{
		m_pTrigger->Set_Activate(false);
		static_pointer_cast<CPlayableVera>(pCollider->Get_Owner().lock())->PlayWinMotion();
		m_pGameInstance.lock()->Set_MainCamera(eCurrentLevel, L"Player_Win");
	}
}

shared_ptr<CEventTrigger> Client::CEventTrigger::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc)
{
	shared_ptr<CEventTrigger> pInstance = make_shared<CEventTrigger>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(initDesc)))
	{
		MSG_BOX("Failed to Created : CEventTrigger");
		pInstance.reset();
	}

	return pInstance;
}