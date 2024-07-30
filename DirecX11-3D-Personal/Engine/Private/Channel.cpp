#include "Channel.h"
#include "AnimModelProp.h"
#include "Bone.h"

Engine::CChannel::CChannel()
{
}

void Engine::CChannel::Reset()
{
    m_vPrevPosition = m_KeyFrames[0].vPosition;
}

HRESULT Engine::CChannel::Initialize(const wstring& strFilePath, shared_ptr<CAnimModelProp> pModel, _uint& outFilePointer)
{
    ifstream istream(strFilePath, ios::binary);
    if (istream.fail())
        return E_FAIL;

    istream.seekg(outFilePointer);

    // 1. 문자열 수
    _uint numString = 0;
    istream.read((char*)&numString, sizeof(_uint));

    // 2. 채널 이름
    _char* pName = new _char[numString];
    ZeroMemory(pName, sizeof(_char) * numString);
    istream.read(pName, sizeof(_char) * numString);

    strcpy_s(m_szName, pName);

    Safe_Delete_Array(pName);

    // 동일한 이름의 본 인덱스 추출
    m_iBoneIndex = pModel->Get_BoneIndex(m_szName);

    // 3. 키프레임 개수
    istream.read((_char*)&m_iNumKeyFrames, sizeof(_uint));

    // 4. 키프레임들 : 파싱 툴에서 빈 데이터는 가장 마지막 데이터로 채워넣었음
    m_KeyFrames.resize(m_iNumKeyFrames);
    istream.read((_char*)m_KeyFrames.data(), sizeof(KEYFRAME) * m_iNumKeyFrames);

    if(m_KeyFrames.size() >= 1)
        m_vPrevPosition = m_KeyFrames[0].vPosition;

    outFilePointer = (_uint)istream.tellg();

    istream.close();

    return S_OK;
}

void Engine::CChannel::Invalidate_TransformationMatrix(_double TrackPosition, _uint& iCurrentKeyFrame, const vector<shared_ptr<CBone>>& Bones, Vector3& outMoveVelocity, _bool enableRootMotion)
{
    if (0.0 == TrackPosition)
    {
        m_vPrevPosition = m_KeyFrames[0].vPosition;
        iCurrentKeyFrame = 0;
    }

    KEYFRAME lastKeyFrame = m_KeyFrames.back();

    Matrix TransformationMatrix;

    Vector3 vScale, vTranslation;
    Quaternion vRotation;

    // 마지막 키프레임(애니메이션 끝)
    if (TrackPosition >= lastKeyFrame.Time)
    {
        vScale = lastKeyFrame.vScale;
        vRotation = lastKeyFrame.vRotation;
        vTranslation = lastKeyFrame.vPosition;
    }
    else
    {
        while(TrackPosition >= m_KeyFrames[iCurrentKeyFrame + 1].Time)
            ++iCurrentKeyFrame;

        _double Ratio = (TrackPosition - m_KeyFrames[iCurrentKeyFrame].Time) /
            (m_KeyFrames[iCurrentKeyFrame + 1].Time - m_KeyFrames[iCurrentKeyFrame].Time);

		vScale = Vector3::Lerp(m_KeyFrames[iCurrentKeyFrame].vScale, m_KeyFrames[iCurrentKeyFrame + 1].vScale, (_float)Ratio);
        vRotation = Quaternion::Slerp(m_KeyFrames[iCurrentKeyFrame].vRotation, m_KeyFrames[iCurrentKeyFrame + 1].vRotation, (_float)Ratio);
        vTranslation = Vector3::Lerp(m_KeyFrames[iCurrentKeyFrame].vPosition, m_KeyFrames[iCurrentKeyFrame + 1].vPosition, (_float)Ratio);
    }

    Vector4 scale = XMLoadFloat3(&vScale);
    Vector4 translation = XMVectorSetW(XMLoadFloat3(&vTranslation), 1.f);
    
    if (Bones[m_iBoneIndex]->IsRootBone() && enableRootMotion)
    {
        Vector3 moveVelocity;
        moveVelocity.x = vTranslation.x - m_vPrevPosition.x;
        moveVelocity.z = vTranslation.z - m_vPrevPosition.z;

        outMoveVelocity = moveVelocity;

        m_vPrevPosition = vTranslation;
        translation = Vector4(0.f, vTranslation.y, 0.f, 1.f);
    }

    TransformationMatrix = XMMatrixAffineTransformation(scale, Vector4(0.f, 0.f, 0.f, 1.f), vRotation, translation);

    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void Engine::CChannel::Invalidate_TransformationMatrix(_double TrackPosition, _uint& iCurrentKeyFrame, const vector<shared_ptr<CBone>>& Bones)
{
    if (0.0 == TrackPosition)
    {
        m_vPrevPosition = m_KeyFrames[0].vPosition;
        iCurrentKeyFrame = 0;
    }

    KEYFRAME lastKeyFrame = m_KeyFrames.back();

    Matrix TransformationMatrix;

    Vector3 vScale, vTranslation;
    Quaternion vRotation;

    // 마지막 키프레임(애니메이션 끝)
    if (TrackPosition >= lastKeyFrame.Time)
    {
        vScale = lastKeyFrame.vScale;
        vRotation = lastKeyFrame.vRotation;
        vTranslation = lastKeyFrame.vPosition;
    }
    else
    {
        if (TrackPosition >= m_KeyFrames[iCurrentKeyFrame].Time)
        {
            while (TrackPosition >= m_KeyFrames[iCurrentKeyFrame + 1].Time)
                ++iCurrentKeyFrame;
        }
        else
        {
            while (TrackPosition < m_KeyFrames[iCurrentKeyFrame].Time)
            {
                if (iCurrentKeyFrame == 0)
                    break;

                --iCurrentKeyFrame;
            }
        }
        
        _double Ratio = (TrackPosition - m_KeyFrames[iCurrentKeyFrame].Time) /
            (m_KeyFrames[iCurrentKeyFrame + 1].Time - m_KeyFrames[iCurrentKeyFrame].Time);

        vScale = Vector3::Lerp(m_KeyFrames[iCurrentKeyFrame].vScale, m_KeyFrames[iCurrentKeyFrame + 1].vScale, (_float)Ratio);
        vRotation = Quaternion::Slerp(m_KeyFrames[iCurrentKeyFrame].vRotation, m_KeyFrames[iCurrentKeyFrame + 1].vRotation, (_float)Ratio);
        vTranslation = Vector3::Lerp(m_KeyFrames[iCurrentKeyFrame].vPosition, m_KeyFrames[iCurrentKeyFrame + 1].vPosition, (_float)Ratio);
    }

    Vector4 scale = XMLoadFloat3(&vScale);
    Vector4 translation = XMVectorSetW(XMLoadFloat3(&vTranslation), 1.f);

    if (Bones[m_iBoneIndex]->IsRootBone())
    {
        Vector3 moveVelocity;
        moveVelocity.x = vTranslation.x - m_vPrevPosition.x;
        moveVelocity.z = vTranslation.z - m_vPrevPosition.z;

        translation = Vector4(0.f, vTranslation.y, 0.f, 1.f);
    }

    TransformationMatrix = XMMatrixAffineTransformation(scale, Vector4(0.f, 0.f, 0.f, 1.f), vRotation, translation);

    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

_bool Engine::CChannel::Invalidate_TransformationMatrix_Lerp(_double TrackPosition, _uint& iCurrentKeyFrame, KEYFRAME nextKeyFrame, const vector<shared_ptr<CBone>>& Bones, Vector3& outMoveVelocity)
{
    Matrix TransformationMatrix;

    Vector3 vScale, vTranslation;
    Quaternion vRotation;

    _double Ratio = (TrackPosition - m_KeyFrames[iCurrentKeyFrame].Time) / 0.1;

    if (Ratio >= 1.0)
    {
        m_vPrevPosition = m_KeyFrames[0].vPosition;
        return true;
    }

    vScale = Vector3::Lerp(m_KeyFrames[iCurrentKeyFrame].vScale, nextKeyFrame.vScale, (_float)Ratio);
    vRotation = Quaternion::Slerp(m_KeyFrames[iCurrentKeyFrame].vRotation, nextKeyFrame.vRotation, (_float)Ratio);
    vTranslation = Vector3::Lerp(m_KeyFrames[iCurrentKeyFrame].vPosition, nextKeyFrame.vPosition, (_float)Ratio);
  
    Vector4 scale = XMLoadFloat3(&vScale);
    Vector4 translation = XMVectorSetW(XMLoadFloat3(&vTranslation), 1.f);

    if (Bones[m_iBoneIndex]->IsRootBone())
    {
        outMoveVelocity = Vector3(0.f,0.f,0.f);
        translation = Vector4(0.f, translation.y, 0.f, 1.f);
    }

    TransformationMatrix = XMMatrixAffineTransformation(scale, Vector4(0.f, 0.f, 0.f, 1.f), vRotation, translation);

    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);

    return false;
}

_bool Engine::CChannel::Invalidate_TransformationMatrix_ToInitial(_double TrackPosition, _uint& iCurrentKeyFrame, const vector<shared_ptr<CBone>>& Bones, Vector3& outMoveVelocity)
{
    Matrix TransformationMatrix;

    Vector3 vScale, vTranslation;
    Quaternion vRotation;

    _double Ratio = (TrackPosition - m_KeyFrames[iCurrentKeyFrame].Time) / 0.1;

    Matrix InitMatrix = Bones[m_iBoneIndex]->Get_InitialTransformationMatrix();
    KEYFRAME InitKeyframe;
    InitMatrix.Decompose(InitKeyframe.vScale, InitKeyframe.vRotation, InitKeyframe.vPosition);

    if (Ratio >= 1.0)
    {
        m_vPrevPosition = m_KeyFrames[0].vPosition;
        return true;
    }

    vScale = Vector3::Lerp(m_KeyFrames[iCurrentKeyFrame].vScale, InitKeyframe.vScale, (_float)Ratio);
    vRotation = Quaternion::Slerp(m_KeyFrames[iCurrentKeyFrame].vRotation, InitKeyframe.vRotation, (_float)Ratio);
    vTranslation = Vector3::Lerp(m_KeyFrames[iCurrentKeyFrame].vPosition, InitKeyframe.vPosition, (_float)Ratio);

    Vector4 scale = XMLoadFloat3(&vScale);
    Vector4 translation = XMVectorSetW(XMLoadFloat3(&vTranslation), 1.f);

    if (Bones[m_iBoneIndex]->IsRootBone())
    {
        outMoveVelocity = Vector3(0.f, 0.f, 0.f);
        translation = Vector4(0.f, translation.y, 0.f, 1.f);
    }

    TransformationMatrix = XMMatrixAffineTransformation(scale, Vector4(0.f, 0.f, 0.f, 1.f), vRotation, translation);

    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);

    return false;
}

shared_ptr<CChannel> Engine::CChannel::Create(const wstring& strFilePath, shared_ptr<CAnimModelProp> pModel, _uint& outFilePointer)
{
    shared_ptr<CChannel> pInstance = make_shared<CChannel>();

    if (FAILED(pInstance->Initialize(strFilePath, pModel, outFilePointer)))
    {
        MSG_BOX("Failed to Created : CChannel");
        pInstance.reset();
    }

    return pInstance;
}
