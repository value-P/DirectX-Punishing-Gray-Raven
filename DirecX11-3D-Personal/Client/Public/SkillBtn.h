#pragma once
#include "UIBase.h"

namespace Client
{
	class CPlayableVera;
	class CSkillBtnContainer;

	class CSkillBtn final : public CUIBase
	{
	public:
		enum SKILLCOLOR { RED, BLUE, YELLOW, COLOR_END };

	public:
		CSkillBtn(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CSkillBtn() = default;

	public:
		void Set_Pos(Vector2 vPos);
		virtual void Activate(SKILLCOLOR eColor, Vector2 vStartPos, Vector2 vDestPos);
		virtual void InActivate();

		SKILLCOLOR Get_Color() { return m_eCurrentColor; }
		
		void Set_Player(shared_ptr<CPlayableVera> pPlayer);
		void Set_SkillStack(_int iStack) { m_iSkillStack = iStack; }
		void Add_FriendBtn(shared_ptr<CSkillBtn> pBtn);
		void Set_DestPos(Vector2 vDestPos) { m_vDestPos = vDestPos; }

	public:
		virtual HRESULT Initialize(const INIT_DESC& initDesc, shared_ptr<CSkillBtnContainer> pContainer);
		virtual _int Tick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	public:
		virtual void OnClickEnter() override;

	private:
		shared_ptr<CTexture> m_pSkillTextures[COLOR_END];
		weak_ptr<CPlayableVera> m_pPlayer;
		weak_ptr<CSkillBtnContainer> m_pContainer;
		SKILLCOLOR m_eCurrentColor = COLOR_END;
		list<weak_ptr<CSkillBtn>> m_pFriendButtonList;
		_int m_iSkillStack = 1;
		Vector2 m_vDestPos = { };

	public:
		static shared_ptr<CSkillBtn> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const INIT_DESC& initDesc, shared_ptr<CSkillBtnContainer> pContainer);
	};
}
