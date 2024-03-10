#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CTimer;

	class CTimer_Manager final
	{
	public :
		CTimer_Manager() = default;
		virtual ~CTimer_Manager() = default;

	public:
		_float Compute_TimeDelta(const wstring& strTimerTag);
		HRESULT Add_Timer(const wstring& strTimerTag);

	private:
		shared_ptr<CTimer> Find_Timer(const wstring& strTimerTag);

	private:
		map<const wstring, shared_ptr<CTimer>> m_mapTimer;

	public:
		static shared_ptr<CTimer_Manager> Create();
	};
}
