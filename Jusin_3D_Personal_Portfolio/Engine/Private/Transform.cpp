#include "Transform.h"
#include "Navigation.h"

Engine::CTransform::CTransform()
    :CComponent(nullptr, nullptr)
{
}

void Engine::CTransform::Set_Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
    m_WorldMatrix.Right(XMVector3Normalize(Get_Right()) * fScaleX);
    m_WorldMatrix.Up(XMVector3Normalize(Get_Up()) * fScaleY);
    m_WorldMatrix.Backward(XMVector3Normalize(Get_Look()) * fScaleZ);
}

shared_ptr<CTransform> Engine::CTransform::Create()
{
	shared_ptr<CTransform> pInstance = make_shared<CTransform>();

	if (nullptr == pInstance)
		MSG_BOX("Failed to Created : CTransform");

	return pInstance;
}

void Engine::CTransform::Set_Pos(Vector3 vPos, shared_ptr<CNavigation> pNavigation)
{
	if (nullptr == pNavigation || true == pNavigation->isMove(vPos))
		m_WorldMatrix.Translation(vPos);
}

void Engine::CTransform::Go_Straight(_float fSpeed)
{
	Vector3 vPos = Get_Pos();
	Vector3 vLookDir = Get_Look();
	vLookDir.Normalize();

	vPos += vLookDir * fSpeed;

	Set_Pos(vPos);
}

void Engine::CTransform::Go_Backward(_float fSpeed)
{
	Vector3 vPos = Get_Pos();
	Vector3 vLookDir = Get_Look();
	vLookDir.Normalize();

	vPos -= vLookDir * fSpeed;

	Set_Pos(vPos);
}

void Engine::CTransform::Go_Right(_float fSpeed)
{
	Vector3 vPos = Get_Pos();
	Vector3 vRight = Get_Right();
	vRight.Normalize();

	vPos += vRight * fSpeed;

	Set_Pos(vPos);
}

void Engine::CTransform::Go_Up(_float fSpeed)
{
	Vector3 vPos = Get_Pos();
	Vector3 vUp = Get_Up();
	vUp.Normalize();

	vPos += vUp * fSpeed;

	Set_Pos(vPos);
}

void Engine::CTransform::Go_Left(_float fSpeed)
{
	Vector3 vPos = Get_Pos();
	Vector3 vRight = Get_Right();
	vRight.Normalize();

	vPos -= vRight * fSpeed;

	Set_Pos(vPos);
}

void Engine::CTransform::Go_Direction(const Vector3& vDir, _float fSpeed)
{
	Vector3 vPos = Get_Pos();
	vPos += vDir * fSpeed;

	Set_Pos(vPos);
}

void Engine::CTransform::Go_Straight(_float fSpeed, shared_ptr<CNavigation> pNavigation)
{
	Vector3 vPos = Get_Pos();
	Vector3 vLookDir = Get_Look();
	vLookDir.Normalize();

	vPos += vLookDir * fSpeed;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPos))
		Set_Pos(vPos);
}

void Engine::CTransform::Go_Right(_float fSpeed, shared_ptr<CNavigation> pNavigation)
{
	Vector3 vPos = Get_Pos();
	Vector3 vRight = Get_Right();
	vRight.Normalize();

	vPos += vRight * fSpeed;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPos))
		Set_Pos(vPos);
}

void Engine::CTransform::Go_Direction(const Vector3& velocity, shared_ptr <CNavigation> pNavigation)
{
	Vector3 vPos = Get_Pos();
	Vector3 vLookDir = Get_Look();
	Vector3 vRight = Get_Right();
	vLookDir.Normalize();
	vRight.Normalize();

	vPos += vRight * velocity.x;
	vPos += vLookDir * velocity.z;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPos))
		Set_Pos(vPos);
}

void Engine::CTransform::Turn(Vector4 vAxis, _float fSpeed)
{
	Quaternion qRotation = XMQuaternionRotationAxis(vAxis, fSpeed);
	Matrix matRot = XMMatrixRotationQuaternion(qRotation);

	Set_Right(XMVector4Transform(Get_Right(), matRot));
	Set_Up(XMVector4Transform(Get_Up(), matRot));
	Set_Look(XMVector4Transform(Get_Look(), matRot));

}

void Engine::CTransform::Turn(Vector3 vRotation)
{
	Quaternion qRotationX = XMQuaternionRotationAxis(Vector4(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(vRotation.x));
	Quaternion qRotationY = XMQuaternionRotationAxis(Vector4(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(vRotation.y));
	Quaternion qRotationZ = XMQuaternionRotationAxis(Vector4(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(vRotation.z));
	Matrix matRot = XMMatrixRotationQuaternion(qRotationX);
	matRot *= XMMatrixRotationQuaternion(qRotationY);
	matRot *= XMMatrixRotationQuaternion(qRotationZ);

	Set_Right(XMVector4Transform(Get_Right(), matRot));
	Set_Up(XMVector4Transform(Get_Up(), matRot));
	Set_Look(XMVector4Transform(Get_Look(), matRot));
}

void Engine::CTransform::Rotation(Vector4 vAxis, _float fRadian)
{
	Quaternion qRotation = XMQuaternionRotationAxis(vAxis, fRadian);
	Matrix matRot = XMMatrixRotationQuaternion(qRotation);
	Vector3 vScale = Get_Scaled();

	Vector4 vRight = { vScale.x , 0.f,0.f,0.f };
	Vector4 vUp = { 0.f, vScale.y,0.f,0.f };
	Vector4 vLook = { 0.f, 0.f,vScale.z,0.f };

	Set_Right(XMVector4Transform(vRight, matRot));
	Set_Up(XMVector4Transform(vUp, matRot));
	Set_Look(XMVector4Transform(vLook, matRot));
}

void Engine::CTransform::Rotation(Vector3 vDgreeRoataion)
{
	Quaternion qRotationX = XMQuaternionRotationAxis(Vector4(1.f,0.f,0.f,0.f), XMConvertToRadians(vDgreeRoataion.x));
	Quaternion qRotationY = XMQuaternionRotationAxis(Vector4(0.f, 1.f,0.f,0.f), XMConvertToRadians(vDgreeRoataion.y));
	Quaternion qRotationZ = XMQuaternionRotationAxis(Vector4(0.f, 0.f, 1.f,0.f), XMConvertToRadians(vDgreeRoataion.z));
	Matrix matRot = XMMatrixRotationQuaternion(qRotationX);
	matRot *= XMMatrixRotationQuaternion(qRotationY);
	matRot *= XMMatrixRotationQuaternion(qRotationZ);
	Vector3 vScale = Get_Scaled();

	Vector4 vRight = { vScale.x , 0.f,0.f,0.f };
	Vector4 vUp = { 0.f, vScale.y,0.f,0.f };
	Vector4 vLook = { 0.f, 0.f,vScale.z,0.f };

	Set_Right(XMVector4Transform(vRight, matRot));
	Set_Up(XMVector4Transform(vUp, matRot));
	Set_Look(XMVector4Transform(vLook, matRot));

}

void Engine::CTransform::LookAt(Vector4 vPoint)
{
	Vector3 vScaled = Get_Scaled();

	Vector3 vLook = vPoint - Get_Pos();
	Vector3 vRight = Vector3(0.f,1.f,0.f);
	vRight.Cross(vLook, vRight);
	Vector3 vUp = XMVector3Cross(vLook, vRight);
	vLook.Normalize();
	vRight.Normalize();
	vUp.Normalize();

	Set_Right(vRight * vScaled.x);
	Set_Up(vUp * vScaled.y);
	Set_Look(vLook * vScaled.z);	
}
