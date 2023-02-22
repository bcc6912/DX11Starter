#include "Camera.h"
#include "Input.h"

using namespace DirectX;

Camera::Camera(float x, float y, float z, float moveSpeed, float mouseLookSpeed, float fieldOfView, float aspectRatio, bool perspective)
{
	this->transform.SetPosition(x, y, z);
	this->movementSpeed = moveSpeed;
	this->mouseLookSpeed = mouseLookSpeed;
	this->fov = fieldOfView;
	this->perspective = perspective;

	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	XMStoreFloat4x4(&this->projectionMatrix, XMMatrixPerspectiveFovLH(this->fov, aspectRatio, 0.01f, 100.0f));
}

void Camera::UpdateViewMatrix()
{
	XMFLOAT3 transformPos = this->transform.GetPosition();
	XMFLOAT3 transformForward = this->transform.GetForward();

	XMStoreFloat4x4(&this->viewMatrix, XMMatrixLookToLH(XMLoadFloat3(&transformPos), XMLoadFloat3(&transformForward), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
}

void Camera::Update(float deltaTime)
{
	Input& input = Input::GetInstance();

	if (input.KeyDown('W'))
	{
		if (input.KeyDown(VK_LSHIFT))
		{
			this->transform.MoveRelative(0.0f, 0.0f, this->movementSpeed * deltaTime * 2);
		}
		else
		{
			this->transform.MoveRelative(0.0f, 0.0f, this->movementSpeed * deltaTime);
		}
	}
	if (input.KeyDown('A'))
	{
		if (input.KeyDown(VK_LSHIFT))
		{
			this->transform.MoveRelative(-this->movementSpeed * deltaTime * 2, 0.0f, 0.0f);
		}
		else
		{
			this->transform.MoveRelative(-this->movementSpeed * deltaTime, 0.0f, 0.0f);
		}
	}
	if (input.KeyDown('S'))
	{
		if (input.KeyDown(VK_SHIFT))
		{
			this->transform.MoveRelative(0.0f, 0.0f, -this->movementSpeed * deltaTime * 2);
		}
		else
		{
			this->transform.MoveRelative(0.0f, 0.0f, -this->movementSpeed * deltaTime);
		}
	}
	if (input.KeyDown('D'))
	{
		if (input.KeyDown(VK_SHIFT))
		{
			this->transform.MoveRelative(this->movementSpeed * deltaTime * 2, 0.0f, 0.0f);
		}
		else
		{
			this->transform.MoveRelative(this->movementSpeed * deltaTime, 0.0f, 0.0f);
		}
	}
	if (input.KeyDown(' '))
	{
		this->transform.MoveAbsolute(0.0f, this->movementSpeed * deltaTime, 0.0f);
	}
	if (input.KeyDown('X'))
	{
		this->transform.MoveAbsolute(0.0f, -this->movementSpeed * deltaTime, 0.0f);
	}

	// check for mouse movement when dragging
	if(input.MouseLeftDown())
	{
		float xDiff = input.GetMouseXDelta() * this->mouseLookSpeed;
		float yDiff = input.GetMouseYDelta() * this->mouseLookSpeed;
		transform.Rotate(yDiff, xDiff, 0);
	}

	UpdateViewMatrix();
}

XMFLOAT4X4 Camera::GetView()
{
	return this->viewMatrix;
}


XMFLOAT4X4 Camera::GetProjection()
{
	return this->projectionMatrix;
}

float Camera::GetFOV()
{
	return this->fov;
}

Transform Camera::GetTransform()
{
	return this->transform;
}