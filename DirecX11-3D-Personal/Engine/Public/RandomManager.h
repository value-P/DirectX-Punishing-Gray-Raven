#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CRandomManager final
	{
	public:
		CRandomManager() = default;
		virtual ~CRandomManager() = default;

	public:
		HRESULT Initialize();

		_int Rand_Num(_int iMin, _int iMax);
		_float Rand_Num(_float fMin, _float fMax);

	private:
		shared_ptr<mt19937> m_pGenerator = nullptr;

	public:
		static shared_ptr<CRandomManager> Create();
	};
}


