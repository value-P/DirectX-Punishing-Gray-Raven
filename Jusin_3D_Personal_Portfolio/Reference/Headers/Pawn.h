#pragma once
#include "GameObject.h"

namespace Engine
{
	// 플레이어나 AI 가 제어할 수 있는 모든 액터의 베이스 클래스
	class ENGINE_DLL CPawn abstract : public CGameObject
	{
	public:
		CPawn() = default;
		CPawn(const CPawn& rhs) = default;
		virtual ~CPawn() = default;

	public:
		// Pawn의 타입을 반환

	protected:
		// 애니메이터 컴포넌트 -> FSM
		// BehaviorTree컴포넌트
	};
}
