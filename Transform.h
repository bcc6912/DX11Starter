#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Transform
{
public:
	Transform();
	~Transform();

	// setters
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 position);
	void SetRotation(float pitch, float yaw, float roll);
	void SetRotation(XMFLOAT3 rotation);
	void SetScale(float x, float y, float z);
	void SetScale(XMFLOAT3 scale);

	// getters
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetPitchYawRoll();
	XMFLOAT3 GetScale();
	XMFLOAT4X4 GetWorldMatrix();
	XMFLOAT4X4 GetWorldInverseTransposeMatrix();

	// transformers
	void MoveAbsolute(float x, float y, float z);
	void MoveAbsolute(XMFLOAT3 offset);
	void Rotate(float pitch, float yaw, float roll);
	void Rotate(XMFLOAT3 rotation);
	void Scale(float x, float y, float z);
	void Scale(XMFLOAT3 scale);
private:
	XMFLOAT3 position;
	XMFLOAT3 scale;
	XMFLOAT3 pitchYawRoll;
	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 worldInverseTransposeMatrix;

	// helper method for getters
	void UpdateMatrices();
};

