#include "Material.h"
#include "Texture.h"

Engine::CMaterial::CMaterial(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CProperty(pDevice, pContext)
{
}

HRESULT Engine::CMaterial::Initialize(const wstring& filePath, _uint& outFilePointer)
{
	ifstream istream(filePath, ios::binary);
	if (istream.fail()) return E_FAIL;

	istream.seekg(outFilePointer);

	for (size_t i = 0; i < MATERIAL_TEXTURE_TYPE_MAX; i++)
	{
		_uint numString = { 0 };
		istream.read((char*)&numString, sizeof(_uint));

		if (numString == 0)
			continue;
		
		_tchar* strMaterial = new _tchar[numString];
		ZeroMemory(strMaterial, sizeof(_tchar) * numString);
		istream.read((char*)strMaterial, sizeof(_tchar) * numString);

		size_t found = filePath.find_last_of(L"\\");
		wstring rootPath = filePath;

		if (found != wstring::npos)
			rootPath.erase(found + 1);

		wstring fullPath = rootPath + strMaterial;
		m_Matrials[i] = CTexture::Create(m_pDevice, m_pContext, fullPath);

		Safe_Delete_Array(strMaterial);
	}

	outFilePointer = static_cast<_uint>(istream.tellg());

	istream.close();

	return S_OK;
}

ComPtr<ID3D11ShaderResourceView> Engine::CMaterial::Get_MatrialSRV(MaterialTexType eMatrialType)
{
	if (nullptr == m_Matrials[eMatrialType]) return nullptr;

	return m_Matrials[eMatrialType]->GetSRV(0);
}

shared_ptr<CMaterial> Engine::CMaterial::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& filePath, _uint& outFilePointer)
{
	shared_ptr<CMaterial> pInstance = make_shared<CMaterial>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(filePath, outFilePointer)))
	{
		MSG_BOX("Failed to Created : CMaterial");
		pInstance.reset();
	}

	return pInstance;
}
