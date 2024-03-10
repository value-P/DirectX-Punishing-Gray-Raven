#pragma once
#include "MathUtils.h"
#include "StringUtils.h"

#include "Renderer.h"
#include "Transform.h"
#include "SpriteCom.h"
#include "SkeletalMeshCom.h"
#include "StaticMeshCom.h"
#include "TerrainCom.h"
#include "BoxCom.h"
#include "PointCom.h"
#include "PipeLine.h"
#include "Collider.h"
#include "Navigation.h"
#include "ParticleSystem.h"
#include "EffectMeshCom.h"
#include "UIBase.h"
#include "TrailCom.h"
#pragma region ForCollider
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#pragma endregion

namespace Engine
{
	class CGraphic_Device;
	class CLevel_Manager;
	class CObject_Manager;
	class CTimer_Manager;
	class CResource_Manager;
	class CInput_Device;
	class CImgui_Manager;
	class CPicking_Manager;
	class CLight_Manager;
	class CNavMeshManager;
	class CCollisionManager;
	class CRandomManager;
	class CTarget_Manager;
	class CUIManager;
	class CCameraManager;
	class CFontManager;
	class CSoundManager;

	class CLevel;
	class CCamera;
	class CGameObject;
	class CImguiBase;
	class CNavMeshProp;
	class CLayer;

	class ENGINE_DLL CGameInstance final
	{
		DECLARE_SINGLETON(CGameInstance)

	public:
		HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, ComPtr<ID3D11Device>& pDeviceOut, ComPtr<ID3D11DeviceContext>& pDeviceContextOut, vector<_uint>& layer_Collision_Matrix, ImGuiContext* ctx = nullptr);
		void Tick_Engine(_float fTimeDelta);

		HRESULT Begin_Draw(const Vector4& vClearColor);
		HRESULT Draw();
		HRESULT End_Draw();

		void Clear(_uint iLevelIndex);
		void Clear_Object(_uint iLevelIndex);

	public: // Level_Manager
		HRESULT Open_Level(_uint iLevelIndex, shared_ptr<CLevel> pLevel);

	public: // Obect_Manager
		shared_ptr<CLayer> Get_Layer(const wstring& strLayerTag);
		HRESULT Add_Object(_uint iLevelIndex, const wstring& strLayerTag, shared_ptr<CGameObject> pGameObject, const _float* pTimeScale);
		_bool RayPicking(_uint iLevelIndex, const Ray& ray, _float& fOutDist);
		void Clear_Layer(_uint iLevelIndex, const wstring& strLayerTag);

	public: // Timer
		_float Compute_TimeDelta(const wstring& strTimerTag);
		HRESULT Add_Timer(const wstring& strTimerTag);

	public: // Renderer
		HRESULT Add_RenderGroup(CRenderer::RENDERGROUP eRenderGroup, shared_ptr<CGameObject> pGameObject);
		void DarkWindowActivate(_float fDarkDuration);
		_bool isDarkMode();
		void ScreenBlurActivate(_float fScreenBlurDuration, _float fBlurPower = 45.f);
		void UseFog(_bool bUseFog);

#ifdef _DEBUG
	public:
		HRESULT Add_DebugComponent(shared_ptr<CComponent> pComponent);
#endif // _DEBUG

	public: // Resource_Manager
		HRESULT ClearResource(_uint iLevelIndex);

		HRESULT Load_Multi_Textures_FromFile(const wstring& strRootFile, _uint iLevelIndex);
		HRESULT Load_Single_Textures_FromFile(const wstring& strRootFile, _uint iLevelIndex);
		HRESULT Load_TerrainWithHeightMap_FromFile(const wstring& strRootFile, _uint iLevelIndex);
		HRESULT Load_Model_FromFile(const wstring& strRootFile, _uint iLevelIndex, MeshType eMeshType, const Matrix& matPivot);
		HRESULT Load_NavMesh_FromFile(const wstring& strRootFile, _uint iLevelIndex);
		HRESULT Load_AnimEffect_FromFile(const wstring& strRootFile, _uint iLevelIndex, const Matrix& matPivot);

		HRESULT Add_Shader(const wstring& strShaderKey, const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);

		shared_ptr<CTexture> Get_Texture(_uint iLevelIndex, const wstring& strTextureKey);
		shared_ptr<CShader> Get_Shader(const wstring& strShaderKey);

		shared_ptr<CRectProp> Get_RectProp();
		shared_ptr<CCubeProp> Get_CubeProp();
		shared_ptr<CPointProp> Get_PointProp();
		shared_ptr<CTerrainProp> Get_TerrainProp(_uint iLevelIndex, const wstring& strTerrainKey);
		shared_ptr<CAnimModelProp> Get_AnimModel(_uint iLevelIndex, const wstring& strModelKey);
		shared_ptr<CNonAnimModelProp> Get_NonAnimModel(_uint iLevelIndex, const wstring& strModelKey);
		shared_ptr<CNavMeshProp> Get_NavMesh(_uint iLevelIndex, const wstring& strNavMeshKey);
		shared_ptr<Json::Value> Get_AnimEventData(const wstring& strModelKey);
		shared_ptr<Json::Value> Get_EffectEventData(const wstring& strModelKey);
		shared_ptr<CAnimEffectProp> Get_AnimEffect(_uint iLevelIndex, const wstring& strOwnerKey, const wstring& strEffectKey);

		vector<string>* Get_PropertyKeyList(_uint iLevelIndex, PropertyType eType);
		map<string, vector<string>>* Get_AnimEffectPropKeyList(_uint iLevelIndex);
		unordered_map<wstring, shared_ptr<CTexture>>* Get_TextureMap(_uint iLevelIndex);

	public: // PipeLine
		void Set_ViewMatrix(const Matrix& viewMatrix)	{ m_pPipeLine->Set_ViewMatrix(viewMatrix); }
		void Set_ProjMatrix(const Matrix& projMatrix)	{ m_pPipeLine->Set_ProjMatrix(projMatrix); }
		void Set_CamFar(const _float& fFar)				{ m_pPipeLine->Set_CamFar(fFar); }
		Matrix Get_ViewMatrix()			{ return m_pPipeLine->Get_ViewMatrix(); }
		Matrix Get_ProjMatrix()			{ return m_pPipeLine->Get_ProjMatrix(); }
		Matrix Get_ViewMatrixInverse()	{ return m_pPipeLine->Get_ViewMatrixInverse(); }
		Matrix Get_ProjMatrixInverse()	{ return m_pPipeLine->Get_ProjMatrixInverse(); }
		Vector3 Get_CamPos()			{ return m_pPipeLine->Get_CamPos(); }
		_float Get_CamFar()				{ return m_pPipeLine->Get_CamFar(); }

	public: // Imgui_Manager
		HRESULT Add_Imgui(const wstring& strImguiTag, shared_ptr<CImguiBase> pImguiObject);

	public: // Input_Device
		_bool GetKeyDown(_ubyte byKeyID);
		_bool GetKey(_ubyte byKeyID);
		_bool GetKeyUp(_ubyte byKeyID);

		_bool GetMouseButtonDown(MOUSEKEYSTATE eMouseState);
		_bool GetMouseButton(MOUSEKEYSTATE eMouseState);
		_bool GetMouseButtonUp(MOUSEKEYSTATE eMouseState);
		_long  Get_DIMouseMove(MOUSEMOVESTATE eMouseState);

	public: // Picking_Manager
		Ray RayAtViewSpace();
		Ray RayAtWorldSpace();

	public: // Light_Manager
		const shared_ptr<LIGHT_DESC> Get_LightDesc(_uint iIndex);
		const Matrix* Get_LightViewMatrix(_uint iIndex);
		HRESULT Add_Light(const LIGHT_DESC& LightDesc);
		HRESULT Render_Light(shared_ptr<CShader> pShader);
		void Clear_Light();

	public: // NavMeshManager
		_float Get_HeightByIndex(_int currentIndex, const Vector3& vPosition);;
		_bool isMove(const Vector3& vPosition, _int& iCurrentIndex);
		_int GetIndexByPosition(_uint iLevelIndex, const Vector3& vPosition);
		HRESULT Set_NavMesh(_uint iLevelIndex, shared_ptr<CNavMeshProp> pNavMeshProp, shared_ptr<CGameObject> pCombinedObject);

	public: // CollisionManager
		void Add_Collider(_uint iLevelIndex, shared_ptr<CCollider> pCollider);

	public: // RandomManager
		_int Rand_Num(_int iMin, _int iMax);
		_float Rand_Num(_float fMin, _float fMax);

	public: // Target_Manager
		HRESULT Add_RenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const Vector4& vClearColor);
		HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag);
		HRESULT Begin_MRT(const wstring& strMRTTag, ComPtr<ID3D11DepthStencilView> pDSView = nullptr);
		HRESULT End_MRT();
		HRESULT Bind_SRV(const wstring& strTargetTag, const shared_ptr<CShader> pShader, const _char* pConstantName);

	public: // UI Manager
		HRESULT Add_UI(_uint iLevelIndex, shared_ptr<CUIBase> pUI);
		void EnableUIs();
		void DisableUIs();

	public: // Camera Manager
		HRESULT Add_Camera(_uint iLevelIndex, const wstring& strCameraTag, shared_ptr<CCamera> pCamera);
		HRESULT Set_MainCamera(_uint iLevelIndex, const wstring& strCameraTag);

	public: // Font Manager
		HRESULT Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);
		HRESULT Render_Font(const wstring& strFontTag, const wstring& strText, const Vector2& vPosition, Vector4 vColor = Vector4(1.f, 1.f, 1.f, 1.f), _float fRotation = 0.f, _float fScale = 1.f);

	public: // Sound Manager
		CHANNELID Play_Sound(const wstring& strSoundKey, _float fVolume);
		_bool isPlaying(CHANNELID eChannel);
		void Play_BGM(const wstring& strSoundKey, _float fVolume);
		void Stop_Sound(CHANNELID eID);
		void Stop_All();
		void Set_ChannelVolume(CHANNELID eID, _float fVolume);

		void LoadSound(const wstring& strRootFile);


#ifdef _DEBUG
		HRESULT Init_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
		HRESULT Render_MRT(const wstring& strMRTTag, const shared_ptr<CShader> pShader);
#endif // _DEBUG

	private:
		shared_ptr<CGraphic_Device>		m_pGraphic_Device	= nullptr;
		shared_ptr<CLevel_Manager>		m_pLevel_Manager	= nullptr;
		shared_ptr<CObject_Manager>		m_pObject_Manager	= nullptr;
		shared_ptr<CRenderer>			m_pRenderer			= nullptr;
		shared_ptr<CTimer_Manager>		m_pTimer_Manager	= nullptr;
		shared_ptr<CResource_Manager>	m_pResource_Manager = nullptr;
		shared_ptr<CPipeLine>			m_pPipeLine			= nullptr;
		shared_ptr<CImgui_Manager>		m_pImgui_Manager	= nullptr;
		shared_ptr<CInput_Device>		m_pInput_Device		= nullptr;
		shared_ptr<CPicking_Manager>	m_pPickingManager	= nullptr;
		shared_ptr<CLight_Manager>		m_pLightManager		= nullptr;
		shared_ptr<CNavMeshManager>		m_pNavMeshManager	= nullptr;
		shared_ptr<CCollisionManager>	m_pCollisionManager = nullptr;
		shared_ptr<CRandomManager>		m_pRandomManager	= nullptr;
		shared_ptr<CTarget_Manager>		m_pTargetManager	= nullptr;
		shared_ptr<CUIManager>			m_pUIManager		= nullptr;
		shared_ptr<CCameraManager>		m_pCameraManager	= nullptr;
		shared_ptr<CFontManager>		m_pFontManager		= nullptr;
		shared_ptr<CSoundManager>		m_pSoundManager		= nullptr;

	private:
		ComPtr<ID3D11Device>			m_pDevice	= nullptr;
		ComPtr<ID3D11DeviceContext>		m_pContext	= nullptr;

	private:
		_uint m_iCurrentLevel = 0;
	};
}
