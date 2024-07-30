#include "pch.h"
#include "MonsterBase.h"
#include "CameraReference.h"

Client::CMonsterBase::CMonsterBase(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CMovableObject(pDevice, pContext)
{
}

HRESULT Client::CMonsterBase::Initialize(const INIT_DESC& monsterDesc)
{
	m_fSpeed = monsterDesc.fSpeed;
	m_pTarget = monsterDesc.pTarget;
	m_iCurrentHp = m_iMaxHp = monsterDesc.iHp;
	m_iAttackPower = monsterDesc.iAttackPower;
	m_iCurrentShield = m_iMaxShield = monsterDesc.iShield;
	
	if(FAILED(__super::Initialize(monsterDesc.vInitPos)))
		return E_FAIL;

	return S_OK;
}

_int Client::CMonsterBase::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_iCurrentShield <= 0)
	{
		m_iCurrentShield = 0;
		if (m_StopWatchs[SW_SHIELD].Start != true)
			m_StopWatchs[SW_SHIELD].StartCheck();
		else
		{
			if (m_StopWatchs[SW_SHIELD].fTime >= 4.f)
			{
				m_iCurrentShield = m_iMaxShield;
				m_StopWatchs[SW_SHIELD].Clear();
			}
		}
	}

	return OBJ_ALIVE;
}

void Client::CMonsterBase::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_SHADOW, shared_from_this());
}

void Client::CMonsterBase::OnCollisionStay(shared_ptr<CCollider> pSrcCol, shared_ptr<CCollider> pCollider)
{
	if (pCollider->Get_ColliderLayer() == Col_Monster)
	{
		if (pSrcCol == m_MapColliderCom[L"Col_Body"])
		{
			Vector3 vDir = pCollider->Get_Center() - pSrcCol->Get_Center();
			_float fDist = vDir.Length();
			vDir.y = 0.f;
			vDir.Normalize();
			_float radiusAdd = pCollider->Get_Radius() + pSrcCol->Get_Radius();
			Vector3 vCurPos = m_pTransformCom->Get_Pos();
			m_pTransformCom->Set_Pos(vCurPos - vDir * (radiusAdd - fDist), m_pNavigationCom);
		}
	}
}

void Client::CMonsterBase::Move()
{
	if (m_pTarget.lock() != nullptr)
	{
		Vector3 vDir = m_pTarget.lock()->Get_Transform()->Get_Pos() - m_pTransformCom->Get_Pos();
		vDir.y = 0.f;
		vDir.Normalize();

		moveVelocity = vDir * m_fSpeed;

		Vector3 vLook = m_pTransformCom->Get_Look();
		vLook.y = 0;
		vLook.Normalize();

		if (vLook.Dot(vDir) > 0.f)
			vDir = CMathUtils::Slerp(vLook, vDir, 0.3f);
		else
			vDir = CMathUtils::Slerp(vLook, vDir, 0.8f);

		Vector4 vAt = XMVectorSetW(m_pTransformCom->Get_Pos() + vDir, 1.f);
		m_pTransformCom->LookAt(vAt);
	}
}