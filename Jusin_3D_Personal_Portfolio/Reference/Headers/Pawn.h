#pragma once
#include "GameObject.h"

namespace Engine
{
	// �÷��̾ AI �� ������ �� �ִ� ��� ������ ���̽� Ŭ����
	class ENGINE_DLL CPawn abstract : public CGameObject
	{
	public:
		CPawn() = default;
		CPawn(const CPawn& rhs) = default;
		virtual ~CPawn() = default;

	public:
		// Pawn�� Ÿ���� ��ȯ

	protected:
		// �ִϸ����� ������Ʈ -> FSM
		// BehaviorTree������Ʈ
	};
}
