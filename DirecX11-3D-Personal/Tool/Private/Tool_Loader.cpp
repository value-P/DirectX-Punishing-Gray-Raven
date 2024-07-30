#include "pch.h"
#include "Tool_Loader.h"
#include "GameInstance.h"

#include "TerrainProp.h"
#include "RectProp.h"

CTool_Loader::CTool_Loader(LEVEL eNextLevel, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_eNextLevel(eNextLevel), m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(CGameInstance::GetInstance())
{
}

Tool::CTool_Loader::~CTool_Loader()
{
	m_thread.join();
}

HRESULT CTool_Loader::Initialize()
{
	m_thread = thread(&CTool_Loader::Loading_Selector, this);
    
	return S_OK;
}

HRESULT CTool_Loader::Loading_Selector()
{
	lock_guard<mutex> guard(m_Mutex);

	if(FAILED(CoInitialize(nullptr))) return E_FAIL;

	HRESULT hr = E_FAIL;

	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		hr = Loading_For_LogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlayLevel();
		break;
	}
	
	return hr;
}

#ifdef _DEBUG
void CTool_Loader::Output_LoadingText()
{
	SetWindowText(g_hWnd, m_strLoadingText.c_str());
}
#endif

HRESULT CTool_Loader::Loading_For_LogoLevel()
{
	m_strLoadingText = TEXT("텍스처를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("버퍼를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("오브젝트를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CTool_Loader::Loading_For_GamePlayLevel()
{
	m_strLoadingText = TEXT("텍스처를(을) 로딩 중 입니다.");
	if (FAILED(m_pGameInstance.lock()->Load_Multi_Textures_FromFile(L"../Bin/Resources/Textures/MultiTex", LEVEL_STATIC)))
		return E_FAIL;

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");
	if (FAILED(m_pGameInstance.lock()->Add_Shader(L"Shader_VtxNorTex", L"../Bin/ShaderFiles/Shader_VtxNorTex.hlsl", VTXNORTEX::Elements, VTXNORTEX::iNumElements)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Shader(L"Shader_VtxMesh", L"../Bin/ShaderFiles/Shader_VtxMesh.hlsl", VTXMESH::Elements, VTXMESH::iNumElements)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Shader(L"Shader_VtxAnimMesh", L"../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl", VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Shader(L"Shader_VtxPointInstance", L"../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl", VTX_POINT_INSTANCE::Elements, VTX_POINT_INSTANCE::iNumElements)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Shader(L"Shader_VtxRectInstance", L"../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl", VTX_RECT_INSTANCE::Elements, VTX_RECT_INSTANCE::iNumElements)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Shader(L"Shader_VtxEffect", L"../Bin/ShaderFiles/Shader_VtxEffect.hlsl", VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)))
		return E_FAIL;

	m_strLoadingText = TEXT("버퍼를(을) 로딩 중 입니다.");
	Matrix matPivot = XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance.lock()->Load_Model_FromFile(L"../Bin/Resources/NonAnimModels", LEVEL_GAMEPLAY, MeshType::NON_ANIM, matPivot)))
		return E_FAIL;

	matPivot.Translation(Vector3(0.f, 0.8f, 0.f));
	if (FAILED(m_pGameInstance.lock()->Load_Model_FromFile(L"../Bin/Resources/AnimModels", LEVEL_GAMEPLAY, MeshType::ANIM, matPivot)))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Load_AnimEffect_FromFile(L"../Bin/Resources/Effects", LEVEL_GAMEPLAY, XMMatrixIdentity())))
		return E_FAIL;

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

shared_ptr<CTool_Loader> CTool_Loader::Create(LEVEL eNextLevel, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CTool_Loader> pInstance = make_shared<CTool_Loader>(eNextLevel, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CTool_Loader");
		pInstance.reset();
	}

	return pInstance;
}
