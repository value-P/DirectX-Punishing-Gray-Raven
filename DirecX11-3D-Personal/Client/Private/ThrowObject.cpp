#include "pch.h"
#include "ThrowObject.h"
#include "MovableObject.h"
#include "MeshEffect.h"

Client::CThrowObject::CThrowObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CEffectObject(pDevice, pContext)
{
}

HRESULT Client::CThrowObject::Initialize(const INIT_DESC& initDesc)
{
	if (FAILED(__super::Initialize(initDesc)))
		return E_FAIL;

	if (FAILED(Add_Component(initDesc)))
		return E_FAIL;

	if (initDesc.strSkillPrefabPath_1 != L"")
	{
		CMeshEffect::INIT_DESC desc;
		desc.eLevel = initDesc.eCreateLevel;
		desc.FXPrefabPath = initDesc.strSkillPrefabPath_1;
		desc.vInitPos = Vector3(0.f, 0.f, 0.f);

		m_pMeshEffect1 = CMeshEffect::Create(m_pDevice, m_pContext, desc);
		m_pMeshEffect1->PlayLoop();
	}

	if (initDesc.strSkillPrefabPath_2 != L"")
	{
		CMeshEffect::INIT_DESC desc;
		desc.eLevel = initDesc.eCreateLevel;
		desc.FXPrefabPath = initDesc.strSkillPrefabPath_2;
		desc.vInitPos = Vector3(0.f, 0.f, 0.f);

		m_pMeshEffect2 = CMeshEffect::Create(m_pDevice, m_pContext, desc);
		m_pMeshEffect2->PlayLoop();
	}

	return S_OK;
}

void Client::CThrowObject::Priority_Tick(_float fTimeDelta)
{
	if (m_pCollider->IsActivate() == false) return;

	__super::Priority_Tick(fTimeDelta);
	
	if (nullptr != m_pMeshEffect1)
		m_pMeshEffect1->Priority_Tick(fTimeDelta);
	if (nullptr != m_pMeshEffect2)
		m_pMeshEffect2->Priority_Tick(fTimeDelta);
}

_int Client::CThrowObject::Tick(_float fTimeDelta)
{
	if (m_pCollider->IsActivate() == false) return OBJ_ALIVE;

	__super::Tick(fTimeDelta);

	if (m_pCollider->IsActivate())
		m_pTransformCom->Go_Direction(m_vThrowDir, m_fSpeed * fTimeDelta);

	m_matWorld = m_pTransformCom->Get_WorldMatrix();

	if (nullptr != m_pMeshEffect1)
		m_pMeshEffect1->Tick(fTimeDelta, m_matWorld);
	if (nullptr != m_pMeshEffect2)
		m_pMeshEffect2->Tick(fTimeDelta, m_matWorld);

	return OBJ_ALIVE;
}

void Client::CThrowObject::Late_Tick(_float fTimeDelta)
{
	if (m_pCollider->IsActivate() == false) return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pMeshEffect1)
		m_pMeshEffect1->Late_Tick(fTimeDelta);
	if (nullptr != m_pMeshEffect2)
		m_pMeshEffect2->Late_Tick(fTimeDelta);

	//m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT Client::CThrowObject::Render()
{
	__super::Render();
		
	return S_OK;
}

void Client::CThrowObject::Throw(const Vector3& vStartPos, const Vector3& vTargetPos)
{
	m_pTransformCom->Set_Pos(vStartPos);

	Vector3 vDir = vTargetPos - vStartPos;
	vDir.Normalize();
	
	m_pTransformCom->LookAt(XMVectorSetW(vStartPos + vDir,1.f));

	m_vThrowDir = vDir;
	__super::Activate();
}

void Client::CThrowObject::InActivate()
{
	__super::InActivate();

	m_vThrowDir = Vector3(0.f, 0.f, 0.f);
}

HRESULT Client::CThrowObject::Add_Component(const INIT_DESC& initDesc)
{
	CBounding_Sphere::INIT_DESC sphereColDesc;
	sphereColDesc.vCenter = Vector3(0.f, 0.f, 0.f);
	sphereColDesc.fRadius = initDesc.fColRadius;

	CCollider::INIT_DESC colliderDesc;
	colliderDesc.eType = CCollider::TYPE_SPHERE;
	colliderDesc.iLayerType = Col_MonsterAttack;
	colliderDesc.pInitDesc = make_shared<CBounding_Sphere::INIT_DESC>(sphereColDesc);
	colliderDesc.pOwner = shared_from_this();
	colliderDesc.iCreateLevelIndex = initDesc.eCreateLevel;
	m_pCollider = CCollider::Create(m_pDevice, m_pContext, colliderDesc);
	m_pCollider->Set_Activate(false);

	return S_OK;
}

HRESULT Client::CThrowObject::Bind_ShaderResources()
{
	return S_OK;
}

void Client::CThrowObject::OnCollisionEnter(const shared_ptr<CCollider> pSrcCol, const shared_ptr<CCollider> pCollider)
{
	if (pCollider->Get_ColliderLayer() == Col_Player)
	{
		static_pointer_cast<CMovableObject>(pCollider->Get_Owner().lock())->Hit(m_iDamage);
	}
}

shared_ptr<CThrowObject> Client::CThrowObject::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc)
{
	shared_ptr<CThrowObject> pInstance = make_shared<CThrowObject>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(initDesc)))
	{
		MSG_BOX("Failed to Created : CThrowObject");
		pInstance.reset();
	}

	return pInstance;
}