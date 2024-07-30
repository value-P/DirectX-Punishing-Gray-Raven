#include "pch.h"
#include "DistortionPointRect.h"

Client::CDistortionPointRect::CDistortionPointRect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CFX(pDevice, pContext)
{
}

HRESULT Client::CDistortionPointRect::Initialize(const INIT_DESC& InitDesc)
{
	if(FAILED(__super::Initialize(Vector3(0.f, 0.f, 0.f))))
		return E_FAIL;

	m_fDuration = InitDesc.fDuration;

	m_strMaskTexKey = InitDesc.strMaskTexKey;
	m_strNoiseTexKey = InitDesc.strNoiseTexKey;

	m_vStartUvSpeed = InitDesc.vStartUvSpeed;
	m_vEndUvSpeed = InitDesc.vEndUvSpeed;

	m_vStartScale = InitDesc.vStartScale;
	m_vEndScale = InitDesc.vEndScale;

	//m_vStartRotationSpeed = InitDesc.vStartRotationSpeed;
	//m_vEndRotationSpeed = InitDesc.vEndRotationSpeed;

	m_fStartDistortionPower = InitDesc.fStartDistortionPower;
	m_fEndDistortionPower = InitDesc.fEndDistortionPower;
	m_fCurrentDistortionPower = m_fStartDistortionPower;

	if (FAILED(Add_Component(InitDesc.eCreatLevel)))
		return E_FAIL;

	return S_OK;
}

void Client::CDistortionPointRect::Priority_Tick(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	__super::Priority_Tick(fTimeDelta);
}

_int Client::CDistortionPointRect::Tick(_float fTimeDelta)
{
	if (!m_bActivate)
		return OBJ_ALIVE;

	__super::Tick(fTimeDelta);


	// 무한루프 옵션이 안켜져있다면
	if (!m_bUseLoop)
	{
		m_fDurationCheck += fTimeDelta;

		if (m_fDurationCheck >= m_fDuration)
		{
			Stop();
			return OBJ_ALIVE;
		}
	}
	else
	{
		if (m_fDurationCheck >= m_fDuration)
			m_fDuration = 0.f;
	}

	m_fFrameRate = m_fDurationCheck / m_fDuration;

	// uv이동
	m_vCurrentUvMoveSpeed = Vector2::Lerp(m_vStartUvSpeed, m_vEndUvSpeed, m_fFrameRate);
	m_vUvMoveAcc += m_vCurrentUvMoveSpeed;

	if (m_vUvMoveAcc.x > 1.f)
		m_vUvMoveAcc.x - 1.f;
	else if (m_vUvMoveAcc.x < 0.f)
		m_vUvMoveAcc.x + 1.f;

	if (m_vUvMoveAcc.y > 1.f)
		m_vUvMoveAcc.y - 1.f;
	else if (m_vUvMoveAcc.y < 0.f)
		m_vUvMoveAcc.y + 1.f;

	// 스케일
	m_vCurrentScale = Vector2::Lerp(m_vStartScale, m_vEndScale, m_fFrameRate);
	m_pTransformCom->Set_Scaling(m_vCurrentScale.x, m_vCurrentScale.y, 1.f);

	// 회전
	//m_vCurrentRoationSpeed = CMathUtils::lerp(m_vStartRotationSpeed, m_vEndRotationSpeed, m_fFrameRate);
	
	// 디스토션파워
	//m_fCurrentDistortionPower = CMathUtils::lerp(m_fCurrentDistortionPower, m_fEndDistortionPower, 0.01f);
	m_fCurrentDistortionPower = CMathUtils::lerp(m_fStartDistortionPower, m_fEndDistortionPower, m_fFrameRate + 0.05f);

	return OBJ_ALIVE;
}

_int Client::CDistortionPointRect::Tick(_float fTimeDelta, const Matrix& socket)
{
	if (!m_bActivate)
		return OBJ_ALIVE;

	Tick(fTimeDelta);

	m_SocketMatrix = socket;

	return OBJ_ALIVE;
}

void Client::CDistortionPointRect::Late_Tick(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	__super::Late_Tick(fTimeDelta);

	m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_DISTORT, shared_from_this());
}

HRESULT Client::CDistortionPointRect::Render_Distortion()
{
	if (FAILED(Bind_DistortionResources()))
		return E_FAIL;

	m_pPointCom->Render(3);

	return S_OK;
}

void Client::CDistortionPointRect::PlayOnce()
{
	__super::PlayOnce();

	m_bUseLoop = false;
	m_fDurationCheck = 0.f;
}

void Client::CDistortionPointRect::PlayLoop()
{
	__super::PlayLoop();
	m_bUseLoop = true;
}

void Client::CDistortionPointRect::Stop()
{
	__super::Stop();
	m_fDurationCheck = 0.f;
	m_fCurrentDistortionPower = m_fStartDistortionPower;
}

HRESULT Client::CDistortionPointRect::Add_Component(LEVEL eLevel)
{
	m_pPointCom = CPointCom::Create(m_pDevice, m_pContext);
	if (nullptr == m_pPointCom)
		return E_FAIL;

	if(FAILED(m_pPointCom->Setting_Shader(m_pGameInstance.lock()->Get_Shader(L"Shader_VtxPoint"))))
		return E_FAIL;
	if(FAILED(m_pPointCom->Setting_Texture(SpriteTexType::TEX_MASK, m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, m_strMaskTexKey))))
		return E_FAIL;
	if(FAILED(m_pPointCom->Setting_Texture(SpriteTexType::TEX_NOISE, m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, m_strNoiseTexKey))))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::CDistortionPointRect::Bind_DistortionResources()
{
	Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix() * m_SocketMatrix;
	Matrix ViewMatrix = m_pGameInstance.lock()->Get_ViewMatrix();
	Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();

	Vector4 vCampos = XMVectorSetW(m_pGameInstance.lock()->Get_CamPos(),1.f);

	if (FAILED(m_pPointCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pPointCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pPointCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pPointCom->Bind_RawValue("g_vUVMove", &m_vUvMoveAcc, sizeof(Vector2))))
		return E_FAIL;
	if (FAILED(m_pPointCom->Bind_RawValue("g_fDistortPower", &m_fCurrentDistortionPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pPointCom->Bind_RawValue("g_vCamPosition", &vCampos, sizeof(Vector4))))
		return E_FAIL;
	if (FAILED(m_pPointCom->Bind_SRV("g_Texture", SpriteTexType::TEX_MASK,0)))
		return E_FAIL;
	if (FAILED(m_pPointCom->Bind_SRV("g_DistortionTexture", SpriteTexType::TEX_NOISE,0)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CDistortionPointRect> Client::CDistortionPointRect::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& InitDesc)
{
	shared_ptr<CDistortionPointRect> pInstance = make_shared<CDistortionPointRect>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(InitDesc)))
	{
		MSG_BOX("Failed to Created : CDistortionPointRect");
		pInstance.reset();
	}

	return pInstance;
}
