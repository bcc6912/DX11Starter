#include "GameEntity.h"
#include "Vertex.h"
#include "BufferStructs.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh)
{
	this->mesh = mesh;
	this->transform = std::make_shared<Transform>();
}

GameEntity::~GameEntity()
{

}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
	return this->mesh;
}

std::shared_ptr<Transform> GameEntity::GetTransform()
{
	return this->transform;
}

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer, DirectX::XMFLOAT4 colorTint, std::shared_ptr<Camera> camera)
{
	VertexShaderExternalData vsData;
	vsData.colorTint = colorTint;
	vsData.worldMatrix = this->transform->GetWorldMatrix();
	vsData.viewMatrix = camera->GetView();
	vsData.projectionMatrix = camera->GetProjection();

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};

	context->Map(vsConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));
	context->Unmap(vsConstantBuffer.Get(), 0);

	this->mesh->Draw(context);
}