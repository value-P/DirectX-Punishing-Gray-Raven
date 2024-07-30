#include "Timer_Manager.h"
#include "Timer.h"

_float Engine::CTimer_Manager::Compute_TimeDelta(const wstring& strTimerTag)
{
	shared_ptr<CTimer> pTimer = Find_Timer(strTimerTag);

	if (pTimer == nullptr) return 0.f;

	return pTimer->Compute_TimeDelta();
}

HRESULT Engine::CTimer_Manager::Add_Timer(const wstring& strTimerTag)
{
	if (Find_Timer(strTimerTag) != nullptr) return E_FAIL;

	shared_ptr<CTimer> pTimer = CTimer::Create();
	if (pTimer == nullptr) return E_FAIL;

	m_mapTimer.emplace(strTimerTag, pTimer);

	return S_OK;
}

shared_ptr<CTimer> Engine::CTimer_Manager::Find_Timer(const wstring& strTimerTag)
{
	auto iter = m_mapTimer.find(strTimerTag);

	if (iter == m_mapTimer.end()) return nullptr;

	return iter->second;
}

shared_ptr<CTimer_Manager> Engine::CTimer_Manager::Create()
{
	return make_shared<CTimer_Manager>();
}
