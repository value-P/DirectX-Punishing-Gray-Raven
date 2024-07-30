#pragma once
#include "ImguiBase.h"
#include "Tool_ParticleObj.h"

namespace Engine
{
	class CTexture;
}

namespace Tool
{
	class CToolMain;
	class CMeshFX;
	class CSampleUI;

    class CEffectTool : public CImguiBase
    {
	public:
		CEffectTool(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CEffectTool() = default;

	public:
		virtual HRESULT Initialize(shared_ptr<CToolMain> pMainTool);
		virtual void Tick() override;
		virtual void Render() override;

	public:
		void InitEffectEventData(const string& strModelname);
		void ClearEffectEventData();

	private:
		void ParticleTab();
		void MeshEffectTab();
		void ParticlePrefabView();
		void MeshEffectPrefabView();
		void AnimEventView();

		HRESULT SavePariclePrefab();
		HRESULT SaveMeshEffectPrefab();

		void Add_ParticlePrefabKey(const string& PrefabName);
		void Add_MeshFXPrefabKey(const string& prefabName);

		void Read_ParticlePrefab(const string& strRootFile);
		void Read_MeshFXPrefab(const string& strRootFile);

		void Parse_EffectEventData();

	public:
		weak_ptr<CToolMain> m_pToolMain;
		weak_ptr<CSampleUI> m_pSampleUI;
		shared_ptr<CTool_ParticleObj> m_pParticleObj = nullptr;

		ImGuiWindowFlags m_eWindowFlag = 0;

	public:
		_int m_iNumInstance = 30;

		const _char* m_ParitcleMove[2] = { "Drop", "Spread" };
		_int m_iCurrentMove = 0;

		unordered_map<wstring, shared_ptr<CTexture>>* m_pTextureMap = nullptr;

		// Paritlce
		CPointParticle::INSTANCE_DESC m_tParicleData = {};
		CParticleSystem::PARTICLE_OPTION m_eParitcleTickOption = CParticleSystem::Drop;
		_float m_fSpeedMinLimit = 0.f;
		_float m_fLifeTimeMinLimit = 0.f;

		char m_strParticleName[MAX_PATH] = "";

		vector<const _char*> m_pTextureKeyList;
		shared_ptr<CTexture> m_pMyTexture = nullptr;
		_int m_iCurTex = 0;

		// MeshEffect
		map<string, vector<const _char*>> m_EffectMeshKeyList;
		_int m_iCurrentEffectIndex = 0;
		string m_currentEffectOwner = "";

		shared_ptr<CMeshFX> m_pEffectMesh = nullptr;
		_bool m_bCanCreate = false;
		Vector2 m_vUVSpeed = {};
		_float m_fDuration = 0.f;
		_bool m_bUsePrimaryColor = false; // 원색을 사용할 것인지
		wstring m_strMaskTextureKey = L""; // 마스크 텍스처 키
		Vector4 m_vPrimColor = {}; // 사용할 색
		Vector3 m_vScale = {1.f,1.f,1.f};
		Vector3 m_vRotation = {0.f,0.f,0.f};
		
		// Model Anim Event
		_int m_iCurrentKeyFrame = 0;
		_float m_fAnimSpeed = 1.f;
		FX_EVENT_DATA m_tData = {};
		list<string> m_ModelUseParticlePrefabNames;

		// 애니메이션 이벤트 저장용
		Json::Value AnimEvents;

		map<string, map<_int, vector<FX_EVENT_DATA>>> EventDatas; // 애니메이션 : 키프레임 : 생성할 이펙트들
		_int m_iEventIndex = 0;

		// Paritcle Prefab
		vector<string> m_ParticlePrefabs;
		vector<const _char*> m_szParticlePrefabs;
		_int m_iCurrentParticlePrefab = 0;
		map<string, shared_ptr<CTool_ParticleObj>> m_particleSamples;

		// MeshEffect Prefab
		vector<string> m_MeshEffectPrefabs;
		vector<const _char*> m_szMeshEffectPrefabs;
		_int m_iCurrentMeshFXPrefab = 0;
		map<string, shared_ptr<CMeshFX>> m_meshSamples;

	public:
		static shared_ptr<CEffectTool> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<CToolMain> pMainTool);
	};
}


