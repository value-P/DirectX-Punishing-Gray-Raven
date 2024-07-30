#pragma once
#include "ImguiBase.h"
#include "Cell.h"
#include "NavMeshProp.h"

namespace Tool
{
	class CToolMain;
	class CTool_Terrain;
	class CTool_MapMesh;
	class CTool_Player;

    class CSampleUI : public CImguiBase
    {
	public:
		CSampleUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
		virtual ~CSampleUI() = default;

	public:
		virtual HRESULT Initialize(shared_ptr<CToolMain> pMainTool);
		virtual void Tick() override;
		virtual void Render() override;

	public:
		void Key_Input();
		void sortPointsClockWise();
		_bool ParseNavMeshData(const string& savePath);
		void AnimationKeyFrameWindow();
		void AnimationEventWindow();

	public:
		ImGuiWindowFlags m_eWindowFlag = 0;

		Vector3 m_vPickedPos = {0.f,0.f,0.f};
		_float m_vInitPos[3] = { 0.f,0.f,0.f };

		shared_ptr<CTool_Terrain> m_pTerrain = nullptr;
		_char m_strPropKey[MAX_PATH] = "";
		_int   m_Size[2] = { 0,0 };

		vector<const _char*> szMapKeys;
		shared_ptr<CTool_MapMesh> m_pMapMesh = nullptr;
		_int iPickedMap = 0;
		_float m_scales[3] = { 1.f,1.f,1.f };

		_bool AnimationMode = false;
		vector<const _char*> szAnimModelKeys;
		shared_ptr<CTool_Player> m_pModelMesh = nullptr;
		_int iPickedAnimModel = 0;
		_int iOutKeyFrame = 0;
		_int iAnimIndex = 0;
		_float fAnimSpeed = 1.f;
		_int iCurrentKeyFrame = 0;

		_bool m_bActivateNavmeshMaking = false;
		_bool m_bPointsFull = false;
		_int  m_iPointsCount = 0;
		shared_ptr<CNavMeshProp> NavMeshProp = nullptr;
		shared_ptr<CNavMeshProp> m_pCheckingNavMesh = nullptr;
		Vector3 vPoints[3] = {};
		char navMeshSavePath[MAX_PATH] = "";

		// 카메라 포지션
		Vector3 vCamPosition = {};

		// 애니메이션 이벤트 저장용
		Json::Value AnimEvents;
		_int functionsIndex = 0;
		const _char* EventFunctions[19] = { "EnableBodyCollider", "DisAbleBodyCollider", 
											"EnAbleAttackCollider1", "DisAbleAttackCollider1",
											"EnAbleAttackCollider2", "DisAbleAttackCollider2", 
											"EnAbleAttackCollider3", "DisAbleAttackCollider3",
											"ActiveParryFlag", "InActiveParryFlag",
											"UseSkill1", "UseSkill2", "UseSkill3", "UseSkill4",
				 							"PlayTrail", "StopTrail",
											"GroundEffectPlay",
											"DodgeColEnable", "DodgeColDisable"};

		// 애니메이션(키프레임이벤트들(키프레임 인덱스,이벤트명))
		list<string> originStr;
		map<string, map<_int, vector<const _char*>>> EventDatas;
		_int iListIndex = 0;
		string pickedModelName = "";

	private:
		weak_ptr<CToolMain> m_pToolMain;

	public:
		static shared_ptr<CSampleUI> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<CToolMain> pMainTool);
	};
}


