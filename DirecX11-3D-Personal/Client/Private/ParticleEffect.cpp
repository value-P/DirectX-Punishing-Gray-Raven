#include "pch.h"
#include "ParticleEffect.h"

Client::CParticleEffect::CParticleEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CFX(pDevice, pContext)
{
}

HRESULT Client::CParticleEffect::Initialize(const INIT_DESC& InitDesc)
{
	if (FAILED(__super::Initialize(InitDesc.vInitPos)))
		return E_FAIL;

	if (FAILED(Add_Component(InitDesc.eLevel, InitDesc.FXPrefabPath)))
		return E_FAIL;

	return S_OK;
}

void Client::CParticleEffect::Priority_Tick(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	__super::Priority_Tick(fTimeDelta);
}

_int Client::CParticleEffect::Tick(_float fTimeDelta)
{
	if (!m_bActivate)
		return OBJ_ALIVE;

	__super::Tick(fTimeDelta);

	m_pParticleSystem->Tick(fTimeDelta);

	if (m_pParticleSystem->isStopped())
		m_bActivate = false;

	return OBJ_ALIVE;
}

_int Client::CParticleEffect::Tick(_float fTimeDelta, const Matrix& socket)
{
	if (!m_bActivate)
		return OBJ_ALIVE;

	__super::Tick(fTimeDelta);

	m_pParticleSystem->Tick(fTimeDelta);

	if (m_pParticleSystem->isStopped())
		m_bActivate = false;

	m_SocketMatrix = socket;

	return OBJ_ALIVE;
}

void Client::CParticleEffect::Late_Tick(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	__super::Late_Tick(fTimeDelta);

	m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, shared_from_this());

	if (m_pGameInstance.lock()->isDarkMode())
	{
		m_pGameInstance.lock()->Add_RenderGroup(CRenderer::RENDER_EXCEPTDARK, shared_from_this());
	}
}

HRESULT Client::CParticleEffect::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	auto moveOption = m_pParticleSystem->Get_ParticleOption();

	if (moveOption == CParticleSystem::Drop)
		m_pParticleSystem->Render(0);
	else if (moveOption == CParticleSystem::Spread)
		m_pParticleSystem->Render(1);

	return S_OK;
}

HRESULT Client::CParticleEffect::Render_ExceptDark()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	auto moveOption = m_pParticleSystem->Get_ParticleOption();

	if (moveOption == CParticleSystem::Drop)
		m_pParticleSystem->Render(2);
	else if (moveOption == CParticleSystem::Spread)
		m_pParticleSystem->Render(3);

	return S_OK;
}

void Client::CParticleEffect::PlayOnce()
{
	__super::PlayOnce();

	m_pParticleSystem->PlayOnce();
}

void Client::CParticleEffect::PlayLoop()
{
	__super::PlayLoop();

	m_pParticleSystem->PlayLoop();
}

void Client::CParticleEffect::Stop()
{
	__super::Stop();

	m_pParticleSystem->Stop();
}

HRESULT Client::CParticleEffect::Add_Component(LEVEL eLevel, const wstring& FXPrefabPath)
{
	ifstream istream(FXPrefabPath);

	if (istream.fail())
		return E_FAIL;

	_int iNumInstnace = 0;
	CPointParticle::INSTANCE_DESC particleData = {};
	CParticleSystem::PARTICLE_OPTION eParticleMoveOption = {};
	_int iKeyLen = 0;
	wstring strTexKey = L"";

	istream.read((_char*)&iNumInstnace, sizeof(iNumInstnace));
	istream.read((_char*)&particleData, sizeof(particleData));
	istream.read((_char*)&eParticleMoveOption, sizeof(eParticleMoveOption));
	istream.read((_char*)&iKeyLen, sizeof(iKeyLen));
	strTexKey.resize(iKeyLen);
	istream.read((_char*)strTexKey.c_str(), sizeof(_tchar) * iKeyLen);

	m_pParticleSystem = CParticleSystem::Create(m_pDevice, m_pContext, iNumInstnace, particleData);
	if (nullptr == m_pParticleSystem)
		return E_FAIL;

	m_pParticleSystem->Change_Option(eParticleMoveOption);

	auto texture = m_pGameInstance.lock()->Get_Texture(LEVEL_STATIC, strTexKey);
	if (nullptr == texture)
		return E_FAIL;

	m_pParticleSystem->Setting_Texture(SpriteTexType::TEX_DIFFUSE, texture);

	// ½¦ÀÌ´õ ¼¼ÆÃ
	if (FAILED(m_pParticleSystem->Setting_Shader(m_pGameInstance.lock()->Get_Shader(L"Shader_VtxPointInstance"))))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::CParticleEffect::Bind_ShaderResources()
{
	Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix() * m_SocketMatrix;
	Matrix ViewMatrix = m_pGameInstance.lock()->Get_ViewMatrix();
	Matrix ProjMatrix = m_pGameInstance.lock()->Get_ProjMatrix();
	Vector4 vCamPos = XMVectorSetW(m_pGameInstance.lock()->Get_CamPos(), 1.f);

	if (FAILED(m_pParticleSystem->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pParticleSystem->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pParticleSystem->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pParticleSystem->Bind_RawValue("g_vCamPosition", &vCamPos, sizeof(Vector4))))
		return E_FAIL;
	if (FAILED(m_pParticleSystem->Bind_SRV("g_Texture", SpriteTexType::TEX_DIFFUSE, 0)))
		return E_FAIL;

	Vector4 vPivotPos = XMVectorSetW(m_pParticleSystem->Get_PivotPos(), 1.f);
	if (FAILED(m_pParticleSystem->Bind_RawValue("g_vPivotPos", &vPivotPos, sizeof(Vector4))))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CParticleEffect> Client::CParticleEffect::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& InitDesc)
{
	shared_ptr<CParticleEffect> pInstance = make_shared<CParticleEffect>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(InitDesc)))
	{
		MSG_BOX("Failed to Created : CParticleEffect");
		pInstance.reset();
	}

	return pInstance;
}
