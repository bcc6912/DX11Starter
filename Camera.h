#pragma once

#include "Transform.h"
#include <DirectXMath.h>
#include <memory>

using namespace DirectX;

class Camera
{
public:
	Camera(float x, float y, float z, float moveSpeed, float mouseLookSpeed, float fieldOfView, float aspectRatio, bool perspective);

	// getters
	XMFLOAT4X4 GetView();
	XMFLOAT4X4 GetProjection();
	float GetFOV();
	Transform GetTransform();

	void Update(float deltaTime);
	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float aspectRatio);

private:
	Transform transform;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
	float fov; // radians
	float movementSpeed;
	float mouseLookSpeed;
	bool perspective; // if not true, camera is orthographic
};

