#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include "Mesh.h"
#include <memory>
#include <vector>

class Game
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();
	void CreateGeometry();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	// holds meshes
	std::vector<std::shared_ptr<Mesh>> meshes{ std::shared_ptr<Mesh>(), std::shared_ptr<Mesh>(), std::shared_ptr<Mesh>()};

	// Assignment 3
	Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer;

	// DirectX::XMFLOAT4 colorTint = DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f);
	// DirectX::XMFLOAT3 offset = DirectX::XMFLOAT3(0.25f, 0.0f, 0.0f);

	// different tints for each mesh
	// must add a new item manually to use on new mesh
	std::vector<DirectX::XMFLOAT4> colorTints{ DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f) };
	std::vector<DirectX::XMFLOAT3> offsets{ DirectX::XMFLOAT3(0.25f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.25f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.25f, 0.0f, 0.0f) };
};

