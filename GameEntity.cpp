#include "GameEntity.h"
#include "Vertex.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
{
	this->mesh = mesh;
	this->transform = std::make_shared<Transform>();
	this->material = material;
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

std::shared_ptr<Material> GameEntity::GetMaterial()
{
	return this->material;
}

void GameEntity::SetMaterial(std::shared_ptr<Material> newMaterial)
{
	this->material = newMaterial;
}

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, DirectX::XMFLOAT4 colorTint, std::shared_ptr<Camera> camera)
{
	this->material->GetVertexShader()->SetShader();
	this->material->GetPixelShader()->SetShader();

	/*
	* Commented out for Assignment 6
	VertexShaderExternalData vsData;
	vsData.colorTint = colorTint;
	vsData.worldMatrix = this->transform->GetWorldMatrix();
	vsData.viewMatrix = camera->GetView();
	vsData.projectionMatrix = camera->GetProjection();
	*/

	this->material->SetColorTint(colorTint);

	std::shared_ptr<SimpleVertexShader> vertexShader = this->material->GetVertexShader();
	// vertexShader->SetFloat4("colorTint", this->material->GetColorTint());
	vertexShader->SetMatrix4x4("world", this->transform->GetWorldMatrix());
	vertexShader->SetMatrix4x4("view", camera->GetView());
	vertexShader->SetMatrix4x4("projection", camera->GetProjection());

	// Assignment 7
	// Normals & World Position (Task 5)
	vertexShader->SetMatrix4x4("worldInverseTranspose", this->transform->GetWorldInverseTransposeMatrix());

	std::shared_ptr<SimplePixelShader> pixelShader = this->material->GetPixelShader();
	pixelShader->SetFloat4("colorTint", this->material->GetColorTint());

	/*
	* commented out for Assignment 6
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};

	context->Map(vsConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));
	context->Unmap(vsConstantBuffer.Get(), 0);
	*/

	vertexShader->CopyAllBufferData();

	pixelShader->CopyAllBufferData();

	this->material->PrepareMaterial(this->material->GetRoughness(), camera->GetTransform().GetPosition());

	this->mesh->Draw(context);
}