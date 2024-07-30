#pragma once
#include "Engine_Defines.h"

namespace Engine
{
	class CAnimModelProp;
	class CBone;

	class CChannel final
	{
	public:
		struct KEYFRAME
		{
			Vector3 vScale, vPosition;
			Quaternion vRotation;
			_double Time;
		};

	public:
		CChannel();
		virtual ~CChannel() = default;

	public:
		KEYFRAME Get_StartKeyFrame() { return m_KeyFrames[0]; }
		_double Get_FrameTime(_int iKeyFrameIndex) { return m_KeyFrames[iKeyFrameIndex].Time; }
		_char* Get_Name() { return m_szName; }
		_int Get_NumKeyFrame() { return static_cast<_int>(m_iNumKeyFrames); }
		void Reset();

	public:
		HRESULT Initialize(const wstring& strFilePath, shared_ptr<CAnimModelProp> pModel, _uint& outFilePointer);
		void Invalidate_TransformationMatrix(_double TrackPosition, _uint& iCurrentKeyFrame, const vector<shared_ptr<CBone>>& Bones, Vector3& outMoveVelocity, _bool enableRootMotion);
		void Invalidate_TransformationMatrix(_double TrackPosition, _uint& iCurrentKeyFrame, const vector<shared_ptr<CBone>>& Bones);
		_bool Invalidate_TransformationMatrix_Lerp(_double TrackPosition, _uint& iCurrentKeyFrame, KEYFRAME nextKeyFrame, const vector<shared_ptr<CBone>>& Bones, Vector3& outMoveVelocity);
		_bool Invalidate_TransformationMatrix_ToInitial(_double TrackPosition, _uint& iCurrentKeyFrame, const vector<shared_ptr<CBone>>& Bones, Vector3& outMoveVelocity);

	private:
		_char	m_szName[MAX_PATH] = "";
		_uint	m_iNumKeyFrames = 0;
		vector<KEYFRAME> m_KeyFrames;

		Vector3 m_vPrevPosition;

		_int	m_iBoneIndex = 0;

	public:
		static shared_ptr<CChannel> Create(const wstring& strFilePath, shared_ptr<CAnimModelProp> pModel, _uint& outFilePointer);
	};
}


