#include "GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Resource_Manager.h"
#include "Input_Device.h"
#include "Imgui_Manager.h"
#include "Picking_Manager.h"
#include "Light_Manager.h"
#include "NavMeshManager.h"
#include "CollisionManager.h"
#include "RandomManager.h"
#include "Layer.h"
#include "Target_Manager.h"
#include "UIManager.h"
#include "CameraManager.h"
#include "FontManager.h"
#include "SoundManager.h"

#include "GameObject.h"
#include "ImguiBase.h"

IMPLEMENT_SINGLETON(CGameInstance)

HRESULT Engine::CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, ComPtr<ID3D11Device>& pDeviceOut, 
	ComPtr<ID3D11DeviceContext>& pDeviceContextOut, vector<_uint>& layer_Collision_Matrix, ImGuiContext* ctx)
{
	// 그래픽 디바이스 생성 및 초기화
	m_pGraphic_Device = CGraphic_Device::Create(GraphicDesc, m_pDevice, m_pContext);
	if (m_pGraphic_Device == nullptr) return E_FAIL;

	// Level Manager 생성 및 초기화
	m_pLevel_Manager = CLevel_Manager::Create();
	if (m_pLevel_Manager == nullptr) return E_FAIL;

	// Object Manager 생성 및 초기화
	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (m_pObject_Manager == nullptr) return E_FAIL;

	// Resource Manager 생성
	m_pResource_Manager = CResource_Manager::Create(m_pDevice, m_pContext, iNumLevels);
	if (m_pResource_Manager == nullptr) return E_FAIL;

	// Target Manager 생성 및 초기화
	m_pTargetManager = CTarget_Manager::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTargetManager) return E_FAIL;

	// Renderer 생성 및 초기화
	CRenderer::INIT_DESC rendererDesc;
	rendererDesc.iUpScalingDepthStencilSizeX = GraphicDesc.iUpScalingSizeX;
	rendererDesc.iUpScalingDepthStencilSizeY = GraphicDesc.iUpScalingSizeY;
	m_pRenderer = CRenderer::Create(m_pDevice, m_pContext, rendererDesc);
	if (m_pRenderer == nullptr) return E_FAIL;

	// Timer Manager 생성
	m_pTimer_Manager = CTimer_Manager::Create();
	if (m_pTimer_Manager == nullptr) return E_FAIL;

	// PipeLine Manager 생성
	m_pPipeLine = CPipeLine::Create();
	if (m_pPipeLine == nullptr) return E_FAIL;

	// Input Device 생성 및 초기화
	m_pInput_Device = CInput_Device::Create(hInst, GraphicDesc.hWnd);
	if (m_pInput_Device == nullptr) return E_FAIL;

	// Imgui_Manager 생성 및 초기화
	if (nullptr != ctx)
	{
		m_pImgui_Manager = CImgui_Manager::Create(m_pDevice, m_pContext, GraphicDesc.hWnd, ctx);
		if (m_pImgui_Manager == nullptr) return E_FAIL;
	}
	
	// Picking Manager 생성 및 초기화
	m_pPickingManager = CPicking_Manager::Create(GraphicDesc.hWnd, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY);
	if (m_pPickingManager == nullptr) return E_FAIL;
	
	// Light Manager 생성 및 초기화
	m_pLightManager = CLight_Manager::Create(m_pDevice, m_pContext);
	if (m_pLightManager == nullptr) return E_FAIL;

	// NavMesh Manager 생성 및 초기화
	m_pNavMeshManager = CNavMeshManager::Create(iNumLevels);
	if (m_pNavMeshManager == nullptr) return E_FAIL;

	// Collision Manager 생성 및 초기화
	m_pCollisionManager = CCollisionManager::Create(iNumLevels, layer_Collision_Matrix);
	if (m_pCollisionManager == nullptr) return E_FAIL;

	// Random Manager 생성 및 초기화
	m_pRandomManager = CRandomManager::Create();
	if (m_pRandomManager == nullptr) return E_FAIL;

	// UI Manager 생성 및 초기화
	m_pUIManager = CUIManager::Create(iNumLevels, GraphicDesc.hWnd);
	if (m_pUIManager == nullptr) return E_FAIL;

	// Camera Manager 생성 및 초기화
	m_pCameraManager = CCameraManager::Create(iNumLevels);
	if (m_pCameraManager == nullptr) return E_FAIL;

	// Font Manager 생성 및 초기화
	m_pFontManager = CFontManager::Create(m_pDevice, m_pContext);
	if (m_pFontManager == nullptr) return E_FAIL;

	// Sound Manager 생성 및 초기화
	m_pSoundManager = CSoundManager::Create();
	if (m_pSoundManager == nullptr) return E_FAIL;

	pDeviceOut = m_pDevice;
	pDeviceContextOut = m_pContext;

	return S_OK;
}

void Engine::CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (m_pLevel_Manager == nullptr||
		m_pObject_Manager == nullptr)
		return;

	m_pInput_Device->Tick();

	m_pLevel_Manager->Tick(fTimeDelta);

	m_pObject_Manager->Priority_Tick(fTimeDelta);

	m_pCameraManager->Priority_Tick(fTimeDelta);

	m_pNavMeshManager->Tick(m_iCurrentLevel);

	m_pPipeLine->Tick();

	m_pObject_Manager->Tick(fTimeDelta);

	m_pCameraManager->Tick(fTimeDelta);

	m_pCollisionManager->Tick(m_iCurrentLevel);
	
	m_pObject_Manager->Late_Tick(fTimeDelta);

	m_pRenderer->Tick(fTimeDelta);

	m_pUIManager->Tick(fTimeDelta);

	if(nullptr != m_pImgui_Manager)
		m_pImgui_Manager->Tick();
}

// Level Manager
HRESULT Engine::CGameInstance::Begin_Draw(const Vector4& vClearColor)
{
	if (m_pGraphic_Device == nullptr) return E_FAIL;

	m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
	m_pGraphic_Device->Clear_DepthStencil_View();

	return S_OK;
}

HRESULT Engine::CGameInstance::Draw()
{
	//그리기
	m_pRenderer->Render();
	m_pLevel_Manager->Render();

	if (nullptr != m_pImgui_Manager)
		m_pImgui_Manager->Render();

#ifdef _DEBUG
	//m_pNavMeshManager->Render(m_iCurrentLevel);
#endif

	return S_OK;
}

HRESULT Engine::CGameInstance::End_Draw()
{
	return m_pGraphic_Device->Present();
}

void Engine::CGameInstance::Clear(_uint iLevelIndex)
{
	if (m_pObject_Manager == nullptr || m_pResource_Manager == nullptr) return;

	m_pObject_Manager->Clear(iLevelIndex);
	m_pResource_Manager->ClearResource(iLevelIndex);
	m_pNavMeshManager->Clear(iLevelIndex);
	m_pCollisionManager->Clear(iLevelIndex);
	m_pUIManager->Clear(iLevelIndex);
	m_pRenderer->Clear();
}

void Engine::CGameInstance::Clear_Object(_uint iLevelIndex)
{
	if (m_pObject_Manager == nullptr) return;

	m_pObject_Manager->Clear(iLevelIndex);
}

HRESULT Engine::CGameInstance::Open_Level(_uint iLevelIndex, shared_ptr<CLevel> pLevel)
{
	if (m_pLevel_Manager == nullptr)
		return E_FAIL;

	m_iCurrentLevel = iLevelIndex;

	m_pCameraManager->Change_Scene(iLevelIndex);

	return m_pLevel_Manager->Open_Level(iLevelIndex, pLevel);
}

// Object Manager
shared_ptr<CLayer> Engine::CGameInstance::Get_Layer(const wstring& strLayerTag)
{
	if (m_pObject_Manager == nullptr)
		return nullptr;

	return m_pObject_Manager->Get_Layer(m_iCurrentLevel, strLayerTag);
}

HRESULT Engine::CGameInstance::Add_Object(_uint iLevelIndex, const wstring& strLayerTag, shared_ptr<CGameObject> pGameObject, const _float* pTimeScale)
{
	if (m_pObject_Manager == nullptr)
		return E_FAIL;

	return m_pObject_Manager->Add_Object(iLevelIndex, strLayerTag, pGameObject, pTimeScale);
}

_bool Engine::CGameInstance::RayPicking(_uint iLevelIndex, const Ray& ray, _float& fOutDist)
{
	if (m_pObject_Manager == nullptr)
		return false;

	return m_pObject_Manager->RayPicking(iLevelIndex,ray,fOutDist);
}

void Engine::CGameInstance::Clear_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (m_pObject_Manager == nullptr)
		return;

	return m_pObject_Manager->Clear_Layer(iLevelIndex, strLayerTag);
}

_float Engine::CGameInstance::Compute_TimeDelta(const wstring& strTimerTag)
{
	if (m_pTimer_Manager == nullptr) return 0.f;

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

HRESULT Engine::CGameInstance::Add_Timer(const wstring& strTimerTag)
{
	if (m_pTimer_Manager == nullptr) return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

HRESULT Engine::CGameInstance::Add_RenderGroup(CRenderer::RENDERGROUP eRenderGroup, shared_ptr<CGameObject> pGameObject)
{
	if (eRenderGroup >= CRenderer::RENDER_END || pGameObject == nullptr)
		return E_FAIL;

	return m_pRenderer->Add_RenderGroup(eRenderGroup,pGameObject);
}

void Engine::CGameInstance::DarkWindowActivate(_float fDarkDuration)
{
	m_pRenderer->DarkWindowActivate(fDarkDuration);
}

_bool Engine::CGameInstance::isDarkMode()
{
	return m_pRenderer->isDarkMode();
}

void Engine::CGameInstance::ScreenBlurActivate(_float fScreenBlurDuration, _float fBlurPower)
{
	m_pRenderer->ScreenBlurActivate(fScreenBlurDuration, fBlurPower);
}

void Engine::CGameInstance::UseFog(_bool bUseFog)
{
	m_pRenderer->UseFog(bUseFog);
}

#ifdef _DEBUG
HRESULT Engine::CGameInstance::Add_DebugComponent(shared_ptr<CComponent> pComponent)
{
	return m_pRenderer->Add_DebugComponent(pComponent);
}
#endif // _DEBUG

// Resource_Manager
HRESULT Engine::CGameInstance::ClearResource(_uint iLevelIndex)
{
	if (nullptr == m_pResource_Manager) return E_FAIL;

	return m_pResource_Manager->ClearResource(iLevelIndex);
}

HRESULT Engine::CGameInstance::Load_Multi_Textures_FromFile(const wstring& strRootFile, _uint iLevelIndex)
{
	if (nullptr == m_pResource_Manager) return E_FAIL;

	return m_pResource_Manager->Load_Multi_Textures_FromFile(strRootFile, iLevelIndex);
}

HRESULT Engine::CGameInstance::Load_Single_Textures_FromFile(const wstring& strRootFile, _uint iLevelIndex)
{
	if (nullptr == m_pResource_Manager) return E_FAIL;

	return m_pResource_Manager->Load_Single_Textures_FromFile(strRootFile, iLevelIndex);
}

HRESULT Engine::CGameInstance::Load_TerrainWithHeightMap_FromFile(const wstring& strRootFile, _uint iLevelIndex)
{
	if (nullptr == m_pResource_Manager) return E_FAIL;

	return m_pResource_Manager->Load_TerrainWithHeightMap_FromFile(strRootFile, iLevelIndex);
}

HRESULT Engine::CGameInstance::Load_Model_FromFile(const wstring& strRootFile, _uint iLevelIndex, MeshType eMeshType, const Matrix& matPivot)
{
	if (nullptr == m_pResource_Manager) return E_FAIL;

	return m_pResource_Manager->Load_Model_FromFile(strRootFile, iLevelIndex, eMeshType, matPivot);
}

HRESULT Engine::CGameInstance::Load_NavMesh_FromFile(const wstring& strRootFile, _uint iLevelIndex)
{
	if (nullptr == m_pResource_Manager) return E_FAIL;

	return m_pResource_Manager->Load_NavMesh_FromFile(strRootFile, iLevelIndex);
}

HRESULT Engine::CGameInstance::Load_AnimEffect_FromFile(const wstring& strRootFile, _uint iLevelIndex, const Matrix& matPivot)
{
	if (nullptr == m_pResource_Manager) return E_FAIL;

	return m_pResource_Manager->Load_AnimEffect_FromFile(strRootFile, iLevelIndex, matPivot);
}

HRESULT Engine::CGameInstance::Add_Shader(const wstring& strShaderKey, const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	if (nullptr == m_pResource_Manager) return E_FAIL;

	return m_pResource_Manager->Add_Shader(strShaderKey, strShaderFilePath, pElements, iNumElements);
}

shared_ptr<CTexture> Engine::CGameInstance::Get_Texture(_uint iLevelIndex, const wstring& strTextureKey)
{
	if (nullptr == m_pResource_Manager) return nullptr;

	return m_pResource_Manager->Get_Texture(iLevelIndex, strTextureKey);
}

shared_ptr<CShader> Engine::CGameInstance::Get_Shader(const wstring& strShaderKey)
{
	if (nullptr == m_pResource_Manager) return nullptr;

	return m_pResource_Manager->Get_Shader(strShaderKey);
}

shared_ptr<CRectProp> Engine::CGameInstance::Get_RectProp()
{
	if (nullptr == m_pResource_Manager) return nullptr;

	return m_pResource_Manager->Get_RectProp();
}

shared_ptr<CCubeProp> Engine::CGameInstance::Get_CubeProp()
{
	if (nullptr == m_pResource_Manager) return nullptr;

	return m_pResource_Manager->Get_CubeProp();
}

shared_ptr<CPointProp> Engine::CGameInstance::Get_PointProp()
{
	if (nullptr == m_pResource_Manager) return nullptr;

	return m_pResource_Manager->Get_PointProp();
}

shared_ptr<CTerrainProp> Engine::CGameInstance::Get_TerrainProp(_uint iLevelIndex, const wstring& strTerrainKey)
{
	if (nullptr == m_pResource_Manager) return nullptr;

	return m_pResource_Manager->Get_TerrainProp(iLevelIndex, strTerrainKey);
}

shared_ptr<CAnimModelProp> Engine::CGameInstance::Get_AnimModel(_uint iLevelIndex, const wstring& strModelKey)
{
	if (nullptr == m_pResource_Manager) return nullptr;

	return m_pResource_Manager->Get_AnimModel(iLevelIndex, strModelKey);
}

shared_ptr<CNonAnimModelProp> Engine::CGameInstance::Get_NonAnimModel(_uint iLevelIndex, const wstring& strModelKey)
{
	if (nullptr == m_pResource_Manager) return nullptr;

	return m_pResource_Manager->Get_NonAnimModel(iLevelIndex, strModelKey);
}

shared_ptr<CNavMeshProp> Engine::CGameInstance::Get_NavMesh(_uint iLevelIndex, const wstring& strNavMeshKey)
{
	if (nullptr == m_pResource_Manager) return nullptr;

	return m_pResource_Manager->Get_NavMesh(iLevelIndex, strNavMeshKey);
}

shared_ptr<Json::Value> Engine::CGameInstance::Get_AnimEventData(const wstring& strModelKey)
{
	if (nullptr == m_pResource_Manager) return nullptr;

	return m_pResource_Manager->Get_AnimEventData(strModelKey);
}

shared_ptr<Json::Value> Engine::CGameInstance::Get_EffectEventData(const wstring& strModelKey)
{
	if (nullptr == m_pResource_Manager) return nullptr;

	return m_pResource_Manager->Get_EffectEventData(strModelKey);
}

shared_ptr<CAnimEffectProp> Engine::CGameInstance::Get_AnimEffect(_uint iLevelIndex, const wstring& strOwnerKey, const wstring& strEffectKey)
{
	if (nullptr == m_pResource_Manager) return nullptr;

	return m_pResource_Manager->Get_AnimEffect(iLevelIndex, strOwnerKey, strEffectKey);
}

vector<string>* Engine::CGameInstance::Get_PropertyKeyList(_uint iLevelIndex, PropertyType eType)
{
	if (nullptr == m_pResource_Manager) return nullptr;

	return m_pResource_Manager->Get_PropertyKeyList(iLevelIndex, eType);
}

map<string, vector<string>>* Engine::CGameInstance::Get_AnimEffectPropKeyList(_uint iLevelIndex)
{
	if (nullptr == m_pResource_Manager) return nullptr;

	return m_pResource_Manager->Get_AnimEffectPropKeyList(iLevelIndex);
}

unordered_map<wstring, shared_ptr<CTexture>>* Engine::CGameInstance::Get_TextureMap(_uint iLevelIndex)
{
	if (nullptr == m_pResource_Manager) return nullptr;

	return m_pResource_Manager->Get_TextureMap(iLevelIndex);
}

// Imgui_Manager
HRESULT Engine::CGameInstance::Add_Imgui(const wstring& strImguiTag, shared_ptr<CImguiBase> pImguiObject)
{
	if (nullptr == m_pImgui_Manager) return E_FAIL;

	return m_pImgui_Manager->Add_Imgui(strImguiTag, pImguiObject);
}

// Input_Device
_bool Engine::CGameInstance::GetKeyDown(_ubyte byKeyID)
{
	return m_pInput_Device->GetKeyDown(byKeyID);
}

_bool Engine::CGameInstance::GetKey(_ubyte byKeyID)
{
	return m_pInput_Device->GetKey(byKeyID);
}

_bool Engine::CGameInstance::GetKeyUp(_ubyte byKeyID)
{
	return m_pInput_Device->GetKeyUp(byKeyID);
}

_bool Engine::CGameInstance::GetMouseButtonDown(MOUSEKEYSTATE eMouseState)
{
	return m_pInput_Device->GetMouseButtonDown(eMouseState);
}

_bool Engine::CGameInstance::GetMouseButton(MOUSEKEYSTATE eMouseState)
{
	return m_pInput_Device->GetMouseButton(eMouseState);
}

_bool Engine::CGameInstance::GetMouseButtonUp(MOUSEKEYSTATE eMouseState)
{
	return m_pInput_Device->GetMouseButtonUp(eMouseState);
}

_long Engine::CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

// Picking_Manager
Ray Engine::CGameInstance::RayAtViewSpace()
{
	return m_pPickingManager->RayAtViewSpace();
}

Ray Engine::CGameInstance::RayAtWorldSpace()
{
	return m_pPickingManager->RayAtWorldSpace();
}

// Light_Manager
const shared_ptr<LIGHT_DESC> Engine::CGameInstance::Get_LightDesc(_uint iIndex)
{
	return m_pLightManager->Get_LightDesc(iIndex);
}

const Matrix* Engine::CGameInstance::Get_LightViewMatrix(_uint iIndex)
{
	return m_pLightManager->Get_LightViewMatrix(iIndex);
}

HRESULT Engine::CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLightManager->Add_Light(LightDesc);
}

HRESULT Engine::CGameInstance::Render_Light(shared_ptr<CShader> pShader)
{
	return m_pLightManager->Render_Light(pShader);
}

void Engine::CGameInstance::Clear_Light()
{
	m_pLightManager->Clear();
}

// NavMeshManager
_float Engine::CGameInstance::Get_HeightByIndex(_int currentIndex, const Vector3& vPosition)
{
	if (nullptr == m_pNavMeshManager)
		return false;

	return m_pNavMeshManager->Get_HeightByIndex(m_iCurrentLevel, currentIndex, vPosition);
}

_bool Engine::CGameInstance::isMove(const Vector3& vPosition, _int& iCurrentIndex)
{
	if (nullptr == m_pNavMeshManager)
		return false;

	return m_pNavMeshManager->isMove(m_iCurrentLevel,vPosition,iCurrentIndex);
}

_int Engine::CGameInstance::GetIndexByPosition(_uint iLevelIndex, const Vector3& vPosition)
{
	if (nullptr == m_pNavMeshManager)
		return -1;

	return m_pNavMeshManager->GetIndexByPosition(iLevelIndex, vPosition);
}

HRESULT Engine::CGameInstance::Set_NavMesh(_uint iLevelIndex, shared_ptr<CNavMeshProp> pNavMeshProp, shared_ptr<CGameObject> pCombinedObject)
{
	if (nullptr == m_pNavMeshManager)
		return E_FAIL;

	return m_pNavMeshManager->Set_NavMesh(iLevelIndex, pNavMeshProp, pCombinedObject);
}

// CollisionManager
void Engine::CGameInstance::Add_Collider(_uint iLevelIndex, shared_ptr<CCollider> pCollider)
{
	if (nullptr == m_pCollisionManager)
		return;

	m_pCollisionManager->AddCollider(iLevelIndex, pCollider);
}

// RandomManager
_int Engine::CGameInstance::Rand_Num(_int iMin, _int iMax)
{
	return m_pRandomManager->Rand_Num(iMin, iMax);
}

_float Engine::CGameInstance::Rand_Num(_float fMin, _float fMax)
{
	return m_pRandomManager->Rand_Num(fMin, fMax);
}

// TargetManager
HRESULT Engine::CGameInstance::Add_RenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const Vector4& vClearColor)
{
	return m_pTargetManager->Add_RenderTarget(strTargetTag, iSizeX,  iSizeY, ePixelFormat, vClearColor);
}

HRESULT Engine::CGameInstance::Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag)
{
	return m_pTargetManager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT Engine::CGameInstance::Begin_MRT(const wstring& strMRTTag, ComPtr<ID3D11DepthStencilView> pDSView)
{
	return m_pTargetManager->Begin_MRT(strMRTTag, pDSView);
}

HRESULT Engine::CGameInstance::End_MRT()
{
	return m_pTargetManager->End_MRT();
}

HRESULT Engine::CGameInstance::Bind_SRV(const wstring& strTargetTag, const shared_ptr<CShader> pShader, const _char* pConstantName)
{
	return m_pTargetManager->Bind_SRV(strTargetTag, pShader, pConstantName);
}

// UI Manager
HRESULT Engine::CGameInstance::Add_UI(_uint iLevelIndex, shared_ptr<CUIBase> pUI)
{
	return m_pUIManager->Add_UI(iLevelIndex, pUI);
}

void Engine::CGameInstance::EnableUIs()
{
	m_pUIManager->EnableUIs();
}

void Engine::CGameInstance::DisableUIs()
{
	m_pUIManager->DisableUIs();
}

// Camera Manager
HRESULT Engine::CGameInstance::Add_Camera(_uint iLevelIndex, const wstring& strCameraTag, shared_ptr<CCamera> pCamera)
{
	return m_pCameraManager->Add_Camera(iLevelIndex,strCameraTag,pCamera);
}

HRESULT Engine::CGameInstance::Set_MainCamera(_uint iLevelIndex, const wstring& strCameraTag)
{
	return m_pCameraManager->Set_MainCamera(iLevelIndex, strCameraTag);
}

// Font Manager
HRESULT Engine::CGameInstance::Add_Font(const wstring& strFontTag, const wstring& strFontFilePath)
{
	return m_pFontManager->Add_Font(strFontTag, strFontFilePath);
}

HRESULT Engine::CGameInstance::Render_Font(const wstring& strFontTag, const wstring& strText, const Vector2& vPosition, Vector4 vColor, _float fRotation, _float fScale)
{
	return m_pFontManager->Render(strFontTag, strText, vPosition, vColor, fRotation, fScale);
}

// Sound Manager
CHANNELID Engine::CGameInstance::Play_Sound(const wstring& strSoundKey, _float fVolume)
{
	return m_pSoundManager->Play_Sound(strSoundKey, fVolume);
}

_bool Engine::CGameInstance::isPlaying(CHANNELID eChannel)
{
	return m_pSoundManager->isPlaying(eChannel);
}

void Engine::CGameInstance::Play_BGM(const wstring& strSoundKey, _float fVolume)
{
	m_pSoundManager->Play_BGM(strSoundKey, fVolume);
}

void Engine::CGameInstance::Stop_Sound(CHANNELID eID)
{
	m_pSoundManager->Stop_Sound(eID);
}

void Engine::CGameInstance::Stop_All()
{
	m_pSoundManager->Stop_All();
}

void Engine::CGameInstance::Set_ChannelVolume(CHANNELID eID, _float fVolume)
{
	m_pSoundManager->Set_ChannelVolume(eID, fVolume);
}

void Engine::CGameInstance::LoadSound(const wstring& strRootFile)
{
	m_pSoundManager->LoadSound(strRootFile);
}

#ifdef _DEBUG
HRESULT Engine::CGameInstance::Init_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTargetManager->Init_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}

HRESULT Engine::CGameInstance::Render_MRT(const wstring& strMRTTag, const shared_ptr<CShader> pShader)
{
	return m_pTargetManager->Render_MRT(strMRTTag, pShader);
}
#endif // _DEBUG