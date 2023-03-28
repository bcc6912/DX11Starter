#pragma once

#include "SimpleShader.h"
#include "Mesh.h"
#include "WICTextureLoader.h"
#include "Camera.h"
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <memory>
#include <string>

class Sky
{
public:
	Sky(std::shared_ptr<Mesh> mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions, 
		Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, 
		std::shared_ptr<SimplePixelShader> pixelShader, std::shared_ptr<SimpleVertexShader> vertexShader,
		const wchar_t* right, const wchar_t* left, const wchar_t* up, const wchar_t* down, const wchar_t* front, const wchar_t* back);

	void Draw(std::shared_ptr<Camera> camera);

	// --------------------------------------------------------
	// Author: Chris Cascioli
	// Purpose: Creates a cube map on the GPU from 6 individual textures
	// 
	// - You are allowed to directly copy/paste this into your code base
	//   for assignments, given that you clearly cite that this is not
	//   code of your own design.
	//
	// - Note: This code assumes you’re putting the function in Sky.cpp, 
	//   you’ve included WICTextureLoader.h and you have an ID3D11Device 
	//   ComPtr called “device”.  Make any adjustments necessary for
	//   your own implementation.
	// --------------------------------------------------------
	// Helper for creating a cubemap from 6 individual textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMapSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthBufferComparisonType;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerOptions;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<SimplePixelShader> skyPixelShader;
	std::shared_ptr<SimpleVertexShader> skyVertexShader;
};

