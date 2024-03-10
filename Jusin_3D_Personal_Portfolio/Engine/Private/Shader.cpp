#include "Shader.h"

Engine::CShader::CShader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CProperty(pDevice, pContext)
{
}

HRESULT Engine::CShader::Initialize(const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	_uint iHlslFlag = { 0 };

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	// 경로로 받아온 셰이더 파일을 컴파일하여 컴객체화 한다
	if (FAILED(D3DX11CompileEffectFromFile(strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice.Get(), m_pEffect.GetAddressOf(), nullptr)))
		return E_FAIL;

	D3DX11_EFFECT_DESC EffectDesc = {};
	m_pEffect->GetDesc(&EffectDesc);

	for (_uint i = 0; i < EffectDesc.Techniques; ++i)
	{
		// 테크닉 가져오기
		ComPtr<ID3DX11EffectTechnique> pTechnique = m_pEffect->GetTechniqueByIndex(i);
		if (pTechnique == nullptr) return E_FAIL;

		D3DX11_TECHNIQUE_DESC TechniqueDesc = {};
		pTechnique->GetDesc(&TechniqueDesc);

		for (_uint j = 0; j < TechniqueDesc.Passes; ++j)
		{
			// 패스 가져오기
			ComPtr<ID3D11InputLayout> pInputLayout = nullptr;

			ComPtr<ID3DX11EffectPass> pPass = pTechnique->GetPassByIndex(j);
			if (pPass == nullptr) return E_FAIL;

			D3DX11_PASS_DESC PassDesc = {};
			pPass->GetDesc(&PassDesc);

			// InputLayout생성
			if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, 
				PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, 
				pInputLayout.GetAddressOf())))
				return E_FAIL;

			m_InputLayouts.push_back(pInputLayout);
		}
	}

	return S_OK;
}

HRESULT Engine::CShader::Begin(_uint iPassIndex, _uint iTechniqueIndex)
{
	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(iTechniqueIndex);
	if (pTechnique == nullptr) return E_FAIL;
	
	ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(iPassIndex);
	if (pPass == nullptr) return E_FAIL;

	pPass->Apply(0, m_pContext.Get());

	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex].Get());

	return S_OK;
}

HRESULT Engine::CShader::Bind_SRV(const _char* pConstantName, ComPtr<ID3D11ShaderResourceView> pSRV)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (pVariable == nullptr) 
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (pSRVariable == nullptr) 
		return E_FAIL;

	return pSRVariable->SetResource(pSRV.Get());
}

HRESULT Engine::CShader::Bind_SRVs(const _char* pConstantName, const vector<ComPtr<ID3D11ShaderResourceView>>& vecSRVs)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (pVariable == nullptr) return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (pSRVariable == nullptr) return E_FAIL;

	vector<ID3D11ShaderResourceView*> tempVec;
	tempVec.reserve(vecSRVs.size());

	for (auto& iter : vecSRVs)
		tempVec.push_back(iter.Get());

	return pSRVariable->SetResourceArray(tempVec.data(), 0, (_uint)tempVec.size());
}

HRESULT Engine::CShader::Bind_Matrix(const _char* pConstantName, const Matrix* pMatrix)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (pVariable == nullptr) return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (pMatrixVariable == nullptr) return E_FAIL;

	return pMatrixVariable->SetMatrix((_float*)pMatrix);
}

HRESULT Engine::CShader::Bind_Matrices(const _char* pConstantName, const Matrix* pMatrices, _uint iNumMatrices)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (pVariable == nullptr) 
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable) 
		return E_FAIL;

	return pMatrixVariable->SetMatrixArray((_float*)pMatrices,0,iNumMatrices);
}

HRESULT Engine::CShader::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable) return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iLength);
}

shared_ptr<CShader> Engine::CShader::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	shared_ptr<CShader> pInstance = make_shared<CShader>(pDevice,pContext);

	if (FAILED(pInstance->Initialize(strShaderFilePath, pElements, iNumElements)))
	{
		MSG_BOX("Failed to Created : CShader");
		pInstance.reset();
	}

	return pInstance;
}
