#pragma once

#include "SimpleShader.h"

#include <DirectXMath.h>
#include <memory>

using namespace DirectX;

class Material
{
public:
	Material(XMFLOAT4 colorTint, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader, float roughness);

	// getters
	XMFLOAT4 GetColorTint();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	float GetRoughness();

	// setters
	void SetColorTint(XMFLOAT4 newColorTint);
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> newVertexShader);
	void SetPixelShader(std::shared_ptr<SimplePixelShader> newPixelShader);

	// helpers
	void PrepareMaterial(float roughness, XMFLOAT3 cameraPosition);

private:
	XMFLOAT4 colorTint;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;
	float roughness;
};

