#include "pch.h"
#include "Tool_Player.h"

Tool::CTool_Player::CTool_Player(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice, pContext)
{
}

_uint Tool::CTool_Player::Get_CurrentAnimIndex()
{
	return m_pMeshCom->Get_CurrentAnimIndex();
}

HRESULT Tool::CTool_Player::Initialize(LEVEL eLevel, const wstring& strModelKey, Vector3 vInitPos)
{
	if (FAILED(__super::Initialize(vInitPos)))
		return E_FAIL;

	if (FAILED(Add_Component(eLevel, strModelKey)))
		return E_FAIL;

	return S_OK;
}

void Tool::CTool_Player::Priority_Tick(_float fTimeDelta)
{
}

_int Tool::CTool_Player::Tick(_float fTimeDelta)
{
	Key_Input();

	Vector3 moveVelocity;
	_bool lastKeyFrameCalled;
	_double keyFrameRate;

	if(!m_bStopAnim)
		m_pMeshCom->Play_Animation(fTimeDelta * m_AnimSpeed, moveVelocity, lastKeyFrameCalled, keyFrameRate, true, true, true);

	//m_pTransformCom->Go_Right(moveVelocity.x);
	//m_pTransformCom->Go_Straight(moveVelocity.z);
	return OBJ_ALIVE;
}

void Tool::CTool_Player::Late_Tick(_float fTimeDelta)
{
	if(FAILED(m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_PRIORITY, shared_from_this())))
		return;
}

HRESULT Tool::CTool_Player::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_pMeshCom->Get_MeshCount(); ++i)
	{
		if (FAILED(m_pMeshCom->Bind_SRV("g_DiffuseTexture", i, MaterialTexType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pMeshCom->Bind_BoneMatrices("g_BoneMatrices", i)))
			return E_FAIL;

		m_pMeshCom->Render(i, 0);
	}

	return S_OK;
}

void Tool::CTool_Player::Change_Anim(_uint animIndex)
{
	if (animIndex != 0 || animIndex <= m_pMeshCom->Get_AnimCount())
		m_pMeshCom->Set_Animation(animIndex);
}

void Tool::CTool_Player::Set_CurrentKeyFrame(_int iKeyFrame)
{
	m_pMeshCom->Set_KeyFrame(iKeyFrame);
}

void Tool::CTool_Player::Key_Input()
{
	if (m_pGameInstance.lock()->GetKeyDown(DIK_UPARROW))
	{
		m_pMeshCom->Set_AnimationNext();
	}
	else if (m_pGameInstance.lock()->GetKeyDown(DIK_DOWNARROW))
	{
		m_pMeshCom->Set_AnimationPrev();
	}
	
	if (m_pGameInstance.lock()->GetKeyDown(DIK_SPACE))
		m_bStopAnim = !m_bStopAnim;
}

HRESULT Tool::CTool_Player::Add_Component(LEVEL eLevel, const wstring& strModelKey)
{
	auto pModelProp = m_pGameInstance.lock()->Get_AnimModel(eLevel, strModelKey);
	m_pBufferCom = m_pMeshCom = CSkeletalMeshCom::Create(m_pDevice, m_pContext, pModelProp);

	if (nullptr == m_pMeshCom)
		return E_FAIL;

	if (FAILED(m_pMeshCom->Setting_Shader(m_pGameInstance.lock()->Get_Shader(L"Shader_VtxAnimMesh"))))
		return E_FAIL;

	return S_OK;
}

HRESULT Tool::CTool_Player::Bind_ShaderResources()
{
	Matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	Matrix matView = m_pGameInstance.lock()->Get_ViewMatrix();
	Matrix matProj = m_pGameInstance.lock()->Get_ProjMatrix();

	if (FAILED(m_pMeshCom->Bind_Matrix("g_WorldMatrix", &matWorld)))
		return E_FAIL;
	if (FAILED(m_pMeshCom->Bind_Matrix("g_ViewMatrix", &matView)))
		return E_FAIL;
	if (FAILED(m_pMeshCom->Bind_Matrix("g_ProjMatrix", &matProj)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CTool_Player> Tool::CTool_Player::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eLevel, const wstring& strModelKey,Vector3 vInitPos)
{
	shared_ptr<CTool_Player> pInstance = make_shared<CTool_Player>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eLevel, strModelKey, vInitPos)))
	{
		MSG_BOX("Failed to Created : CTool_MapMesh");
		pInstance.reset();
	}

	return pInstance;
}
