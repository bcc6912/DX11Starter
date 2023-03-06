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

	// Assignment 8
	for (auto& t : this->textureSRVs)
	{
		this->pixelShader->SetShaderResourceView(t.first.c_str(), t.second);
	}
	for (auto& s : this->samplers)
	{
		this->pixelShader->SetSamplerState(s.first.c_str(), s.second);
	}
}

/*
void Material::PrepareMaterial(std::shared_ptr<Transform> transform, std::shared_ptr<Camera> camera)
{
	this->vertexShader->SetShader();
	this->pixelShader->SetShader();
	
	this->vertexShader->SetMatrix4x4("world", transform->GetWorldMatrix());
	this->vertexShader->SetMatrix4x4("view", camera->GetView());
	this->vertexShader->SetMatrix4x4("projection", camera->GetProjection());
	this->vertexShader->SetMatrix4x4("worldInverseTranspose", transform->GetWorldInverseTransposeMatrix());

	this->pixelShader->SetFloat4("colorTint", this->colorTint);

	this->vertexShader->CopyAllBufferData();

	this->pixelShader->CopyAllBufferData();

	// Assignment 8
	for (auto& t : this->textureSRVs)
	{
		this->pixelShader->SetShaderResourceView(t.first.c_str(), t.second);
	}
	for (auto& s : this->samplers)
	{
		this->pixelShader->SetSamplerState(s.first.c_str(), s.second);
	}
}
*/

void Material::AddTextureSRV(std::string shaderName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
	this->textureSRVs.insert({ shaderName, srv });
}

void Material::AddSampler(std::string samplerName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
	this->samplers.insert({ samplerName, sampler });
}

void Material::SetRoughness(float roughness)
{
	this->roughness = roughness;
}
