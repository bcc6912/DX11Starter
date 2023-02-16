#include "Transform.h"

using namespace DirectX;

Transform::Transform()
{
	this->position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	this->pitchYawRoll = XMFLOAT3(0.0f, 0.0f, 0.0f);

	this->rightVector = XMFLOAT3(1.0f, 0.0f, 0.0f);
	this->upVector = XMFLOAT3(0.0f, 1.0f, 0.0f);
	this->forwardVector = XMFLOAT3(0.0f, 0.0f, 1.0f);


	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixIdentity());
}

void Transform::SetPosition(float x, float y, float z)
{
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;
}

void Transform::SetPosition(XMFLOAT3 position)
{
	this->position = position;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	this->pitchYawRoll.x = pitch;
	this->pitchYawRoll.y = yaw;
	this->pitchYawRoll.z = roll;
}

void Transform::SetRotation(XMFLOAT3 rotation)
{
	this->pitchYawRoll = rotation;
}

void Transform::SetScale(float x, float y, float z)
{
	this->scale.x = x;
	this->scale.y = y;
	this->scale.z = z;
}

void Transform::SetScale(XMFLOAT3 scale)
{
	this->scale = scale;
}

XMFLOAT3 Transform::GetPosition()
{
	return this->position;
}

XMFLOAT3 Transform::GetPitchYawRoll()
{
	return this->pitchYawRoll;
}

XMFLOAT3 Transform::GetScale()
{
	return this->scale;
}

void Transform::UpdateMatrices()
{
	float x = this->position.x;
	XMMATRIX translation = XMMatrixTranslation(this->position.x, this->position.y, this->position.z);
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(this->pitchYawRoll.x, this->pitchYawRoll.y, this->pitchYawRoll.z);
	XMMATRIX scale = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);

	XMMATRIX world = scale * rotation * translation;
	XMStoreFloat4x4(&this->worldMatrix, world);
	XMStoreFloat4x4(&this->worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(world)));
}

XMFLOAT4X4 Transform::GetWorldMatrix()
{
	UpdateMatrices();
	return this->worldMatrix;
}

XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	UpdateMatrices();
	return this->worldInverseTransposeMatrix;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	this->position.x += x;
	this->position.y += y;
	this->position.z += z;
	float newX = this->position.x;
}

void Transform::MoveAbsolute(XMFLOAT3 offset)
{
	this->position.x += offset.x;
	this->position.y += offset.y;
	this->position.z += offset.z;
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	this->pitchYawRoll.x += pitch;
	this->pitchYawRoll.y += yaw;
	this->pitchYawRoll.z += roll;
}

void Transform::Rotate(XMFLOAT3 rotation)
{
	this->pitchYawRoll.x += rotation.x;
	this->pitchYawRoll.y += rotation.y;
	this->pitchYawRoll.z += rotation.z;
}

void Transform::Scale(float x, float y, float z)
{
	this->scale.x *= x;
	this->scale.y *= y;
	this->scale.z *= z;
}

void Transform::Scale(XMFLOAT3 scale)
{
	this->scale.x += scale.x;
	this->scale.y += scale.y;
	this->scale.z += scale.z;
}

void Transform::MoveRelative(float x, float y, float z)
{
	// store parameters and quaternion of current rotation as vectors
	XMVECTOR moveVector = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR moveQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&this->pitchYawRoll));

	// use above vectors to rotate direction
	XMVECTOR relativeDir = XMVector3Rotate(moveVector, moveQuat);

	// move current position using relativeDir
	XMStoreFloat3(&this->position, XMLoadFloat3(&this->position) + relativeDir);
}

void Transform::MoveRelative(XMFLOAT3 offset)
{
	// store parameter and quaternion of current rotation as vectors
	XMVECTOR moveVector = XMVectorSet(offset.x, offset.y, offset.z, 1.0f);
	XMVECTOR moveQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&this->pitchYawRoll));

	// use above vectors to rotate direction
	XMVECTOR relativeDir = XMVector3Rotate(moveVector, moveQuat);

	// XMVECTOR positionVector = XMLoadFloat3(&this->position);

	// move current position using relativeDir
	XMStoreFloat3(&this->position, XMLoadFloat3(&this->position) + relativeDir);
}

XMFLOAT3 Transform::GetRight()
{
	XMVECTOR rotVector = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR rotQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&this->pitchYawRoll));

	XMVECTOR relativeDir = XMVector3Rotate(rotVector, rotQuat);

	XMStoreFloat3(&this->rightVector, relativeDir);

	return this->rightVector;
}

XMFLOAT3 Transform::GetUp()
{
	XMVECTOR rotVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR rotQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&this->pitchYawRoll));

	XMVECTOR relativeDir = XMVector3Rotate(rotVector, rotQuat);

	XMStoreFloat3(&this->upVector, relativeDir);

	return this->upVector;
}

XMFLOAT3 Transform::GetForward()
{
	XMVECTOR rotVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR rotQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&this->pitchYawRoll));

	XMVECTOR relativeDir = XMVector3Rotate(rotVector, rotQuat);

	XMStoreFloat3(&this->forwardVector, relativeDir);

	return this->forwardVector;
}

Transform::~Transform()
{

}