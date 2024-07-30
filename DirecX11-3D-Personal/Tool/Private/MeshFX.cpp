#include "pch.h"
#include "MeshFX.h"

Tool::CMeshFX::CMeshFX(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice, pContext)
{
}

void Tool::CMeshFX::Set_UVSpeed(Vector2 UVSpeed)
{
	m_pMeshCom->Set_UVMoveSpeed(UVSpeed);
}

HRESULT Tool::CMeshFX::Initialize(LEVEL eLevel, const wstring& strOwnerKey, const wstring& strEffectModelKey, Vector3 vInitPos)
{
	if (FAILED(__super::Initialize(vInitPos)))
		return E_FAIL;

	if (FAILED(Add_Component(eLevel, strOwnerKey, strEffectModelKey))) return E_FAIL;

	return S_OK;
}

void Tool::CMeshFX::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

_int Tool::CMeshFX::Tick(_float fTimeDelta)
{
	if (m_bActivate)
	{
		if (m_pMeshCom->isAnimEffect())
		{
			m_pMeshCom->Play_Animation(fTimeDelta, m_bLastFrameCalled, m_fFrameRate, false);

			if (m_fDuration > 0.f)
			{
				m_fTimeCheck += fTimeDelta;

				if (m_fTimeCheck >= m_fDuration)
				{
					m_fTimeCheck = 0.f;
					m_bActivate = false;
				}
			}
			else if (m_bLastFrameCalled)
			{
				m_pMeshCom->Set_KeyFrame(0);
				m_bActivate = false;
			}
		}
		else
		{
			m_fTimeCheck += fTimeDelta;

			if (m_fTimeCheck >= m_fDuration)
			{
				m_fTimeCheck = 0.f;
				m_bActivate = false;
			}
		}

		m_pMeshCom->UV_Move(fTimeDelta);
	}

	return OBJ_ALIVE;
}

void Tool::CMeshFX::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if(m_bActivate)
		m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_BLEND, shared_from_this());
}

HRESULT Tool::CMeshFX::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (m_pMeshCom->isAnimEffect())
	{
		if (m_bUsePrimeColor)
		{
			if (FAILED(m_pMeshCom->Bind_SRV("g_MaskTexture", m_pMaskTex)))
				return E_FAIL;

			for (_uint i = 0; i < m_pMeshCom->Get_MeshCount(); ++i)
			{
				if (FAILED(m_pMeshCom->Bind_BoneMatrices("g_BoneMatrices", i)))
					return E_FAIL;

				m_pMeshCom->Render(i, 3);
			}
		}
		else
		{
			for (_uint i = 0; i < m_pMeshCom->Get_MeshCount(); ++i)
			{
				// 텍스처 바인딩
				if (FAILED(m_pMeshCom->Bind_SRV("g_DiffuseTexture", i, MaterialTexType_DIFFUSE)))
					return E_FAIL;
				if (FAILED(m_pMeshCom->Bind_BoneMatrices("g_BoneMatrices", i)))
					return E_FAIL;

				m_pMeshCom->Render(i, 1);
			}
		}
	}
	else
	{
		if (m_bUsePrimeColor)
		{
			if (FAILED(m_pMeshCom->Bind_SRV("g_MaskTexture", m_pMaskTex)))
				return E_FAIL;

			for (_uint i = 0; i < m_pMeshCom->Get_MeshCount(); ++i)
			{
				m_pMeshCom->Render(i, 2);
			}
		}
		else
		{
			for (_uint i = 0; i < m_pMeshCom->Get_MeshCount(); ++i)
			{
				// 텍스처 바인딩
				if (FAILED(m_pMeshCom->Bind_SRV("g_DiffuseTexture", i, MaterialTexType_DIFFUSE)))
					return E_FAIL;

				m_pMeshCom->Render(i, 0);
			}
		}
	}

	return S_OK;
}

HRESULT Tool::CMeshFX::Add_Component(LEVEL eLevel, const wstring& strOwnerKey, const wstring& strEffectModelKey)
{
	auto pModelProp = m_pGameInstance.lock()->Get_AnimEffect(eLevel, strOwnerKey, strEffectModelKey);
	m_pBufferCom = m_pMeshCom = CEffectMeshCom::Create(m_pDevice, m_pContext, pModelProp);

	if (nullptr == m_pMeshCom)
		return E_FAIL;

	// 쉐이더 세팅
	if (FAILED(m_pMeshCom->Setting_Shader(m_pGameInstance.lock()->Get_Shader(L"Shader_VtxEffect"))))
		return E_FAIL;

	return S_OK;
}

HRESULT Tool::CMeshFX::Bind_ShaderResources()
{
	Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	Matrix ViewMatrix = m_pGameInstance.lock()->Get_ViewMatrix();
	Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();

	Vector2 vUVMove = m_pMeshCom->Get_UVMove();

	if (FAILED(m_pMeshCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pMeshCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pMeshCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;
	if(FAILED(m_pMeshCom->Bind_RawValue("g_UVMove", &vUVMove, sizeof(Vector2))))
		return E_FAIL;
	if(FAILED(m_pMeshCom->Bind_RawValue("g_fPrimaryColor", &m_vPrimeColor, sizeof(Vector4))))
		return E_FAIL;

	if (m_pMeshCom->isAnimEffect())
	{
		_float fFrameRate = m_fFrameRate;
		if (FAILED(m_pMeshCom->Bind_RawValue("g_fDurationRate", &fFrameRate , sizeof(_float))))
			return E_FAIL;
	}

	return S_OK;
}

shared_ptr<CMeshFX> Tool::CMeshFX::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eLevel, const wstring& strOwnerKey, const wstring& strEffectModelKey, Vector3 vInitPos)
{
	shared_ptr<CMeshFX> pInstance = make_shared<CMeshFX>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eLevel, strOwnerKey, strEffectModelKey, vInitPos)))
	{
		MSG_BOX("Failed to Created : CMeshFX");
		pInstance.reset();
	}

	return pInstance;
}
