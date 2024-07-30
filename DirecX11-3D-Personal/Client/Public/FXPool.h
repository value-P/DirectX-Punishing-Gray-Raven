#pragma once

namespace Client
{
	class CThrowObject;

	class CFXPool
	{
	public:
		CFXPool();
		virtual ~CFXPool() = default;

	public:
		shared_ptr<CThrowObject> PopActivate();
		shared_ptr<CThrowObject> PopInActivate();

	public:
		void Reserve(_uint iSize);
		void PushBack(shared_ptr<CThrowObject> pFX, LEVEL eLevel, const wstring& strLayerTag, _float* pTimeScale);

	private:
		weak_ptr<CGameInstance> m_pGameInstance;
		vector<shared_ptr<CThrowObject>> m_vecFXPool;
	};
}