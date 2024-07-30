#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CTimer final
	{
	public:
		CTimer() = default;
		virtual ~CTimer() = default;

	public:
		HRESULT Initialize();

	public:
		_float Compute_TimeDelta();

	private:
		LARGE_INTEGER			m_FrameTime = {};
		LARGE_INTEGER			m_FixTime	= {};
		LARGE_INTEGER			m_LastTime	= {};
		LARGE_INTEGER			m_CpuTick	= {};

		_float					m_fTimeDelta = 0.f;

	public:
		static shared_ptr<CTimer> Create();
	};
}
