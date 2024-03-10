#include "pch.h"
#include "Loader.h"
#include "GameInstance.h"

#include "TerrainProp.h"
#include "RectProp.h"

CLoader::CLoader(LEVEL eNextLevel, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_eNextLevel(eNextLevel), m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(CGameInstance::GetInstance())
{
}

Client::CLoader::~CLoader()
{
	m_thread.join();
}

HRESULT CLoader::Initialize()
{
	m_thread = thread(&CLoader::Loading_Selector, this);
    
	return S_OK;
}

HRESULT CLoader::Loading_Selector()
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
	case LEVEL_BOSS:
		hr = Loading_For_BossLevel();
		break;
	case LEVEL_BOSS2:
		hr = Loading_For_BossLevel2();
		break;
	}
	
	return hr;
}

#ifdef _DEBUG
void CLoader::Output_LoadingText()
{
	SetWindowText(g_hWnd, m_strLoadingText.c_str());
}
#endif

HRESULT CLoader::Loading_For_LogoLevel()
{
	m_strLoadingText = TEXT("사운드를(을) 로딩 중 입니다.");
	m_pGameInstance.lock()->LoadSound(L"../Bin/Resources/Sounds/InGame");

	m_strLoadingText = TEXT("텍스처를(을) 로딩 중 입니다.");
	if (FAILED(m_pGameInstance.lock()->Load_Multi_Textures_FromFile(L"../Bin/Resources/Textures/MultiTex", LEVEL_STATIC)))
		return E_FAIL;

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");
	if (FAILED(m_pGameInstance.lock()->Add_Shader(L"Shader_Cell", L"../Bin/ShaderFiles/Shader_Cell.hlsl", VTXPOS::Elements, VTXPOS::iNumElements)))
		return E_FAIL;

	m_strLoadingText = TEXT("버퍼를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("오브젝트를(을) 로딩 중 입니다.");

	LIGHT_DESC LightDesc = {};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = Vector4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = Vector4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = Vector4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = Vector4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vPosition = Vector4(2.7f,40.f,-13.f, 1.f);
	LightDesc.vAt = Vector4(-52.f, 0.f, -13.5f, 1.f);

	if (FAILED(m_pGameInstance.lock()->Add_Light(LightDesc))) return E_FAIL;
	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlayLevel()
{
	m_strLoadingText = TEXT("텍스처를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");
	if (FAILED(m_pGameInstance.lock()->Add_Shader(L"Shader_VtxNorTex", L"../Bin/ShaderFiles/Shader_VtxNorTex.hlsl", VTXNORTEX::Elements, VTXNORTEX::iNumElements)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Shader(L"Shader_VtxMesh", L"../Bin/ShaderFiles/Shader_VtxMesh.hlsl", VTXMESH::Elements, VTXMESH::iNumElements)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Shader(L"Shader_VtxAnimMesh", L"../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl", VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Shader(L"Shader_VtxCube", L"../Bin/ShaderFiles/Shader_VtxCube.hlsl", VTXCUBE::Elements, VTXCUBE::iNumElements)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Shader(L"Shader_VtxEffect", L"../Bin/ShaderFiles/Shader_VtxEffect.hlsl", VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Shader(L"Shader_VtxPointInstance", L"../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl", VTX_POINT_INSTANCE::Elements, VTX_POINT_INSTANCE::iNumElements)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Shader(L"Shader_VtxSuperDodgeEffect", L"../Bin/ShaderFiles/Shader_VtxSuperDodgeEffect.hlsl", VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)))
		return E_FAIL;

	m_strLoadingText = TEXT("버퍼를(을) 로딩 중 입니다.");
	Matrix matPivot = XMMatrixRotationY(XMConvertToRadians(180.f));
	if(FAILED(m_pGameInstance.lock()->Load_Model_FromFile(L"../Bin/Resources/Models/AnimModels/Playable", LEVEL_STATIC, MeshType::ANIM, matPivot)))
		return E_FAIL;
	if(FAILED(m_pGameInstance.lock()->Load_Model_FromFile(L"../Bin/Resources/Models/AnimModels/Level1", LEVEL_GAMEPLAY, MeshType::ANIM, matPivot)))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Load_Model_FromFile(L"../Bin/Resources/Models/NonAnimModels/Playable", LEVEL_STATIC, MeshType::NON_ANIM, matPivot)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Load_Model_FromFile(L"../Bin/Resources/Models/NonAnimModels/Level1", LEVEL_GAMEPLAY, MeshType::NON_ANIM, matPivot)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Load_NavMesh_FromFile(L"../Bin/Resources/NavMeshFile/Level1", LEVEL_GAMEPLAY)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Load_AnimEffect_FromFile(L"../Bin/Resources/Effects/Vera", LEVEL_STATIC, XMMatrixIdentity())))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Load_AnimEffect_FromFile(L"../Bin/Resources/Effects", LEVEL_GAMEPLAY, XMMatrixIdentity())))
		return E_FAIL;

	m_strLoadingText = TEXT("오브젝트를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT Client::CLoader::Loading_For_BossLevel()
{
	m_strLoadingText = TEXT("텍스처를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("버퍼를(을) 로딩 중 입니다.");

	Matrix matPivot = XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance.lock()->Load_Model_FromFile(L"../Bin/Resources/Models/AnimModels/LevelBoss", LEVEL_BOSS, MeshType::ANIM, matPivot)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Load_Model_FromFile(L"../Bin/Resources/Models/NonAnimModels/LevelBoss", LEVEL_BOSS, MeshType::NON_ANIM, matPivot)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Load_NavMesh_FromFile(L"../Bin/Resources/NavMeshFile/LevelBoss", LEVEL_BOSS)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Load_AnimEffect_FromFile(L"../Bin/Resources/Effects/Pushin", LEVEL_BOSS, matPivot)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Load_AnimEffect_FromFile(L"../Bin/Resources/Effects", LEVEL_BOSS, XMMatrixIdentity())))
		return E_FAIL;

	m_strLoadingText = TEXT("오브젝트를(을) 로딩 중 입니다.");

	LIGHT_DESC LightDesc = {};

	m_pGameInstance.lock()->Clear_Light();

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = Vector4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = Vector4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = Vector4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = Vector4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vPosition = Vector4(-112.f,45.f,-142.f, 1.f);
	LightDesc.vAt = Vector4(-136.f,26.f,106.f, 1.f);

	if (FAILED(m_pGameInstance.lock()->Add_Light(LightDesc))) return E_FAIL;

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT Client::CLoader::Loading_For_BossLevel2()
{
	m_strLoadingText = TEXT("텍스처를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("버퍼를(을) 로딩 중 입니다.");

	Matrix matPivot = XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance.lock()->Load_Model_FromFile(L"../Bin/Resources/Models/AnimModels/LevelBoss2", LEVEL_BOSS2, MeshType::ANIM, matPivot)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Load_Model_FromFile(L"../Bin/Resources/Models/NonAnimModels/LevelBoss2", LEVEL_BOSS2, MeshType::NON_ANIM, matPivot)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Load_NavMesh_FromFile(L"../Bin/Resources/NavMeshFile/LevelBoss2", LEVEL_BOSS2)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Load_AnimEffect_FromFile(L"../Bin/Resources/Effects/StartKnight", LEVEL_BOSS2, matPivot)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Load_AnimEffect_FromFile(L"../Bin/Resources/Effects", LEVEL_BOSS2, XMMatrixIdentity())))
		return E_FAIL;

	m_strLoadingText = TEXT("오브젝트를(을) 로딩 중 입니다.");

	LIGHT_DESC LightDesc = {};

	m_pGameInstance.lock()->Clear_Light();

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = Vector4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = Vector4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = Vector4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = Vector4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vPosition = Vector4(28.f,40.f,-2.7f, 1.f);
	LightDesc.vAt = Vector4(-30.f,12.2f,-34.f, 1.f);

	if (FAILED(m_pGameInstance.lock()->Add_Light(LightDesc))) return E_FAIL;

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

shared_ptr<CLoader> CLoader::Create(LEVEL eNextLevel, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CLoader> pInstance = make_shared<CLoader>(eNextLevel, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLoader");
		pInstance.reset();
	}

	return pInstance;
}
