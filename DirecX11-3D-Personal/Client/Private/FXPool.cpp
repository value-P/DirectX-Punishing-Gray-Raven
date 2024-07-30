#include "pch.h"
#include "FXPool.h"
#include "ThrowObject.h"

Client::CFXPool::CFXPool()
	:m_pGameInstance(CGameInstance::GetInstance())
{
}

void Client::CFXPool::Reserve(_uint iSize)
{
	if (iSize > m_vecFXPool.size())
		m_vecFXPool.reserve(iSize);
}

void Client::CFXPool::PushBack(shared_ptr<CThrowObject> pFX, LEVEL eLevel, const wstring& strLayerTag, _float* pTimeScale)
{
	if (nullptr == pFX)
		return;

	m_pGameInstance.lock()->Add_Object(eLevel, strLayerTag, pFX, pTimeScale);
	m_vecFXPool.push_back(pFX);
}

shared_ptr<CThrowObject> Client::CFXPool::PopActivate()
{
	for (auto& pFX : m_vecFXPool)
	{
		if (true == pFX->isActivate())
			return pFX;
	}

	return nullptr;
}

shared_ptr<CThrowObject> Client::CFXPool::PopInActivate()
{
	for (auto& pFX : m_vecFXPool)
	{
		if (false == pFX->isActivate())
			return pFX;
	}

	return nullptr;
}
