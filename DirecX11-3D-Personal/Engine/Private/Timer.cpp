#include "Timer.h"

HRESULT Engine::CTimer::Initialize()
{
    QueryPerformanceCounter(&m_FrameTime);
    QueryPerformanceCounter(&m_FixTime);
    QueryPerformanceCounter(&m_LastTime);
    QueryPerformanceFrequency(&m_CpuTick);

    return S_OK;
}

_float Engine::CTimer::Compute_TimeDelta()
{
    QueryPerformanceCounter(&m_FrameTime);
    if (m_FrameTime.QuadPart - m_FixTime.QuadPart >= m_CpuTick.QuadPart)
    {
        QueryPerformanceFrequency(&m_CpuTick);
        m_FixTime = m_FrameTime;
    }

    m_fTimeDelta = _float(m_FrameTime.QuadPart - m_LastTime.QuadPart) / (_float)m_CpuTick.QuadPart;

    m_LastTime = m_FrameTime;

    return m_fTimeDelta;
}

shared_ptr<CTimer> Engine::CTimer::Create()
{
    shared_ptr<CTimer> pInstance = make_shared<CTimer>();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CTimer");
        pInstance.reset();
    }

    return pInstance;
}
