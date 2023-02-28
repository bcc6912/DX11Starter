#include "Material.h"

Material::Material(XMFLOAT4 colorTint, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader, float roughness)
{
	this->colorTint = colorTint;
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
	this->roughness = roughness;
}

XMFLOAT4 Material::GetColorTint()
{
	return this->colorTint;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
	return this->vertexShader;
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
	return this->pixelShader;
}

float Material::GetRoughness()
{
	return this->roughness;
}

void Material::SetColorTint(XMFLOAT4 newColorTint)
{
	this->colorTint = newColorTint;
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> newVertexShader)
{
	this->vertexShader = newVertexShader;
}

void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> newPixelShader)
{
	this->pixelShader = newPixelShader;
}

void Material::PrepareMaterial(float roughness, XMFLOAT3 cameraPosition)
{
	this->roughness = roughness;
	this->pixelShader->SetFloat("roughness", this->roughness);
	this->pixelShader->SetFloat3("cameraPosition", cameraPosition);
}
