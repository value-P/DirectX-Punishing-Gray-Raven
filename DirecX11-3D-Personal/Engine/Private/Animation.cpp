#include "Animation.h"
#include "Channel.h"

Engine::CAnimation::CAnimation()
{
}

Engine::CAnimation::CAnimation(const CAnimation& rhs)
    :m_Duration(rhs.m_Duration),
    m_TickPerSecond(rhs.m_TickPerSecond),
    m_TrackPosition(rhs.m_TrackPosition),
    m_iNumChannels(rhs.m_iNumChannels),
    m_Channels(rhs.m_Channels),
    m_iCurrentKeyFrames(rhs.m_iCurrentKeyFrames),
    m_isFinishied(rhs.m_isFinishied),
    maxKeyFrameIndex(rhs.maxKeyFrameIndex)
{
    strcpy_s(m_szName, rhs.m_szName);
}

void Engine::CAnimation::Set_KeyFrame(_int iKeyFrame, const vector<shared_ptr<CBone>>& Bones)
{
    m_TrackPosition = Get_KeyFrameTime(iKeyFrame);

    for (_uint i = 0; i < m_iNumChannels; i++)
    {
        m_Channels[i]->Invalidate_TransformationMatrix(m_TrackPosition, m_iCurrentKeyFrames[i], Bones);
    }
}

_int Engine::CAnimation::Get_MaxKeyFrame()
{
    return m_Channels[maxKeyFrameIndex]->Get_NumKeyFrame();
}
_double Engine::CAnimation::Get_KeyFrameTime(_int iKeyFrameIndex)
{
    _double dTime = m_Channels[maxKeyFrameIndex]->Get_FrameTime(iKeyFrameIndex);

    return dTime;
}

HRESULT Engine::CAnimation::Initialize(const wstring& strFilePath, shared_ptr<CAnimModelProp> pModel, _uint& outFilePointer)
{
    ifstream istream(strFilePath, ios::binary);
    if (istream.fail())
        return E_FAIL;

    istream.seekg(outFilePointer);

    // 1. 문자열 수
    _uint numString = 0;
    istream.read((char*)&numString, sizeof(_uint));

    // 2. 애니메이션 이름
    _char* pName = new _char[numString];
    ZeroMemory(pName, sizeof(_char) * numString);
    istream.read(pName, sizeof(_char) * numString);

    strcpy_s(m_szName, pName);

    Safe_Delete_Array(pName);

    // 3. Duration
    istream.read((_char*)&m_Duration, sizeof(_double));

    // 4. TickPerSec
    istream.read((_char*)&m_TickPerSecond, sizeof(_double));

    // 5. 채널 개수
    istream.read((_char*)&m_iNumChannels, sizeof(_uint));
    m_iCurrentKeyFrames.resize(m_iNumChannels, 0);

    outFilePointer = static_cast<_uint>(istream.tellg());

    // 6. 채널
    m_Channels.reserve(m_iNumChannels);
    for (_uint i = 0; i < m_iNumChannels; i++)
    {
        shared_ptr<CChannel> pChannel = CChannel::Create(strFilePath, pModel, outFilePointer);

        if (nullptr == pChannel)
        {
            istream.close();
            return E_FAIL;
        }

        m_Channels.push_back(pChannel);
    }

    istream.close();

    // 키프레임 끝까지 다 가지고있는 인덱스 1개 등록(채널뭔지는 상관없음)
    _int iMaxKeyFrame = 0;
    for (size_t i = 0; i < m_Channels.size(); ++i)
    {
        if (iMaxKeyFrame < m_Channels[i]->Get_NumKeyFrame())
        {
            iMaxKeyFrame = m_Channels[i]->Get_NumKeyFrame();
            maxKeyFrameIndex = i;
        }
    }

    return S_OK;
}

void Engine::CAnimation::Inivalidate_TransformationMatrix(_float fTimeDelta, const vector<shared_ptr<CBone>>& Bones, _bool isLoop, Vector3& outMoveVelocity, _bool& outLastFrameCalled, _double& outFrameRate, _bool enableRootMotion)
{
    m_TrackPosition += m_TickPerSecond * fTimeDelta;

    if (m_TrackPosition >= m_Duration)
    {

        if (false == isLoop)
        {
            m_isFinishied = true;
        }
        else
        {
            m_TrackPosition = 0.0;
        }
    }
    else
        outLastFrameCalled = false;

    outFrameRate = m_TrackPosition / m_Duration;

    if (m_TrackPosition >= m_Duration * 0.9)
        outLastFrameCalled = true;

    for (_uint i = 0; i < m_iNumChannels; i++)
    {
        m_Channels[i]->Invalidate_TransformationMatrix(m_TrackPosition,m_iCurrentKeyFrames[i], Bones, outMoveVelocity, enableRootMotion);
    }
}

_bool Engine::CAnimation::Invalidate_TransformationMatrix_Lerp(_float fTimeDelta, const vector<shared_ptr<CChannel>>& Channels, const vector<shared_ptr<CBone>>& Bones, Vector3& outMoveVelocity)
{
    m_TrackPosition += m_TickPerSecond * fTimeDelta;

    _bool result =true;

    for (_uint i = 0; i < m_iNumChannels; i++)
    {
        auto channel = find_if(Channels.begin(), Channels.end(), [&](shared_ptr<CChannel> pChannel) {
            return !strcmp(m_Channels[i]->Get_Name(), pChannel->Get_Name());
            });

        if (channel == Channels.end())
        {
            if (false == m_Channels[i]->Invalidate_TransformationMatrix_ToInitial(m_TrackPosition, m_iCurrentKeyFrames[i], Bones, outMoveVelocity))
                result = false;
            //m_Channels[i]->Invalidate_TransformationMatrix(m_TrackPosition, m_iCurrentKeyFrames[i], Bones, outMoveVelocity, false);
        }
        else
        {
            if (false == m_Channels[i]->Invalidate_TransformationMatrix_Lerp(m_TrackPosition, m_iCurrentKeyFrames[i], (*channel)->Get_StartKeyFrame(), Bones, outMoveVelocity))
                result = false;
        }
    }

    return result;
}

void Engine::CAnimation::Reset()
{
    for (auto& keyFrame : m_iCurrentKeyFrames)
        keyFrame = 0;

    for (auto& channel : m_Channels)
        channel->Reset();

    m_TrackPosition = 0.0;
}

shared_ptr<CAnimation> Engine::CAnimation::Create(const wstring& strFilePath, shared_ptr<CAnimModelProp> pModel, _uint& outFilePointer)
{
    shared_ptr<CAnimation> pInstance = make_shared<CAnimation>();

    if (FAILED(pInstance->Initialize(strFilePath, pModel, outFilePointer)))
    {
        MSG_BOX("Failed to Created : CAnimation");
        pInstance.reset();
    }

    return pInstance;
}

shared_ptr<CAnimation> Engine::CAnimation::Clone()
{
    return make_shared<CAnimation>(*this);
}
