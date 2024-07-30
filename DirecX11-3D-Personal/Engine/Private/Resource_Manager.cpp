#include "Resource_Manager.h"
#include "Texture.h"
#include "Shader.h"
#include "RectProp.h"
#include "CubeProp.h"
#include "TerrainProp.h"
#include "AnimModelProp.h"
#include "NonAnimModelProp.h"
#include "NavMeshProp.h"
#include "PointProp.h"
#include "AnimEffectProp.h"
#include "NonAnimEffectProp.h"

Engine::CResource_Manager::CResource_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice(pDevice),m_pContext(pContext)
{
}

HRESULT Engine::CResource_Manager::Initialize(_uint iNumLevels)
{
	m_iLevelNum = iNumLevels;

	m_vecTextureMap.resize(iNumLevels);
	m_vecTerrainMap.resize(iNumLevels);
	m_vecNonAnimModelMap.resize(iNumLevels);
	m_vecAnimModelMap.resize(iNumLevels);
	m_vecNavMeshMap.resize(iNumLevels);
	m_vecAnimEffectMap.resize(iNumLevels);

	m_vecTextureKey.resize(iNumLevels);
	m_vecTerrainKey.resize(iNumLevels);
	m_vecAnimModelKey.resize(iNumLevels);
	m_vecNonAnimModelKey.resize(iNumLevels);
	m_vecNavMeshKey.resize(iNumLevels);
	m_vecAnimEffectKey.resize(iNumLevels);

	return S_OK;
}

HRESULT Engine::CResource_Manager::ClearResource(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iLevelNum) return E_FAIL;

	m_vecTextureMap[iLevelIndex].clear();
	m_vecTerrainMap[iLevelIndex].clear();

	return S_OK;
}

HRESULT Engine::CResource_Manager::Load_Multi_Textures_FromFile(const wstring& strRootFile, _uint iLevelIndex)
{
	if (iLevelIndex >= m_iLevelNum) return E_FAIL;

	filesystem::path path(strRootFile);
	filesystem::directory_iterator iter(path);
	_bool startLoadTex = false;
	shared_ptr<CTexture> pTex = nullptr;

	while (iter != filesystem::end(iter))
	{
		// �Ϲ� ���� �� ���
		if (iter->is_regular_file())
		{
			if (!startLoadTex)
			{
				auto Tex = m_vecTextureMap[iLevelIndex].find(path.stem().wstring());
				if (Tex != m_vecTextureMap[iLevelIndex].end())
				{
					MSG_BOX("�̹� ��ϵ� �ؽ�ó ������Ƽ Ű�Դϴ�");
					return E_FAIL;
				}

				pTex = CTexture::Create(m_pDevice, m_pContext);

				startLoadTex = true;
			}

			if (FAILED(pTex->Add_Texture(iter->path().wstring())))
			{				
				MessageBox(NULL, iter->path().wstring().c_str(), L"Texture Error", MB_OK);
				return E_FAIL;
			}
		}
		// ���� �� ���
		else if (iter->is_directory())
		{
			Load_Multi_Textures_FromFile(iter->path().wstring(), iLevelIndex);
		}

		++iter;
	}

	if (startLoadTex && nullptr != pTex)
	{
		m_vecTextureMap[iLevelIndex].emplace(path.stem().wstring(), pTex);
		m_vecTextureKey[iLevelIndex].push_back(path.stem().string());
	}

	return S_OK;
}

HRESULT Engine::CResource_Manager::Load_Single_Textures_FromFile(const wstring& strRootFile, _uint iLevelIndex)
{
	if (iLevelIndex >= m_iLevelNum) return E_FAIL;

	filesystem::path path = strRootFile;
	filesystem::directory_iterator iter(path);

	while (iter != filesystem::end(iter))
	{
		// �Ϲ� ���� �� ���
		if (iter->is_regular_file())
		{
			auto Tex = m_vecTextureMap[iLevelIndex].find(iter->path().stem().wstring());
			if (Tex != m_vecTextureMap[iLevelIndex].end())
			{
				MSG_BOX("�̹� ��ϵ� �ؽ�ó ������Ƽ Ű�Դϴ�");
				return E_FAIL;
			}

			shared_ptr<CTexture> pTex = CTexture::Create(m_pDevice, m_pContext, iter->path().wstring());

			if (nullptr == pTex)
			{
				MessageBox(NULL, iter->path().wstring().c_str(), L"Texture Error", MB_OK);
				return E_FAIL;
			}

			m_vecTextureMap[iLevelIndex].emplace(iter->path().stem().wstring(), pTex);
			m_vecTextureKey[iLevelIndex].push_back(iter->path().stem().string());
		}
		// ���� �� ���
		else if (iter->is_directory())
		{
			Load_Single_Textures_FromFile(iter->path().wstring(), iLevelIndex);
		}

		++iter;
	}

	return S_OK;
}

HRESULT Engine::CResource_Manager::Load_TerrainWithHeightMap_FromFile(const wstring& strRootFile, _uint iLevelIndex)
{
	if (iLevelIndex >= m_iLevelNum) return E_FAIL;

	filesystem::path path = strRootFile;
	filesystem::directory_iterator iter(path);

	while (iter != filesystem::end(iter))
	{
		// �Ϲ� ���� �� ���
		if (iter->is_regular_file())
		{
			auto Terrain = m_vecTerrainMap[iLevelIndex].find(iter->path().stem().wstring());
			if (Terrain != m_vecTerrainMap[iLevelIndex].end()) 
			{
				MSG_BOX("�̹� ��ϵ� �ͷ��� ������Ƽ Ű�Դϴ�");
				return E_FAIL;
			}

			shared_ptr<CTerrainProp> pTerrain = CTerrainProp::Create(m_pDevice, m_pContext, iter->path().wstring());

			if (nullptr == pTerrain)
			{
				MessageBox(NULL, iter->path().wstring().c_str(), L"Terrain Error", MB_OK);
				return E_FAIL;
			}

			m_vecTerrainMap[iLevelIndex].emplace(iter->path().stem().wstring(), pTerrain);
			m_vecTerrainKey[iLevelIndex].push_back(iter->path().stem().string());
		}
		// ���� �� ���
		else if (iter->is_directory())
		{
			Load_Multi_Textures_FromFile(iter->path().wstring(), iLevelIndex);
		}

		++iter;
	}

	return S_OK;
}

HRESULT Engine::CResource_Manager::Load_Model_FromFile(const wstring& strRootFile, _uint iLevelIndex, MeshType eMeshType, const Matrix& matPivot)
{
	if (iLevelIndex >= m_iLevelNum) return E_FAIL;

	filesystem::path path(strRootFile);
	filesystem::directory_iterator iter(path);

	while (iter != filesystem::end(iter))
	{
		// fbx ���� �� ���
		_bool flag = eMeshType == MeshType::NON_ANIM ? iter->path().extension() == ".NonAnimFbx" : iter->path().extension() == ".AnimFbx";

		if (iter->is_regular_file() && flag)
		{
			if (eMeshType == MeshType::NON_ANIM)
			{
				auto meshs = m_vecNonAnimModelMap[iLevelIndex].find(iter->path().stem().wstring());
				if (meshs != m_vecNonAnimModelMap[iLevelIndex].end())
				{
					MSG_BOX("�̹� ��ϵ� �� ������Ƽ Ű�Դϴ�");
					return E_FAIL;
				}
			}
			else
			{
				auto meshs = m_vecAnimModelMap[iLevelIndex].find(iter->path().stem().wstring());
				if (meshs != m_vecAnimModelMap[iLevelIndex].end())
				{
					MSG_BOX("�̹� ��ϵ� �� ������Ƽ Ű�Դϴ�");
					return E_FAIL;
				}
			}

			if (eMeshType == MeshType::NON_ANIM)
			{
				shared_ptr<CNonAnimModelProp> pModel = CNonAnimModelProp::Create(m_pDevice, m_pContext, iter->path().wstring());

				m_vecNonAnimModelMap[iLevelIndex].emplace(iter->path().stem().wstring(), pModel);
				m_vecNonAnimModelKey[iLevelIndex].push_back(iter->path().stem().string());
			}
			else
			{
				shared_ptr<CAnimModelProp> pModel = CAnimModelProp::Create(m_pDevice, m_pContext, iter->path().wstring(), matPivot);

				m_vecAnimModelMap[iLevelIndex].emplace(iter->path().stem().wstring(), pModel);
				m_vecAnimModelKey[iLevelIndex].push_back(iter->path().stem().string());

				wstring strModelName = iter->path().stem().wstring();
				Add_AnimData(pModel, strModelName);
				Add_EffectData(pModel, strModelName);
			}


		}
		// ���� �� ���
		else if (iter->is_directory())
		{
			Load_Model_FromFile(iter->path().wstring(), iLevelIndex, eMeshType, matPivot);
		}

		++iter;
	}

	return S_OK;
}

HRESULT Engine::CResource_Manager::Load_NavMesh_FromFile(const wstring& strRootFile, _uint iLevelIndex)
{
	if (iLevelIndex >= m_iLevelNum) return E_FAIL;

	filesystem::path path(strRootFile);
	filesystem::directory_iterator iter(path);

	while (iter != filesystem::end(iter))
	{
		// NavMesh ���� �� ���
		if (iter->is_regular_file() && iter->path().extension() == ".NavMesh")
		{
			auto navMeshs = m_vecNavMeshMap[iLevelIndex].find(iter->path().stem().wstring());
			if (navMeshs != m_vecNavMeshMap[iLevelIndex].end())
			{
				MSG_BOX("�̹� ��ϵ� NavMesh ������Ƽ Ű�Դϴ�");
				return E_FAIL;
			}

			shared_ptr<CNavMeshProp> pModel = CNavMeshProp::Create(m_pDevice, m_pContext, iter->path().wstring());

			m_vecNavMeshMap[iLevelIndex].emplace(iter->path().stem().wstring(), pModel);
			m_vecNavMeshKey[iLevelIndex].push_back(iter->path().stem().string());

		}
		// ���� �� ���
		else if (iter->is_directory())
		{
			Load_NavMesh_FromFile(iter->path().wstring(), iLevelIndex);
		}

		++iter;
	}

	return S_OK;
}

HRESULT Engine::CResource_Manager::Load_AnimEffect_FromFile(const wstring& strRootFile, _uint iLevelIndex, const Matrix& matPivot)
{
	if (iLevelIndex >= m_iLevelNum) return E_FAIL;

	filesystem::path path(strRootFile);
	filesystem::directory_iterator iter(path);
	shared_ptr<CAnimEffectProp> pEffectProp = nullptr;
	wstring EffectOwner = path.parent_path().stem().wstring();
	string EffectOwnerKey = path.parent_path().stem().string();
	wstring wStrFBXName = L"";
	string strFBXName = "";

	while (iter != filesystem::end(iter))
	{
		if (iter->is_regular_file())
		{
			// fbx ���� �� ���
			if (iter->is_regular_file() && iter->path().extension() == ".AnimFbx")
			{
				auto Pair1 = m_vecAnimEffectMap[iLevelIndex].find(EffectOwner);
				if (Pair1 == m_vecAnimEffectMap[iLevelIndex].end())
				{
					AnimEffectlMap EffectMap;
					m_vecAnimEffectMap[iLevelIndex].emplace(EffectOwner, EffectMap);
				}

				auto Pair2 = m_vecAnimEffectKey[iLevelIndex].find(EffectOwnerKey);
				if (Pair2 == m_vecAnimEffectKey[iLevelIndex].end())
				{
					vector<string> EffectKeys;
					m_vecAnimEffectKey[iLevelIndex].emplace(EffectOwnerKey, EffectKeys);
				}

				pEffectProp = CAnimEffectProp::Create(m_pDevice, m_pContext);
				pEffectProp->Add_Buffer(iter->path().wstring(), matPivot);
				wStrFBXName = iter->path().stem().wstring();
				strFBXName = iter->path().stem().string();

				m_vecAnimEffectMap[iLevelIndex][EffectOwner].emplace(wStrFBXName, pEffectProp);
				m_vecAnimEffectKey[iLevelIndex][EffectOwnerKey].push_back(strFBXName);
			}
		}
		// ���� �� ���
		else if(iter->is_directory())
		{
			Load_AnimEffect_FromFile(iter->path().wstring(), iLevelIndex, matPivot);
		}

		++iter;
	}

	return S_OK;
}

HRESULT Engine::CResource_Manager::Add_Shader(const wstring& strShaderKey, const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	auto iter = m_ShaderMap.find(strShaderKey);
	if (iter != m_ShaderMap.end()) 
	{
		MSG_BOX("�̹� ��ϵ� ���̴� ������Ƽ Ű�Դϴ�");
		return E_FAIL;
	}

	shared_ptr<CShader> pShader = CShader::Create(m_pDevice, m_pContext, strShaderFilePath, pElements, iNumElements);

	if (nullptr == pShader) return E_FAIL;

	m_ShaderMap.emplace(strShaderKey, pShader);

	return S_OK;
}

shared_ptr<CTexture> Engine::CResource_Manager::Get_Texture(_uint iLevelIndex, const wstring& strTextureKey)
{
	if (iLevelIndex >= m_iLevelNum) return nullptr;

	auto iter = m_vecTextureMap[iLevelIndex].find(strTextureKey);

	if (iter == m_vecTextureMap[iLevelIndex].end())
	{
		MSG_BOX("�������� �ʴ� �ؽ�ó ������Ƽ Ű�Դϴ�");
		return nullptr;
	}

	return iter->second;
}

shared_ptr<CShader> Engine::CResource_Manager::Get_Shader(const wstring& strShaderKey)
{
	auto iter = m_ShaderMap.find(strShaderKey);

	if (iter == m_ShaderMap.end())
	{
		MSG_BOX("�������� �ʴ� ���̴� ������Ƽ Ű�Դϴ�");
		return nullptr;
	}

	return iter->second;
}

shared_ptr<CAnimModelProp> Engine::CResource_Manager::Get_AnimModel(_uint iLevelIndex, const wstring& strModelKey)
{
	if (iLevelIndex >= m_iLevelNum) return nullptr;

	auto iter = m_vecAnimModelMap[iLevelIndex].find(strModelKey);

	if (iter == m_vecAnimModelMap[iLevelIndex].end())
	{
		MSG_BOX("�������� �ʴ� �� ������Ƽ Ű�Դϴ�");
		return nullptr;
	}

	return iter->second;
}

shared_ptr<CNonAnimModelProp> Engine::CResource_Manager::Get_NonAnimModel(_uint iLevelIndex, const wstring& strModelKey)
{
	if (iLevelIndex >= m_iLevelNum) return nullptr;

	auto iter = m_vecNonAnimModelMap[iLevelIndex].find(strModelKey);

	if (iter == m_vecNonAnimModelMap[iLevelIndex].end())
	{
		MSG_BOX("�������� �ʴ� �� ������Ƽ Ű�Դϴ�");
		return nullptr;
	}

	return iter->second;
}

shared_ptr<CNavMeshProp> Engine::CResource_Manager::Get_NavMesh(_uint iLevelIndex, const wstring& strNavMeshKey)
{
	if (iLevelIndex >= m_iLevelNum) return nullptr;

	auto iter = m_vecNavMeshMap[iLevelIndex].find(strNavMeshKey);

	if (iter == m_vecNavMeshMap[iLevelIndex].end())
	{
		MSG_BOX("�������� �ʴ� �׺�޽� �Դϴ�");
		return nullptr;
	}

	return iter->second;
}

shared_ptr<Json::Value> Engine::CResource_Manager::Get_AnimEventData(const wstring& strModelKey)
{
	auto data = jsonEventDatas.find(strModelKey);
	if (data == jsonEventDatas.end())
		return nullptr;

	return make_shared<Json::Value>(data->second);
}

shared_ptr<CAnimEffectProp> Engine::CResource_Manager::Get_AnimEffect(_uint iLevelIndex, const wstring& strOwnerKey, const wstring& strEffectKey)
{
	auto Owner = m_vecAnimEffectMap[iLevelIndex].find(strOwnerKey);

	if (Owner == m_vecAnimEffectMap[iLevelIndex].end())
		return nullptr;

	auto Effect = Owner->second.find(strEffectKey);
	if (Effect == Owner->second.end())
		return nullptr;

	return Effect->second;
}

shared_ptr<Json::Value> Engine::CResource_Manager::Get_EffectEventData(const wstring& strModelKey)
{
	auto data = jsonEffectEventDatas.find(strModelKey);
	if (data == jsonEffectEventDatas.end())
		return nullptr;

	return make_shared<Json::Value>(data->second);
}

vector<string>* Engine::CResource_Manager::Get_PropertyKeyList(_uint iLevelIndex, PropertyType eType)
{
	switch (eType)
	{
	case Engine::PropertyType::Texture:
		return &m_vecTextureKey[iLevelIndex];
	case Engine::PropertyType::Terrain:
		return &m_vecTerrainKey[iLevelIndex];
	case Engine::PropertyType::NonAnimModel:
		return &m_vecNonAnimModelKey[iLevelIndex];
	case Engine::PropertyType::AnimModel:
		return &m_vecAnimModelKey[iLevelIndex];
	default:
		return nullptr;
	}
}

map<string, vector<string>>* Engine::CResource_Manager::Get_AnimEffectPropKeyList(_uint iLevelIndex)
{
	return &m_vecAnimEffectKey[iLevelIndex];
}

unordered_map<wstring, shared_ptr<CTexture>>* Engine::CResource_Manager::Get_TextureMap(_uint iLevelIndex)
{
	return &m_vecTextureMap[iLevelIndex];
}

shared_ptr<CRectProp> Engine::CResource_Manager::Get_RectProp()
{
	// �� �ѹ��� �簢�� ���� �Ҵ�
	if (nullptr == m_pRectProp)
		m_pRectProp = CRectProp::Create(m_pDevice, m_pContext);

	return m_pRectProp;
}

shared_ptr<CCubeProp> Engine::CResource_Manager::Get_CubeProp()
{
	if (nullptr == m_pCubeProp)
		m_pCubeProp = CCubeProp::Create(m_pDevice, m_pContext);

	return m_pCubeProp;
}

shared_ptr<CPointProp> Engine::CResource_Manager::Get_PointProp()
{
	if (nullptr == m_pPointProp)
		m_pPointProp = CPointProp::Create(m_pDevice, m_pContext);

	return m_pPointProp;
}

shared_ptr<CTerrainProp> Engine::CResource_Manager::Get_TerrainProp(_uint iLevelIndex, const wstring& strTerrainKey)
{
	if (iLevelIndex >= m_iLevelNum) return nullptr;

	auto iter = m_vecTerrainMap[iLevelIndex].find(strTerrainKey);

	if (iter == m_vecTerrainMap[iLevelIndex].end())
	{
		MSG_BOX("�������� �ʴ� �ͷ��� ������Ƽ Ű�Դϴ�");
		return nullptr;
	}

	return iter->second;
}

void Engine::CResource_Manager::Add_AnimData(const shared_ptr<CAnimModelProp> pModelProp, const wstring& strModelName)
{
	// �ش� ���� �̸����� �ִϸ��̼� �̺�Ʈ ������ �� ���� �˻�
	wstring path = L"../Bin/AnimEventData/" + strModelName + L".json";
	ifstream ifModelAnimEvent(path, ios::binary);
	// �ش� ������ �����Ѵٸ� ������ ä���ֱ�
	if (!ifModelAnimEvent.fail())
	{
		// ���̸�Ű ��ϵǾ��ִ��� �˻� : ���ٸ� �����ؼ� ä���ֱ�
		auto Pair = jsonEventDatas.find(strModelName);
		if (Pair == jsonEventDatas.end())
		{
			// Json������ �޾ƿ���
			Json::Value root;
			ifModelAnimEvent >> root;
			jsonEventDatas[strModelName] = root;
		}
	}

	ifModelAnimEvent.close();
}

void Engine::CResource_Manager::Add_EffectData(const shared_ptr<CAnimModelProp> pModelProp, const wstring& strModelName)
{
	// �ش� ���� �̸����� �ִϸ��̼� �̺�Ʈ ������ �� ���� �˻�
	wstring path = L"../Bin/EffectEventData/" + strModelName + L".json";
	ifstream ifModelAnimEvent(path, ios::binary);
	// �ش� ������ �����Ѵٸ� ������ ä���ֱ�
	if (!ifModelAnimEvent.fail())
	{
		// ���̸�Ű ��ϵǾ��ִ��� �˻� : ���ٸ� �����ؼ� ä���ֱ�
		auto Pair = jsonEffectEventDatas.find(strModelName);
		if (Pair == jsonEffectEventDatas.end())
		{
			// Json������ �޾ƿ���
			Json::Value root;
			ifModelAnimEvent >> root;
			jsonEffectEventDatas[strModelName] = root;
		}
	}

	ifModelAnimEvent.close();
}

shared_ptr<CResource_Manager> Engine::CResource_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumLevels)
{
	shared_ptr<CResource_Manager> pInstance = make_shared<CResource_Manager>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CResource_Manager");
		pInstance.reset();
	}

	return pInstance;
}
