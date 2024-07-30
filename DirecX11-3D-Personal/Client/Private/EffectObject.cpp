#include "pch.h"
#include "EffectObject.h"

Client::CEffectObject::CEffectObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice, pContext)
{
}

HRESULT Client::CEffectObject::Initialize(const INIT_DESC& initDesc)
{
	if (FAILED(__super::Initialize(initDesc.vInitPos)))
		return E_FAIL;

	if (FAILED(Add_Component(initDesc.eCreateLevel)))
		return E_FAIL;

	m_vOriginPos = initDesc.vInitPos;
	m_fSpeed = initDesc.fSpeed;
	m_iDamage = initDesc.iDamage;
	m_fDuration = initDesc.fDuration;

	return S_OK;
}

void Client::CEffectObject::Priority_Tick(_float fTimeDelta)
{
	if (m_pCollider->IsActivate() == false) return;

	__super::Priority_Tick(fTimeDelta);
}

_int Client::CEffectObject::Tick(_float fTimeDelta)
{
	if (m_pCollider->IsActivate() == false) return OBJ_ALIVE;

	__super::Tick(fTimeDelta);

	if (m_pCollider->IsActivate())
	{
		m_fDurationCheck -= fTimeDelta;
		if (m_fDurationCheck <= 0.f)
		{
			InActivate();
			return OBJ_ALIVE;
		}

		if (nullptr != m_pParentTransform.lock())
			m_matWorld = m_pTransformCom->Get_WorldMatrix() * m_pParentTransform.lock()->Get_WorldMatrix();
		else
			m_matWorld = m_pTransformCom->Get_WorldMatrix();

		m_pCollider->Tick(m_matWorld);
	}

	return OBJ_ALIVE;
}

void Client::CEffectObject::Late_Tick(_float fTimeDelta)
{
	if (m_pCollider->IsActivate() == false) return;

	__super::Late_Tick(fTimeDelta);

	if (FAILED(m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_BLEND, shared_from_this())))
		return;

#ifdef _DEBUG
	if (m_pCollider != nullptr)
		m_pGameInstance.lock()->Add_DebugComponent(m_pCollider);
#endif
}

HRESULT Client::CEffectObject::Render()
{
	if (m_pCollider->IsActivate() == false) return S_OK;

	__super::Render();

	return S_OK;
}

HRESULT Client::CEffectObject::Add_Component(LEVEL eCreateLevel)
{
	return S_OK;
}
