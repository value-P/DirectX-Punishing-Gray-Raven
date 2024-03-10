#pragma once
#include "Component.h"

namespace Engine
{
    class CNavigation;

    class ENGINE_DLL CTransform final : public CComponent
    {
    public:
        CTransform();
        virtual ~CTransform() = default;

    public:
        Vector3 Get_Right() { return m_WorldMatrix.Right(); }
        Vector3 Get_Up() { return m_WorldMatrix.Up(); }
        Vector3 Get_Look() { return m_WorldMatrix.Backward(); } // 오른손 좌표계라서 반대 방향값을 반환해줌
        Vector3 Get_Pos() { return m_WorldMatrix.Translation(); }
        Vector3 Get_Scaled() { return Vector3(Get_Right().Length(), Get_Up().Length(), Get_Look().Length()); }
        Matrix Get_WorldMatrix() { return m_WorldMatrix; }

        void Set_Right(Vector3 vRight) { m_WorldMatrix.Right(vRight); }
        void Set_Up(Vector3 vUp) { m_WorldMatrix.Up(vUp); }
        void Set_Look(Vector3 vLook) { m_WorldMatrix.Backward(vLook); }
        void Set_Pos(Vector3 vPos) { m_WorldMatrix.Translation(vPos); }
        void Set_Y(_float fY) { m_WorldMatrix._42 = fY; }

        void Set_Pos(Vector3 vPos, shared_ptr <CNavigation> pNavigation);

    public:
        void Go_Straight(_float fSpeed);
        void Go_Backward(_float fSpeed);
        void Go_Right(_float fSpeed);
        void Go_Up(_float fSpeed);
        void Go_Left(_float fSpeed);
        void Go_Direction(const Vector3& vDir, _float fSpeed);

        void Go_Straight(_float fSpeed, shared_ptr<CNavigation> pNavigation);
        void Go_Right(_float fSpeed, shared_ptr <CNavigation> pNavigation);
        void Go_Direction(const Vector3& velocity, shared_ptr <CNavigation> pNavigation);

        void Turn(Vector4 vAxis,_float fSpeed);
        void Turn(Vector3 vRotation);
        void Rotation(Vector4 vAxis, _float fRadian);
        void Rotation(Vector3 vRotation);
        void LookAt(Vector4 vPoint);


    public:
        void Set_Scaling(_float fScaleX, _float fScaleY, _float fScaleZ);

    private:
        Matrix m_WorldMatrix;

    public:
        static shared_ptr<CTransform> Create();
    };
}


