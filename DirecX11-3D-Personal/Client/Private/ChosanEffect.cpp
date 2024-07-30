#include "pch.h"
#include "ChosanEffect.h"
#include "AnimEffectProp.h"

CChosanEffect::CChosanEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CFX(pDevice, pContext)
{
}

HRESULT CChosanEffect::Initialize(const INIT_DESC& InitDesc)
{
    if (FAILED(__super::Initialize(Vector3(0.f,0.f,0.f))))
        return E_FAIL;

	m_fEffectMoveSpeed = InitDesc.fMoveSpeed;
	m_fMinScale = InitDesc.fStartScale;
	m_fMaxScale = InitDesc.fEndScale;

	shared_ptr<CAnimEffectProp> pChosanBuffer = CAnimEffectProp::Create(m_pDevice, m_pContext);
	pChosanBuffer->Add_Buffer(InitDesc.strEffectBufferPath, XMMatrixIdentity());

	for (auto& pMeshCom : m_EffectComs)
	{
		pMeshCom = CEffectMeshCom::Create(m_pDevice, m_pContext, pChosanBuffer);
		pMeshCom->Setting_Shader(m_pGameInstance.lock()->Get_Shader(L"Shader_VtxSuperDodgeEffect"));
	}

	for (auto& pTransformCom : m_EffectTransforms)
		pTransformCom = CTransform::Create();

	// +x, -x¸é 0,1
	m_EffectTransforms[0]->Rotation(Vector3(0.f, 90.f,0.f));
	m_EffectTransforms[1]->Rotation(Vector3(0.f, 90.f, 0.f));
	// +z, -z¸é 2,3
	//m_EffectTransforms[2]->Rotation(Vector3(0.f,0.f,0.f));
	//m_EffectTransforms[3]->Rotation(Vector3(0.f,0.f,0.f));
	// +y, -y¸é 4,5
	m_EffectTransforms[4]->Rotation(Vector3(90.f, 0.f, 0.f));
	m_EffectTransforms[5]->Rotation(Vector3(90.f, 0.f, 0.f));

	m_pTexture = m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, L"TO14_T_FX_noise_ny_AlphaC09");

    return S_OK;
}

_int CChosanEffect::Tick(_float fTimeDelta)
{
	if (false == m_bActivate)
		return OBJ_ALIVE;

	m_fDurationCheck += fTimeDelta;

	if (m_fDuration/3.f > m_fDurationCheck)
	{
		//m_fCurrentScale = CMathUtils::lerp(m_fCurrentScale, m_fMaxScale, m_fDurationCheck / (m_fDuration / 3.f));
		m_fCurrentScale = CMathUtils::lerp(m_fCurrentScale, m_fMaxScale, 0.05f);
		m_fChangableSpeed = CMathUtils::lerp(m_fChangableSpeed, 0.f, 0.05f);
		// +x, -x
		m_EffectTransforms[0]->Go_Direction(Vector3(1.f, 0.f, 0.f), m_fChangableSpeed * fTimeDelta);
		m_EffectTransforms[1]->Go_Direction(Vector3(-1.f, 0.f, 0.f), m_fChangableSpeed * fTimeDelta);
		// +z, -z
		m_EffectTransforms[2]->Go_Direction(Vector3(0.f, 0.f, 1.f), m_fChangableSpeed * fTimeDelta);
		m_EffectTransforms[3]->Go_Direction(Vector3(0.f, 0.f, -1.f), m_fChangableSpeed * fTimeDelta);
		// +y, -y
		m_EffectTransforms[4]->Go_Direction(Vector3(0.f, 1.f, 0.f), m_fChangableSpeed * fTimeDelta);
		m_EffectTransforms[5]->Go_Direction(Vector3(0.f, -0.03f, 0.f), m_fChangableSpeed * fTimeDelta);


		for (auto& pTransform : m_EffectTransforms)
			pTransform->Set_Scaling(m_fCurrentScale, m_fCurrentScale, m_fCurrentScale);
	}
	else if (m_fDuration <= m_fDurationCheck)
	{
		m_fDurationCheck = 0.f;
		m_bActivate = false;
	}	


    return OBJ_ALIVE;
}

void CChosanEffect::Late_Tick(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;

	m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_BLEND, shared_from_this());

	if (m_pGameInstance.lock()->isDarkMode())
	{
		m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_EXCEPTDARK, shared_from_this());
	}
}

HRESULT CChosanEffect::Render()
{
	Matrix WorldMatrix;
	_float fDurationRate = m_fDurationCheck / m_fDuration;
	Matrix ViewMatrix = m_pGameInstance.lock()->Get_ViewMatrix();
	Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();
	for (_int iEffectIndex = 0; iEffectIndex < 6; ++iEffectIndex)
	{
		WorldMatrix = m_EffectTransforms[iEffectIndex]->Get_WorldMatrix() * m_pTransformCom->Get_WorldMatrix();

		if (FAILED(m_EffectComs[iEffectIndex]->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;
		if (FAILED(m_EffectComs[iEffectIndex]->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_EffectComs[iEffectIndex]->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
			return E_FAIL;
		if (FAILED(m_EffectComs[iEffectIndex]->Bind_RawValue("g_fDurationRate", &fDurationRate, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_EffectComs[iEffectIndex]->Bind_SRV("g_Texture", m_pTexture)))
			return E_FAIL;

		for (_uint i = 0; i < m_EffectComs[iEffectIndex]->Get_MeshCount(); ++i)
			m_EffectComs[iEffectIndex]->Render(i, 0);
	}

    return S_OK;
}

HRESULT Client::CChosanEffect::Render_ExceptDark()
{
	Matrix WorldMatrix;
	Matrix ViewMatrix = m_pGameInstance.lock()->Get_ViewMatrix();
	Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();
	for (_int iEffectIndex = 0; iEffectIndex < 6; ++iEffectIndex)
	{
		WorldMatrix = m_EffectTransforms[iEffectIndex]->Get_WorldMatrix() * m_pTransformCom->Get_WorldMatrix();

		if (FAILED(m_EffectComs[iEffectIndex]->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;
		if (FAILED(m_EffectComs[iEffectIndex]->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_EffectComs[iEffectIndex]->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
			return E_FAIL;

		for (_uint i = 0; i < m_EffectComs[iEffectIndex]->Get_MeshCount(); ++i)
			m_EffectComs[iEffectIndex]->Render(i, 1);
	}

	return S_OK;
}

void CChosanEffect::PlayOnce()
{
	__super::PlayOnce();
	m_fDurationCheck = 0.f;
	m_fChangableSpeed = m_fEffectMoveSpeed;
	m_fCurrentScale = m_fMinScale;

	for (auto& pTransform : m_EffectTransforms)
		pTransform->Set_Pos(Vector3(0.f, 0.f, 0.f));
}

shared_ptr<CChosanEffect> CChosanEffect::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& InitDesc)
{
	shared_ptr<CChosanEffect> pInstance = make_shared<CChosanEffect>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(InitDesc)))
	{
		MSG_BOX("Failed to Created : CChosanEffect");
		pInstance.reset();
	}

	return pInstance;
}