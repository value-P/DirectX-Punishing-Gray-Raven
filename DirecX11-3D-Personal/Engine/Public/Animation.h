#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CBone;
	class CAnimModelProp;
	class CChannel;

	class CAnimation final
	{
	public:
		CAnimation();
		CAnimation(const CAnimation& rhs);
		virtual ~CAnimation() = default;

		void Set_KeyFrame(_int iKeyFrame, const vector<shared_ptr<CBone>>& Bones);

		vector<shared_ptr<CChannel>>& Get_Channels() { return m_Channels; }
		const _char* Get_Name() { return m_szName; }
		_int Get_MaxKeyFrame();
		_int Get_CurrentKeyFrame() {return m_iCurrentKeyFrames[maxKeyFrameIndex];}

	private:
		_double Get_KeyFrameTime(_int iKeyFrameIndex);

	public:
		HRESULT Initialize(const wstring& strFilePath, shared_ptr<CAnimModelProp> pModel, _uint& outFilePointer);
		void Inivalidate_TransformationMatrix(_float fTimeDelta, const vector<shared_ptr<CBone>>& Bones, _bool isLoop, Vector3& outMoveVelocity, _bool& outLastFrameCalled, _double& outFrameRate, _bool enableRootMotion);
		_bool Invalidate_TransformationMatrix_Lerp(_float fTimeDelta, const vector<shared_ptr<CChannel>>& Channels, const vector<shared_ptr<CBone>>& Bones, Vector3& outMoveVelocity);

		void Reset();

	private:
		_char	m_szName[MAX_PATH] = "";

		_double	m_Duration = 0.f;
		_double	m_TickPerSecond = 0.f;
		_double	m_TrackPosition = 0.f;

		_uint	m_iNumChannels = 0;
		vector<shared_ptr<CChannel>> m_Channels;
		vector<_uint> m_iCurrentKeyFrames;

		_int maxKeyFrameIndex = 0;

		_bool	m_isFinishied = false;

	public:
		static shared_ptr<CAnimation> Create(const wstring& strFilePath, shared_ptr<CAnimModelProp> pModel, _uint& outFilePointer);
		shared_ptr<CAnimation> Clone();
	};
}


