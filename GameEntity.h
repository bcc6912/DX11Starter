#pragma once

#include "Transform.h"
#include "Mesh.h"
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include "Camera.h"
#include "Material.h"

#include <memory>

class GameEntity
{
public:
	GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
	~GameEntity();

	// getters
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();
	std::shared_ptr<Material> GetMaterial();

	// setters
	void SetMaterial(std::shared_ptr<Material> newMaterial);

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, DirectX::XMFLOAT4 colorTint, std::shared_ptr<Camera> camera);

private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
};

