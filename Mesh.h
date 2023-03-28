#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include "Vertex.h"
#include <DirectXMath.h>
#include <vector>
#include <string>

class Mesh
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	// Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	int indicesCount = 0;

	void CreateBuffers(Vertex* vertices, int vertexCount, unsigned int* indices, int indexCount, Microsoft::WRL::ComPtr<ID3D11Device> device);

public:
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
	Mesh(Vertex* vertices, int vertexCount, unsigned int* indices, int indexCount, Microsoft::WRL::ComPtr<ID3D11Device> device);
	~Mesh();
	
	// Assignment 6
	Mesh(const std::wstring& fileName, Microsoft::WRL::ComPtr<ID3D11Device> device);

	// Assignment 9
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
};