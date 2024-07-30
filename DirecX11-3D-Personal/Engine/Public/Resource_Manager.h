#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CShader;
	class CTexture;
	class CRectProp;
	class CCubeProp;
	class CTerrainProp;
	class CAnimModelProp;
	class CNonAnimModelProp;
	class CNavMeshProp;
	class CPointProp;
	class CAnimEffectProp;
	class CNonAnimEffectProp;

	class CResource_Manager final
	{
	public:
		enum class ResourceType{ Shader, Texture, Buffer, Sound, ResourceEnd};
		
	public:
		CResource_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CResource_Manager() = default;

	public:
		HRESULT Initialize(_uint iNumLevels);
		HRESULT ClearResource(_uint iLevelIndex);

	public:
		HRESULT Load_Multi_Textures_FromFile(const wstring& strRootFile, _uint iLevelIndex);
		HRESULT Load_Single_Textures_FromFile(const wstring& strRootFile, _uint iLevelIndex);
		HRESULT Load_TerrainWithHeightMap_FromFile(const wstring& strRootFile, _uint iLevelIndex);
		HRESULT Load_Model_FromFile(const wstring& strRootFile, _uint iLevelIndex, MeshType eMeshType, const Matrix& matPivot);
		HRESULT Load_NavMesh_FromFile(const wstring& strRootFile, _uint iLevelIndex);
		HRESULT Load_AnimEffect_FromFile(const wstring& strRootFile, _uint iLevelIndex, const Matrix& matPivot);

		HRESULT Add_Shader(const wstring& strShaderKey, const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);

		shared_ptr<CTexture> Get_Texture(_uint iLevelIndex, const wstring& strTextureKey);
		shared_ptr<CShader> Get_Shader(const wstring& strShaderKey);
		shared_ptr<CAnimModelProp> Get_AnimModel(_uint iLevelIndex,const wstring& strModelKey);
		shared_ptr<CNonAnimModelProp> Get_NonAnimModel(_uint iLevelIndex, const wstring& strModelKey);
		shared_ptr<CNavMeshProp> Get_NavMesh(_uint iLevelIndex, const wstring& strNavMeshKey);
		shared_ptr<Json::Value> Get_AnimEventData(const wstring& strModelKey);
		shared_ptr<Json::Value> Get_EffectEventData(const wstring& strModelKey);
		shared_ptr<CAnimEffectProp> Get_AnimEffect(_uint iLevelIndex,const wstring& strOwnerKey, const wstring& strEffectKey);

		vector<string>* Get_PropertyKeyList(_uint iLevelIndex, PropertyType eType);
		map<string, vector<string>>* Get_AnimEffectPropKeyList(_uint iLevelIndex);
		unordered_map<wstring, shared_ptr<CTexture>>* Get_TextureMap(_uint iLevelIndex);

		// 정점 프로퍼티 가져오는 함수
		shared_ptr<CRectProp> Get_RectProp();
		shared_ptr<CCubeProp> Get_CubeProp();
		shared_ptr<CPointProp> Get_PointProp();
		shared_ptr<CTerrainProp> Get_TerrainProp(_uint iLevelIndex, const wstring& strTerrainKey);

	private:
		void Add_AnimData(const shared_ptr<CAnimModelProp> pModelProp, const wstring& strModelName);
		void Add_EffectData(const shared_ptr<CAnimModelProp> pModelProp, const wstring& strModelName);

	private:
		ComPtr<ID3D11Device> m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

		_uint	m_iLevelNum = 0;
		
		using ShaderMap			= unordered_map<wstring, shared_ptr<CShader>>;
		using TextureMap		= unordered_map<wstring, shared_ptr<CTexture>>;
		using TerrainMap		= unordered_map<wstring, shared_ptr<CTerrainProp>>;
		using AnimModelMap		= unordered_map<wstring, shared_ptr<CAnimModelProp>>;
		using NonAnimModelMap	= unordered_map<wstring, shared_ptr<CNonAnimModelProp>>;
		using NavMeshMap		= unordered_map<wstring, shared_ptr<CNavMeshProp>>;
		using AnimEffectlMap	= unordered_map<wstring, shared_ptr<CAnimEffectProp>>;

		ShaderMap m_ShaderMap;
		vector<TextureMap> m_vecTextureMap;
		vector<TerrainMap> m_vecTerrainMap;
		vector<AnimModelMap> m_vecAnimModelMap;
		vector<NonAnimModelMap> m_vecNonAnimModelMap;
		vector<NavMeshMap> m_vecNavMeshMap;
		vector<map<wstring,AnimEffectlMap>> m_vecAnimEffectMap;

		// 애니메이션 이벤트 데이터정보를 가지는 JsonValue
		unordered_map<wstring, Json::Value> jsonEventDatas;
		unordered_map<wstring, Json::Value> jsonEffectEventDatas;
		
		// 키값 모음집
		vector<vector<string>> m_vecTextureKey;
		vector<vector<string>> m_vecTerrainKey;
		vector<vector<string>> m_vecAnimModelKey;
		vector<vector<string>> m_vecNonAnimModelKey;
		vector<vector<string>> m_vecNavMeshKey;
		vector<map<string,vector<string>>> m_vecAnimEffectKey;

		// 사각형 버퍼는 단 하나만 있어도 돌려쓰기 가능하므로
		shared_ptr<CRectProp> m_pRectProp = nullptr;
		shared_ptr<CCubeProp> m_pCubeProp = nullptr;
		shared_ptr<CPointProp> m_pPointProp = nullptr;

	public:
		static shared_ptr<CResource_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumLevels);
	};
}


