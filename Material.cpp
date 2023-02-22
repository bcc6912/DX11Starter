#include "Material.h"

Material::Material(XMFLOAT4 colorTint, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader)
{
	this->colorTint = colorTint;
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
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
