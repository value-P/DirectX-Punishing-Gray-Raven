#include "RandomManager.h"

HRESULT Engine::CRandomManager::Initialize()
{
	random_device randDevice;
	
	m_pGenerator = make_shared<mt19937>(randDevice());

	if (m_pGenerator == nullptr)
		return E_FAIL;

	return S_OK;
}

_int Engine::CRandomManager::Rand_Num(_int iMin, _int iMax)
{
	uniform_int_distribution<int> dis(iMin, iMax);

	return dis(*m_pGenerator);
}

_float Engine::CRandomManager::Rand_Num(_float fMin, _float fMax)
{
	uniform_real_distribution<float> dis(fMin, fMax);

	return dis(*m_pGenerator);
}

shared_ptr<CRandomManager> Engine::CRandomManager::Create()
{
	shared_ptr<CRandomManager> pInstance = make_shared<CRandomManager>();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CRandomManager");
		pInstance.reset();
	}

	return pInstance;
}
