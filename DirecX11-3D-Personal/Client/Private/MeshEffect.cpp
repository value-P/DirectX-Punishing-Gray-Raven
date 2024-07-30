#include "pch.h"
#include "MeshEffect.h"

Client::CMeshEffect::CMeshEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CFX(pDevice, pContext)
{
}

HRESULT Client::CMeshEffect::Initialize(const INIT_DESC& InitDesc)
{
	if (FAILED(__super::Initialize(InitDesc.vInitPos)))
		return E_FAIL;

	if (FAILED(Add_Component(InitDesc.eLevel, InitDesc.FXPrefabPath)))
		return E_FAIL;

	return S_OK;
}

void Client::CMeshEffect::Priority_Tick(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	__super::Priority_Tick(fTimeDelta);
}

_int Client::CMeshEffect::Tick(_float fTimeDelta)
{
	if (!m_bActivate)
		return OBJ_ALIVE;

	__super::Tick(fTimeDelta);

	if(m_pEffectCom->isAnimEffect())
		m_pEffectCom->Play_Animation(fTimeDelta, m_lastFrameCalled, m_fFrameRate, false);

	m_pEffectCom->UV_Move(fTimeDelta);

	// 무한루프 옵션이 안켜져있다면
	if (!m_bUseLoop)
	{
		// 애니메이션이 있으면
		if (m_pEffectCom->isAnimEffect() && m_fDuration == 0.f)
		{
			if (m_lastFrameCalled)
				Stop();
		}
		// 애니메이션이 없거나 있더라도 duration이 존재하면 duration기준으로 꺼지도록
		else
		{
			m_fDurationCheck += fTimeDelta;

			if (m_fDurationCheck >= m_fDuration)
				Stop();
		}
	}

	return OBJ_ALIVE;
}

_int Client::CMeshEffect::Tick(_float fTimeDelta, const Matrix& socket)
{
	if (!m_bActivate)
		return OBJ_ALIVE;

	__super::Tick(fTimeDelta);

	if (m_pEffectCom->isAnimEffect())
		m_pEffectCom->Play_Animation(fTimeDelta, m_lastFrameCalled, m_fFrameRate);

	m_pEffectCom->UV_Move(fTimeDelta);

	// 무한루프 옵션이 안켜져있다면
	if (!m_bUseLoop)
	{
		// 애니메이션이 있으면
		if (m_pEffectCom->isAnimEffect() && m_fDuration == 0.f)
		{
			if (m_lastFrameCalled)
				Stop();
		}
		// 애니메이션이 없거나 있더라도 duration이 존재하면 duration기준으로 꺼지도록
		else
		{
			m_fDurationCheck += fTimeDelta;

			if (m_fDurationCheck >= m_fDuration)
				Stop();
		}
	}

	m_SocketMatrix = socket;

	return OBJ_ALIVE;
}

void Client::CMeshEffect::Late_Tick(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	__super::Late_Tick(fTimeDelta);

	m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_BLEND, shared_from_this());
	m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_DISTORT, shared_from_this());

	if (m_pGameInstance.lock()->isDarkMode())
	{
		m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_EXCEPTDARK, shared_from_this());
	}

}

HRESULT Client::CMeshEffect::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (m_pEffectCom->isAnimEffect())
	{
		if (m_bUsePrimeColor)
		{
			if (FAILED(m_pEffectCom->Bind_SRV("g_MaskTexture", m_pMaskTex)))
				return E_FAIL;

			for (_uint i = 0; i < m_pEffectCom->Get_MeshCount(); ++i)
			{
				if (FAILED(m_pEffectCom->Bind_BoneMatrices("g_BoneMatrices", i)))
					return E_FAIL;

				m_pEffectCom->Render(i, 3);
			}
		}
		else
		{
			for (_uint i = 0; i < m_pEffectCom->Get_MeshCount(); ++i)
			{
				// 텍스처 바인딩
				if (FAILED(m_pEffectCom->Bind_SRV("g_DiffuseTexture", i, MaterialTexType_DIFFUSE)))
					return E_FAIL;
				if (FAILED(m_pEffectCom->Bind_BoneMatrices("g_BoneMatrices", i)))
					return E_FAIL;

				m_pEffectCom->Render(i, 1);
			}
		}
	}
	else
	{
		if (m_bUsePrimeColor)
		{
			if (FAILED(m_pEffectCom->Bind_SRV("g_MaskTexture", m_pMaskTex)))
				return E_FAIL;

			for (_uint i = 0; i < m_pEffectCom->Get_MeshCount(); ++i)
			{
				m_pEffectCom->Render(i, 2);
			}
		}
		else
		{
			for (_uint i = 0; i < m_pEffectCom->Get_MeshCount(); ++i)
			{
				// 텍스처 바인딩
				if (FAILED(m_pEffectCom->Bind_SRV("g_DiffuseTexture", i, MaterialTexType_DIFFUSE)))
					return E_FAIL;

				m_pEffectCom->Render(i, 0);
			}
		}
	}

	return S_OK;
}

HRESULT Client::CMeshEffect::Render_Distortion()
{
	if (FAILED(Bind_DistortionResurces()))
		return E_FAIL;

	if (m_pEffectCom->isAnimEffect())
	{
		for (_uint i = 0; i < m_pEffectCom->Get_MeshCount(); ++i)
		{
			// 텍스처 바인딩
			if (FAILED(m_pEffectCom->Bind_SRV("g_DiffuseTexture", i, MaterialTexType_DIFFUSE)))
				return E_FAIL;
			if (FAILED(m_pEffectCom->Bind_BoneMatrices("g_BoneMatrices", i)))
				return E_FAIL;
			m_pEffectCom->Render(i, 4);
		}
	}

	return S_OK;
}

HRESULT Client::CMeshEffect::Render_ExceptDark()
{
	Matrix WorldMatrix;
	if (m_pOwner.lock() != nullptr)
		WorldMatrix = m_pTransformCom->Get_WorldMatrix() * m_pOwner.lock()->Get_Transform()->Get_WorldMatrix() * m_SocketMatrix;
	else
		WorldMatrix = m_pTransformCom->Get_WorldMatrix() * m_SocketMatrix;

	Matrix ViewMatrix = m_pGameInstance.lock()->Get_ViewMatrix();
	Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();

	Vector2 vUVMove = m_pEffectCom->Get_UVMove();

	if (FAILED(m_pEffectCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectCom->Bind_RawValue("g_UVMove", &vUVMove, sizeof(Vector2))))
		return E_FAIL;
	if (FAILED(m_pEffectCom->Bind_RawValue("g_fPrimaryColor", &m_vPrimeColor, sizeof(Vector4))))
		return E_FAIL;

	if (m_pEffectCom->isAnimEffect())
	{
		for (_uint i = 0; i < m_pEffectCom->Get_MeshCount(); ++i)
		{
			// 텍스처 바인딩
			if (FAILED(m_pEffectCom->Bind_SRV("g_DiffuseTexture", i, MaterialTexType_DIFFUSE)))
				return E_FAIL;
			if (FAILED(m_pEffectCom->Bind_BoneMatrices("g_BoneMatrices", i)))
				return E_FAIL;
			m_pEffectCom->Render(i, 6);
		}
	}
	else
	{
		if (FAILED(m_pEffectCom->Bind_SRV("g_MaskTexture", m_pMaskTex)))
			return E_FAIL;

		for (_uint i = 0; i < m_pEffectCom->Get_MeshCount(); ++i)
		{
			m_pEffectCom->Render(i, 7);
		}
	}

	return S_OK;
}

void Client::CMeshEffect::PlayOnce()
{
	__super::PlayOnce();

	m_bUseLoop = false;
	m_fDurationCheck = 0.f;
}

void Client::CMeshEffect::PlayLoop()
{
	__super::PlayLoop();

	m_bUseLoop = true;
}

void Client::CMeshEffect::Stop()
{
	__super::Stop();

	if(m_pEffectCom->isAnimEffect())
		m_pEffectCom->Set_KeyFrame(0);
	m_fDurationCheck = 0.f;
}

HRESULT Client::CMeshEffect::Add_Component(LEVEL eLevel, const wstring& FXPrefabPath)
{
	ifstream istream(FXPrefabPath);

	if (istream.fail())
		return E_FAIL;

	_int iKeyLen;
	wstring OwnerName;
	wstring EffectName;
	Vector2 uvSpeed;
	_float fDuration;
	_bool usePrimaryColor;
	wstring maskTexKey;
	Vector4 primeColor;
	Vector3 vScale;
	Vector3 vRotation;

	istream.read((_char*)&iKeyLen, sizeof(iKeyLen));
	OwnerName.resize(iKeyLen);
	istream.read((_char*)OwnerName.c_str(), sizeof(_tchar) * iKeyLen);
	
	istream.read((_char*)&iKeyLen, sizeof(iKeyLen));
	EffectName.resize(iKeyLen);
	istream.read((_char*)EffectName.c_str(), sizeof(_tchar) * iKeyLen);

	istream.read((_char*)&uvSpeed, sizeof(uvSpeed));
	istream.read((_char*)&fDuration, sizeof(fDuration));

	istream.read((_char*)&usePrimaryColor, sizeof(usePrimaryColor));
	istream.read((_char*)&iKeyLen, sizeof(iKeyLen));
	maskTexKey.resize(iKeyLen);
	istream.read((_char*)maskTexKey.c_str(), sizeof(_tchar) * iKeyLen);
	istream.read((_char*)&primeColor, sizeof(primeColor));
	istream.read((_char*)&vScale, sizeof(vScale));
	istream.read((_char*)&vRotation, sizeof(vRotation));

	auto FXProp = m_pGameInstance.lock()->Get_AnimEffect(eLevel, OwnerName, EffectName);
	if (nullptr == FXProp)
		return E_FAIL;

	m_pEffectCom = CEffectMeshCom::Create(m_pDevice, m_pContext, FXProp);
	if (nullptr == m_pEffectCom)
		return E_FAIL;

	m_pEffectCom->Set_UVMoveSpeed(uvSpeed);
	
	if (maskTexKey != L"")
	{
		m_pMaskTex = m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, maskTexKey);
		if (nullptr == m_pMaskTex)
		{
			MSG_BOX("Error : MeshEffect::Add_Component");
			return E_FAIL;
		}
	}

	m_fDuration = fDuration;
	m_bUsePrimeColor = usePrimaryColor;
	m_vPrimeColor = primeColor;

	m_pTransformCom->Set_Scaling(vScale.x, vScale.y, vScale.z);
	m_pTransformCom->Rotation(vRotation);
	m_pTransformCom->Turn(Vector4(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	// 쉐이더 세팅
	if (FAILED(m_pEffectCom->Setting_Shader(m_pGameInstance.lock()->Get_Shader(L"Shader_VtxEffect"))))
		return E_FAIL;

	m_pNoiseTex = m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, L"Distortion");

	return S_OK;
}

HRESULT Client::CMeshEffect::Bind_ShaderResources()
{
	Matrix WorldMatrix;
	if(m_pOwner.lock() != nullptr)
		WorldMatrix = m_pTransformCom->Get_WorldMatrix() * m_pOwner.lock()->Get_Transform()->Get_WorldMatrix() * m_SocketMatrix;
	else
		WorldMatrix = m_pTransformCom->Get_WorldMatrix() * m_SocketMatrix;

	Matrix ViewMatrix = m_pGameInstance.lock()->Get_ViewMatrix();
	Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();

	Vector2 vUVMove = m_pEffectCom->Get_UVMove();

	if (FAILED(m_pEffectCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectCom->Bind_RawValue("g_UVMove", &vUVMove, sizeof(Vector2))))
		return E_FAIL;
	if (FAILED(m_pEffectCom->Bind_RawValue("g_fPrimaryColor", &m_vPrimeColor, sizeof(Vector4))))
		return E_FAIL;

	if (m_pEffectCom->isAnimEffect()&& !m_bUsePrimeColor)
	{
		_float fFrameRate = m_fFrameRate;
		if (FAILED(m_pEffectCom->Bind_RawValue("g_fDurationRate", &fFrameRate, sizeof(_float))))
			return E_FAIL;
	}
	else
	{
		_float fDurationRate = m_fDurationCheck / m_fDuration;
		if (FAILED(m_pEffectCom->Bind_RawValue("g_fDurationRate", &fDurationRate, sizeof(_float))))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT Client::CMeshEffect::Bind_DistortionResurces()
{
	Matrix WorldMatrix;
	if (m_pOwner.lock() != nullptr)
		WorldMatrix = m_pTransformCom->Get_WorldMatrix() * m_pOwner.lock()->Get_Transform()->Get_WorldMatrix() * m_SocketMatrix;
	else
		WorldMatrix = m_pTransformCom->Get_WorldMatrix() * m_SocketMatrix;

	Matrix ViewMatrix = m_pGameInstance.lock()->Get_ViewMatrix();
	Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();

	Vector2 vUVMove = m_pEffectCom->Get_UVMove();

	if (FAILED(m_pEffectCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectCom->Bind_RawValue("g_UVMove", &vUVMove, sizeof(Vector2))))
		return E_FAIL;

	if (FAILED(m_pEffectCom->Bind_SRV("g_NoiseTexture", m_pNoiseTex)))
		return E_FAIL;

	_float fDistortPower = 1.f - m_fFrameRate;
	if (FAILED(m_pEffectCom->Bind_RawValue("g_fDistortionPower", &fDistortPower, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CMeshEffect> Client::CMeshEffect::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& InitDesc)
{
	shared_ptr<CMeshEffect> pInstance = make_shared<CMeshEffect>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(InitDesc)))
	{
		MSG_BOX("Failed to Created : CMeshEffect");
		pInstance.reset();
	}

	return pInstance;
}
