#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include "Mesh.h"
#include <memory>
#include <vector>
#include "GameEntity.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Material.h"
#include "Lights.h"
#include "WICTextureLoader.h"
#include "Sky.h"

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
	// Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	// Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	// holds meshes
	std::vector<std::shared_ptr<Mesh>> meshes{ std::shared_ptr<Mesh>(), std::shared_ptr<Mesh>(), std::shared_ptr<Mesh>()};

	// Assignment 3
	// Removed for assignment 6
	// Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer;

	// DirectX::XMFLOAT4 colorTint = DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f);
	// DirectX::XMFLOAT3 offset = DirectX::XMFLOAT3(0.25f, 0.0f, 0.0f);

	// different tints for each mesh
	// must add a new item manually to use on new mesh
	// removed individual tint edit temporarily
	// std::vector<DirectX::XMFLOAT4> colorTints{ DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f) };
	std::vector<DirectX::XMFLOAT3> offsets{ DirectX::XMFLOAT3(0.25f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.25f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.25f, 0.0f, 0.0f) };

	// Assignment 4
	std::vector<std::shared_ptr<GameEntity>> entities;

	// Assignment 5
	// std::shared_ptr<Camera> camera;
	// DirectX::XMFLOAT4 colorTint = DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f);
	DirectX::XMFLOAT4 colorTint = DirectX::XMFLOAT4(0.67f, 0.67f, 0.67f, 1.0f);
	std::vector<std::shared_ptr<Camera>> cameras;
	int activeCamera = 0;

	// Assignment 6
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::vector<std::shared_ptr<Material>> materials;
	std::shared_ptr<SimplePixelShader> customPShader;
	float timer = 0.0f;
	bool timerUp = true;

	// Assignment 7
	// Ambient (Task 4)
	DirectX::XMFLOAT3 ambientColor = DirectX::XMFLOAT3(0.76f, 0.66f, 0.73f);
	// Light (Task 6)
	Light directionalLight1 = {};
	// More Lights (Task 8)
	Light directionalLight2 = {};
	Light directionalLight3 = {};
	// Point Lights (Task 9)
	Light pointLight1 = {};
	Light pointLight2 = {};

	std::vector<Light> lights;

	// Assignment 8
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalSpecularSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalNormalSRV;

	// previously asphalt, now cobblestone for assignment 9
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobblestoneSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobblestoneSpecularSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobblestoneNormalSRV;
	float roughness = 0.5f;

	// Assignment 9
	std::shared_ptr<Sky> sky;
	std::shared_ptr<Mesh> skyMesh;
	std::shared_ptr<SimpleVertexShader> skyVertexShader;
	std::shared_ptr<SimplePixelShader> skyPixelShader;
};

