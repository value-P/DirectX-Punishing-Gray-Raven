#pragma once
#include "UIBase.h"

namespace Client
{
	class CPlayableVera;
	class CSkillBtn;

	class CSkillBtnContainer final : public CUIBase
	{
		struct Slot
		{
			Vector2 vPos;
			shared_ptr<CSkillBtn> button;
		};

	public:
		struct INIT_DESC : CUIBase::INIT_DESC
		{
			LEVEL eCreateLevel;
		};

	public:
		CSkillBtnContainer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CSkillBtnContainer() = default;

	public:
		void CreateSkillBall();
		void CheckSkillChain();
		
	private:
		void ReAllocateSlot();
		void SetStackAndFriend(_int iStartPointer, _int iCheckEndPointer);

	public:
		virtual HRESULT Initialize(const INIT_DESC& initDesc, shared_ptr<CPlayableVera> pPlayer);

	private:
		weak_ptr<CPlayableVera> m_pPlayer;

		list<shared_ptr<CSkillBtn>> m_pSkillBalls;

		Slot m_tSlots[12];
		Vector2 m_vEndPos = {};
	public:
		static shared_ptr<CSkillBtnContainer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc, shared_ptr<CPlayableVera> pPlayer);
	};
}