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

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer)
{
	VertexShaderExternalData vsData;
	vsData.colorTint = XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f);;
	vsData.worldMatrix = this->transform->GetWorldMatrix();

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};

	context->Map(vsConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));
	context->Unmap(vsConstantBuffer.Get(), 0);

	this->mesh->Draw(context);
}