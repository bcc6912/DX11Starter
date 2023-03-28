#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "Helpers.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

#include "WICTextureLoader.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// Direct3D itself, and our window, are not ready at this point!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,			// The application's handle
		L"DirectX Game",	// Text for the window's title bar (as a wide-character string)
		1280,				// Width of the window's client area
		720,				// Height of the window's client area
		false,				// Sync the framerate to the monitor refresh? (lock framerate)
		true)				// Show extra stats (fps) in title bar?
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Delete all objects manually created within this class
//  - Release() all Direct3D objects created within this class
// --------------------------------------------------------
Game::~Game()
{
	// Call delete or delete[] on any objects or arrays you've
	// created using new or new[] within this class
	// - Note: this is unnecessary if using smart pointers

	// Call Release() on any Direct3D objects made within this class
	// - Note: this is unnecessary for D3D objects stored in ComPtrs

	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

// --------------------------------------------------------
// Called once per program, after Direct3D and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();

	CreateGeometry();
	
	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		// context->VSSetShader(vertexShader.Get(), 0, 0);
		// context->PSSetShader(pixelShader.Get(), 0, 0);
	}

	/*
	* Commented out for Assigment 6
	// Get size as the next multiple of 16 (instead of hardcoding a size here!)
	unsigned int size = sizeof(VertexShaderExternalData);
	size = (size + 15) / 16 * 16; // This will work even if the struct size changes

	// Describe the constant buffer
	D3D11_BUFFER_DESC cbDesc = {}; // Sets struct to all zeros
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.ByteWidth = size; // Must be a multiple of 16
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;

	device->CreateBuffer(&cbDesc, 0, vsConstantBuffer.GetAddressOf());
	*/

	// Initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(device.Get(), context.Get());

	ImGui::StyleColorsDark();

	// Assignment 5
	this->cameras.push_back(std::make_shared<Camera>(0.0f, 4.5f, -20.0f, 1.0f, 0.001f, XM_PIDIV4, (float)this->windowWidth / this->windowHeight, true));
	this->cameras.push_back(std::make_shared<Camera>(-0.3f, 4.5f, -20.4f, 1.0f, 0.001f, -XM_PIDIV4, (float)this->windowWidth / this->windowHeight, true));

	// Assignment 7
	this->directionalLight1.Type = 0;
	this->directionalLight1.Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	this->directionalLight1.Color = XMFLOAT3(1.0f, 0.0f, 0.0f);
	this->directionalLight1.Intensity = 1.0f;
	// Task 8
	// directionalLight2
	this->directionalLight2.Type = 0;
	this->directionalLight2.Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	this->directionalLight2.Color = XMFLOAT3(0.0f, 1.0f, 0.0f);
	this->directionalLight2.Intensity = 1.0f;
	// directionalLight3
	this->directionalLight3.Type = 0;
	this->directionalLight3.Direction = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	this->directionalLight3.Color = XMFLOAT3(0.0f, 0.0f, 1.0f);
	this->directionalLight3.Intensity = 1.0f;
	// Task 9
	// pointLight1
	this->pointLight1.Type = 1;
	// this->pointLight1.Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->pointLight1.Color = XMFLOAT3(1.0f, 0.5f, 1.0f);
	this->pointLight1.Intensity = 2.5f;
	this->pointLight1.Range = 10.0f;
	this->pointLight1.Position = XMFLOAT3(6.0f, 4.0f, -2.0f);
	// pointLight2
	this->pointLight2.Type = 1;
	// this->pointLight2.Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->pointLight2.Color = XMFLOAT3(1.0f, 0.6f, 0.2f);
	this->pointLight2.Intensity = 1.0f;
	this->pointLight2.Range = 10.0f;
	this->pointLight2.Position = XMFLOAT3(-6.0f, 4.0f, 0.0f);

	lights.push_back(directionalLight1);
	lights.push_back(directionalLight2);
	lights.push_back(directionalLight3);
	lights.push_back(pointLight1);
	lights.push_back(pointLight2);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	/*
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

		// Create the actual Direct3D shaders on the GPU
		device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
			pixelShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer

		device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer
	}

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
		inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Create the input layout, verifying our description against actual shader code
		device->CreateInputLayout(
			inputElements,							// An array of descriptions
			2,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}
	*/

	this->vertexShader = std::make_shared<SimpleVertexShader>(this->device, this->context, FixPath(L"VertexShader.cso").c_str());
	this->pixelShader = std::make_shared<SimplePixelShader>(this->device, this->context, FixPath(L"PixelShader.cso").c_str());
	this->customPShader = std::make_shared<SimplePixelShader>(this->device, this->context, FixPath(L"CustomPS.cso").c_str());

	// Assignment 9
	this->skyVertexShader = std::make_shared<SimpleVertexShader>(this->device, this->context, FixPath(L"SkyVertexShader.cso").c_str());
	this->skyPixelShader = std::make_shared<SimplePixelShader>(this->device, this->context, FixPath(L"SkyPixelShader.cso").c_str());
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateGeometry()
{
	/*
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red	= XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green	= XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue	= XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in CPU memory
	//    over to a Direct3D-controlled data structure on the GPU (the vertex buffer)
	// - Note: Since we don't have a camera or really any concept of
	//    a "3d world" yet, we're simply describing positions within the
	//    bounds of how the rasterizer sees our screen: [-1 to +1] on X and Y
	// - This means (0,0) is at the very center of the screen.
	// - These are known as "Normalized Device Coordinates" or "Homogeneous 
	//    Screen Coords", which are ways to describe a position without
	//    knowing the exact size (in pixels) of the image/window/etc.  
	// - Long story short: Resizing the window also resizes the triangle,
	//    since we're describing the triangle in terms of the window itself
	Vertex vertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), red },
		{ XMFLOAT3(+0.5f, -0.5f, +0.0f), blue },
		{ XMFLOAT3(-0.5f, -0.5f, +0.0f), green },
	};

	// Set up indices, which tell us which vertices to use and in which order
	// - This is redundant for just 3 vertices, but will be more useful later
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int indices[] = { 0, 1, 2 };


	// Create a VERTEX BUFFER
	// - This holds the vertex data of triangles for a single object
	// - This buffer is created on the GPU, which is where the data needs to
	//    be if we want the GPU to act on it (as in: draw it to the screen)
	{
		// First, we need to describe the buffer we want Direct3D to make on the GPU
		//  - Note that this variable is created on the stack since we only need it once
		//  - After the buffer is created, this description variable is unnecessary
		D3D11_BUFFER_DESC vbd	= {};
		vbd.Usage				= D3D11_USAGE_IMMUTABLE;	// Will NEVER change
		vbd.ByteWidth			= sizeof(Vertex) * 3;       // 3 = number of vertices in the buffer
		vbd.BindFlags			= D3D11_BIND_VERTEX_BUFFER; // Tells Direct3D this is a vertex buffer
		vbd.CPUAccessFlags		= 0;	// Note: We cannot access the data from C++ (this is good)
		vbd.MiscFlags			= 0;
		vbd.StructureByteStride = 0;

		// Create the proper struct to hold the initial vertex data
		// - This is how we initially fill the buffer with data
		// - Essentially, we're specifying a pointer to the data to copy
		D3D11_SUBRESOURCE_DATA initialVertexData = {};
		initialVertexData.pSysMem = vertices; // pSysMem = Pointer to System Memory

		// Actually create the buffer on the GPU with the initial data
		// - Once we do this, we'll NEVER CHANGE DATA IN THE BUFFER AGAIN
		device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());
	}

	// Create an INDEX BUFFER
	// - This holds indices to elements in the vertex buffer
	// - This is most useful when vertices are shared among neighboring triangles
	// - This buffer is created on the GPU, which is where the data needs to
	//    be if we want the GPU to act on it (as in: draw it to the screen)
	{
		// Describe the buffer, as we did above, with two major differences
		//  - Byte Width (3 unsigned integers vs. 3 whole vertices)
		//  - Bind Flag (used as an index buffer instead of a vertex buffer) 
		D3D11_BUFFER_DESC ibd	= {};
		ibd.Usage				= D3D11_USAGE_IMMUTABLE;	// Will NEVER change
		ibd.ByteWidth			= sizeof(unsigned int) * 3;	// 3 = number of indices in the buffer
		ibd.BindFlags			= D3D11_BIND_INDEX_BUFFER;	// Tells Direct3D this is an index buffer
		ibd.CPUAccessFlags		= 0;	// Note: We cannot access the data from C++ (this is good)
		ibd.MiscFlags			= 0;
		ibd.StructureByteStride = 0;

		// Specify the initial data for this buffer, similar to above
		D3D11_SUBRESOURCE_DATA initialIndexData = {};
		initialIndexData.pSysMem = indices; // pSysMem = Pointer to System Memory

		// Actually create the buffer with the initial data
		// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
		device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
	}
	*/

	// Assignment 8
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tileSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tileSpecularSRV;
	// Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalSRV;
	// Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalSpecularSRV;
	// Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> asphaltSRV;
	// Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> asphaltSpecularSRV;
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/brokentiles.png").c_str(), 0, tileSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/brokentiles_specular.png").c_str(), 0, tileSpecularSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/metalfloor.png").c_str(), 0, metalSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/metalfloor_specular.png").c_str(), 0, metalSpecularSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/metalfloor_normals.png").c_str(), 0, metalNormalSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/cobblestone.png").c_str(), 0, asphaltSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/cobblestone_specular.png").c_str(), 0, asphaltSpecularSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/cobblestone_normals.png").c_str(), 0, asphaltNormalSRV.GetAddressOf());

	// AddressU, V, W should be something other than 0, but within 0 - 1 range

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter			= D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy	= 16; // value between 1 and 16 (higher = better but slower)
	samplerDesc.MaxLOD			= D3D11_FLOAT32_MAX;

	HRESULT samplerTest = this->device->CreateSamplerState(&samplerDesc, this->samplerState.GetAddressOf());


	materials.push_back(std::make_shared<Material>(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), this->vertexShader, this->pixelShader, 1.0f));
	materials.push_back(std::make_shared<Material>(XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), this->vertexShader, this->pixelShader, 1.0f));
	materials.push_back(std::make_shared<Material>(XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f), this->vertexShader, this->pixelShader, 1.0f));
	materials.push_back(std::make_shared<Material>(XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f), this->vertexShader, this->customPShader, 1.0f));

	materials.push_back(std::make_shared<Material>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), this->vertexShader, this->pixelShader, 0.5f));
	materials.push_back(std::make_shared<Material>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), this->vertexShader, this->pixelShader, 0.5f));

	// Assignment 8
	// set SRVs and samplers with SimpleShader
	// Asphalt Texture
	materials[4]->AddTextureSRV("SurfaceTexture", this->asphaltSRV);
	materials[4]->AddTextureSRV("SpecularMap", this->asphaltSpecularSRV);
	materials[4]->AddTextureSRV("NormalMap", this->asphaltNormalSRV);
	materials[4]->AddSampler("BasicSampler", this->samplerState);

	// Metal Texture
	materials[5]->AddTextureSRV("SurfaceTexture", this->metalSRV);
	materials[5]->AddTextureSRV("SpecularMap", this->metalSpecularSRV);
	materials[5]->AddTextureSRV("NormalMap", this->metalNormalSRV);
	materials[5]->AddSampler("BasicSampler", this->samplerState);

	/*
	// colors for meshes
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 yellow = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 gray = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	XMFLOAT4 white = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 pink = XMFLOAT4(1.0f, 0.0f, 0.5f, 1.0f);
	XMFLOAT4 purple = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f);

	Vertex vertices0[] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0)},
		{ XMFLOAT3(+0.5f, -0.5f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(-0.5f, -0.5f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) }
	};

	unsigned int indices0[] = { 0, 1, 2 };

	// meshes[0] = std::make_shared<Mesh>(vertices0, 3, indices0, 3, device);

	Vertex vertices1[] =
	{
		{ XMFLOAT3(-0.7f, +0.8f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(-0.75f, +0.75f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(-0.8f, +0.8f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(-0.75f, +0.85f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) }
	};

	unsigned int indices1[] = { 0, 1, 2, 2, 3, 0 };

	// meshes[1] = std::make_shared<Mesh>(vertices1, 6, indices1, 6, device);

	Vertex vertices2[] =
	{
		{ XMFLOAT3(+0.65f, -0.5f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(+0.7f, -0.6f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(+0.75f, -0.5f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(+0.6f, -0.6f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(+0.8f, -0.6f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(+0.7f, -0.9f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) }
	};

	unsigned int indices2[] = { 3, 0, 1, 1, 2, 4, 4, 5, 3 };

	// meshes[2] = std::make_shared<Mesh>(vertices2, 9, indices2, 9, device);

	// create entities
	std::shared_ptr<GameEntity> entity1 = std::make_shared<GameEntity>(meshes[0], materials[0]);
	std::shared_ptr<GameEntity> entity2 = std::make_shared<GameEntity>(meshes[0], materials[1]);
	std::shared_ptr<GameEntity> entity3 = std::make_shared<GameEntity>(meshes[1], materials[1]);
	std::shared_ptr<GameEntity> entity4 = std::make_shared<GameEntity>(meshes[1], materials[2]);
	std::shared_ptr<GameEntity> entity5 = std::make_shared<GameEntity>(meshes[2], materials[2]);

	// transform entities
	entity1->GetTransform()->Rotate(0.0f, 0.0f, 0.5f);
	entity2->GetTransform()->MoveAbsolute(-0.6f, -0.25f, 0.0f);
	entity3->GetTransform()->MoveAbsolute(0.5f, -1.1f, 0.0f);
	entity4->GetTransform()->Rotate(0.0f, 0.0f, 0.33f);
	entity5->GetTransform()->Scale(1.2f, 1.1f, 1.0f);

	entities.push_back(entity1);
	entities.push_back(entity2);
	entities.push_back(entity3);
	entities.push_back(entity4);
	entities.push_back(entity5);
	*/

	meshes[0] = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/quad.obj").c_str(), this->device);
	meshes[1] = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/torus.obj").c_str(), this->device);
	meshes[2] = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/helix.obj").c_str(), this->device);
	meshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/sphere.obj").c_str(), this->device));
	meshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/cylinder.obj").c_str(), this->device));
	meshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/cube.obj").c_str(), this->device));
	meshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/quad_double_sided.obj").c_str(), this->device));


	std::shared_ptr<GameEntity> entity1 = std::make_shared<GameEntity>(meshes[0], materials[5]);
	std::shared_ptr<GameEntity> entity2 = std::make_shared<GameEntity>(meshes[1], materials[4]);
	std::shared_ptr<GameEntity> entity3 = std::make_shared<GameEntity>(meshes[2], materials[4]);
	std::shared_ptr<GameEntity> entity4 = std::make_shared<GameEntity>(meshes[3], materials[5]);
	std::shared_ptr<GameEntity> entity5 = std::make_shared<GameEntity>(meshes[4], materials[4]);
	std::shared_ptr<GameEntity> entity6 = std::make_shared<GameEntity>(meshes[5], materials[5]);
	std::shared_ptr<GameEntity> entity7 = std::make_shared<GameEntity>(meshes[6], materials[4]);

	entity1->GetTransform()->MoveAbsolute(-12.0f, 0.0f, 0.0f);
	entity2->GetTransform()->MoveAbsolute(-8.0f, 0.0f, 0.0f);
	entity3->GetTransform()->MoveAbsolute(-4.0f, 0.0f, 0.0f);
	entity4->GetTransform()->MoveAbsolute(0.0f, 0.0f, 0.0f);
	entity5->GetTransform()->MoveAbsolute(4.0f, 0.0f, 0.0f);
	entity6->GetTransform()->MoveAbsolute(8.0f, 0.0f, 0.0f);
	entity6->GetTransform()->Rotate(0.0f, 2.4f, 0.5f);
	entity7->GetTransform()->MoveAbsolute(12.0f, 0.0f, 0.0f);

	entities.push_back(entity1);
	entities.push_back(entity2);
	entities.push_back(entity3);
	entities.push_back(entity4);
	entities.push_back(entity5);
	entities.push_back(entity6);
	entities.push_back(entity7);

	// Assignment 9
	this->skyMesh = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/quad.obj").c_str(), this->device);
	this->sky = std::make_shared<Sky>(meshes[1], this->samplerState, this->device, this->context, this->skyPixelShader, this->skyVertexShader,
		FixPath(L"../../Assets/Textures/Clouds Pink/right.png").c_str(), FixPath(L"../../Assets/Textures/Clouds Pink/left.png").c_str(), 
		FixPath(L"../../Assets/Textures/Clouds Pink/up.png").c_str(), FixPath(L"../../Assets/Textures/Clouds Pink/down.png").c_str(),
		FixPath(L"../../Assets/Textures/Clouds Pink/front.png").c_str(), FixPath(L"../../Assets/Textures/Clouds Pink/back.png").c_str());
}


// --------------------------------------------------------
// Handle resizing to match the new window size.
//  - DXCore needs to resize the back buffer
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	for (std::shared_ptr<Camera> c : cameras)
	{
		c->UpdateProjectionMatrix((float)this->windowWidth / this->windowHeight);
	}
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// feed fresh input data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)this->windowWidth;
	io.DisplaySize.y = (float)this->windowHeight;

	// reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// determine input capture
	Input& input = Input::GetInstance();
	input.SetKeyboardCapture(io.WantCaptureKeyboard);
	input.SetMouseCapture(io.WantCaptureMouse);

	// show demo window
	ImGui::ShowDemoWindow();

	// ADD ANY NEW IMGUI UI FEATURES HERE
	ImGui::Begin("Custom Window");
	ImGui::Text("Welcome to my custom window");

	if (ImGui::CollapsingHeader("Basic Info"))
	{
		ImGui::Text("Framerate: %f", io.Framerate);
		ImGui::Text("Window Dimensions:");
		ImGui::BulletText("X: %5.1f", io.DisplaySize.x);
		ImGui::BulletText("Y: %5.1f", io.DisplaySize.y);
	}

	// ImGui::ColorEdit4("Color Tint", &this->colorTint.x);
	// ImGui::DragFloat3("Offset", &this->offset.x);
	// only one tint for now. will change all meshes
	ImGui::ColorEdit4("Color Tint", &this->colorTint.x);

	if (ImGui::CollapsingHeader("Meshes"))
	{
		// creates color/offset options for each mesh
		// loops for each mesh
		for (int i = 0; i < meshes.size(); i++)
		{
			// unique labels are important to ensure only the individual editor is being changed
			ImGui::PushID(i);

			if (ImGui::TreeNode("Mesh Node", "Mesh #%i", i + 1))
			{
				// removed individual tint edit temporarily
				// ImGui::ColorEdit4("Color Tint", &this->colorTints[i].x);
				ImGui::Text("%d indices", meshes[i]->GetIndexCount());

				ImGui::TreePop();
			}
			ImGui::PopID();
		}

		// ImGui::TreePop();
		ImGui::Spacing();
	}

	if (ImGui::CollapsingHeader("Scene Entities"))
	{
		
		for (int i = 0; i < entities.size(); i++)
		{
			ImGui::PushID(i);

			std::shared_ptr<Transform> transform = entities[i]->GetTransform();
			XMFLOAT3 position = transform->GetPosition();
			XMFLOAT3 rotation = transform->GetPitchYawRoll();
			XMFLOAT3 scale = transform->GetScale();

			if (ImGui::TreeNode("Entity Node", "Entity #%i", i + 1))
			{
				if (ImGui::DragFloat3("Position", &position.x, 0.01f))
				{
					transform->SetPosition(position);
				}
				if (ImGui::DragFloat3("Rotation", &rotation.x, 0.01f))
				{
					transform->SetRotation(rotation);
				}
				if (ImGui::DragFloat3("Scale", &scale.x, 0.01f))
				{
					transform->SetScale(scale);
				}

				ImGui::TreePop();
			}
			ImGui::PopID();

			ImGui::Spacing();
		}

		// ImGui::TreePop();
	}

	if (ImGui::CollapsingHeader("Lights"))
	{
		/*
		if (ImGui::DragFloat3("Ambient", &ambientColor.x, 0.01f))
		{
			this->ambientColor = ambientColor;
		}
		*/

		ImGui::ColorEdit3("Ambient", &ambientColor.x);

		ImGui::Spacing();

		for (int i = 0; i < lights.size(); i++)
		{
			ImGui::PushID(i);

			XMFLOAT3 color = lights[i].Color;
			XMFLOAT3 position = lights[i].Position;
			XMFLOAT3 direction = lights[i].Direction;
			float intensity = lights[i].Intensity;
			float range = lights[i].Range;

			if (ImGui::TreeNode("Lights", "Light #%i", i + 1))
			{
				if (lights[i].Type == 0)
				{
					ImGui::Text("Light Type: Directional");
				}
				else if (lights[i].Type == 1)
				{
					ImGui::Text("Light Type: Point");
				}

				ImGui::Spacing();

				ImGui::ColorEdit3("Color Tint", &lights[i].Color.x);
				/*
				if (ImGui::ColorEdit3("Color", &color.x, 0.01f))
				{
					lights[i].Color = color;
				}
				*/
				if (lights[i].Type == 0)
				{
					if (ImGui::DragFloat3("Direction", &direction.x, 0.01f))
					{
						lights[i].Direction = direction;
					}
				}
				else if (lights[i].Type == 1)
				{
					if (ImGui::DragFloat3("Position", &position.x, 0.01f))
					{
						lights[i].Position = position;
					}
					if (ImGui::DragFloat("Range", &range, 0.01f))
					{
						lights[i].Range = range;
					}
				}
				if (ImGui::DragFloat("Intensity", &intensity, 0.01f))
				{
					lights[i].Intensity = intensity;
				}

				ImGui::TreePop();
			}
			ImGui::PopID();

			ImGui::Spacing();
		}

		// ImGui::TreePop();
	}

	if (ImGui::CollapsingHeader("Textures"))
	{
		ImGui::Text("Texture 1: Asphalt");
		ImGui::Spacing();

		ImGui::Image(this->asphaltSRV.Get(), ImVec2(200.0f, 200.0f));

		ImGui::Spacing();

		ImGui::Text("Texture 2: Metal");
		ImGui::Spacing();

		ImGui::Image(this->metalSRV.Get(), ImVec2(200.0f, 200.0f));

		if (ImGui::DragFloat("Roughness", &this->roughness, 0.01f))
		{
			if (this->roughness < 0.00f)
			{
				this->roughness = 0.0f;
			}
			else if (this->roughness > 0.99f)
			{
				this->roughness = 0.99f;
			}
			materials[4]->SetRoughness(this->roughness);
			materials[5]->SetRoughness(this->roughness);
		}
	}

	if (ImGui::CollapsingHeader("Cameras"))
	{
		if (ImGui::TreeNode("Movement Controls"))
		{
			ImGui::BulletText("W, A, S, D = Forward, Left, Back, Right");
			ImGui::BulletText("Space, X = Up, Down");
			ImGui::BulletText("Hold Left-Click to look around");
			ImGui::BulletText("Hold LShift to double movement speed");

			ImGui::TreePop();
		}

		if (ImGui::SmallButton("Previous Camera"))
		{
			if (this->activeCamera != 0)
			{
				this->activeCamera--;
			}
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Next Camera"))
		{
			if (this->activeCamera != this->cameras.size() - 1)
			{
				this->activeCamera++;
			}
		}

		ImGui::Spacing();

		ImGui::Text("Active Camera: #%i", this->activeCamera);
		ImGui::Spacing();
		ImGui::Text("Current FOV: %f", this->cameras[activeCamera]->GetFOV());
		ImGui::Spacing();

		ImGui::Text("Position: X: %f", this->cameras[activeCamera]->GetTransform().GetPosition().x);
		ImGui::SameLine();
		ImGui::Text(" Y: %f", this->cameras[activeCamera]->GetTransform().GetPosition().y);
		ImGui::SameLine();
		ImGui::Text(" Z: %f", this->cameras[activeCamera]->GetTransform().GetPosition().z);

		ImGui::Spacing();
		ImGui::Text("Rotation: Roll: %f", this->cameras[activeCamera]->GetTransform().GetPitchYawRoll().x);
		ImGui::SameLine();
		ImGui::Text(" Pitch: %f", this->cameras[activeCamera]->GetTransform().GetPitchYawRoll().y);
		ImGui::SameLine();
		ImGui::Text(" Yaw: %f", this->cameras[activeCamera]->GetTransform().GetPitchYawRoll().z);
	}

	/*
	ImGui::Text("Mesh #1");
	ImGui::ColorEdit4("Color Tint##1", &this->colorTints[0].x);
	ImGui::DragFloat3("Offset##1", &this->offsets[0].x);

	ImGui::Text("Mesh #2");
	ImGui::ColorEdit4("Color Tint##2", &this->colorTints[1].x);
	ImGui::DragFloat3("Offset##2", &this->offsets[1].x);

	ImGui::Text("Mesh #3");
	ImGui::ColorEdit4("Color Tint##3", &this->colorTints[2].x);
	ImGui::DragFloat3("Offset##3", &this->offsets[2].x);
	*/

	ImGui::End();

	// move all entities
	for (std::shared_ptr<GameEntity> g : entities)
	{
		std::shared_ptr<Transform> transform = g->GetTransform();
		// transform->Rotate(0.0f, 0.0f, -deltaTime * 0.5f);
	}
	std::shared_ptr<Transform> helixTransform = entities[2]->GetTransform();
	helixTransform->Rotate(0.0f, -deltaTime * 0.5f, 0.0f);

	std::shared_ptr<Transform> cubeTransform = entities[5]->GetTransform();
	cubeTransform->Rotate(0.0f, deltaTime * 0.75f, 0.0f);

	std::shared_ptr<Transform> sphereTransform = entities[3]->GetTransform();
	sphereTransform->Rotate(0.0f, deltaTime, 0.0f);

	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();

	this->cameras[activeCamera]->Update(deltaTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// VertexShaderExternalData vsData;
	// vsData.colorTint = XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f);
	// vsData.offset = XMFLOAT3(0.25f, 0.0f, 0.0f);

	// temporary comments here
	// next assignment lets us set up architecture to give each mesh own color/offset
	// the three commented lines below work, but are commented to do the above for now
	// D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	// context->Map(vsConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	// memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));
	// context->Unmap(vsConstantBuffer.Get(), 0);
	
	/*
	* Commented out for Assingment 6
	context->VSSetConstantBuffers(
		0, // Which slot (register) to bind the buffer to?
		1, // How many are we activating? Can do multiple at once
		vsConstantBuffer.GetAddressOf()); // Array of buffers (or the address of one)
	*/

	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erases what's on the screen)
		const float bgColor[4] = { 0.4f, 0.6f, 0.75f, 1.0f }; // Cornflower Blue
		context->ClearRenderTargetView(backBufferRTV.Get(), bgColor);

		// Clear the depth buffer (resets per-pixel occlusion information)
		context->ClearDepthStencilView(depthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	// DRAW geometry
	// - These steps are generally repeated for EACH object you draw
	// - Other Direct3D calls will also be necessary to do more complex things
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	{
		/*
		// Set buffers in the input assembler (IA) stage
		//  - Do this ONCE PER OBJECT, since each object may have different geometry
		//  - For this demo, this step *could* simply be done once during Init()
		//  - However, this needs to be done between EACH DrawIndexed() call
		//     when drawing different geometry, so it's here as an example
		context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		// Tell Direct3D to draw
		//  - Begins the rendering pipeline on the GPU
		//  - Do this ONCE PER OBJECT you intend to draw
		//  - This will use all currently set Direct3D resources (shaders, buffers, etc)
		//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
		//     vertices in the currently set VERTEX BUFFER
		context->DrawIndexed(
			3,     // The number of indices to use (we could draw a subset if we wanted)
			0,     // Offset to the first index we want to use
			0);    // Offset to add to each index when looking up vertices
		*/

		// temporary int here
		// next assignment lets us set up architecture to give each mesh own color/offset
		/*
		int i = 0;
		for (std::shared_ptr<Mesh> m : meshes)
		{
			// set vertex shader data to customized tint/offset
			vsData.colorTint = this->colorTints[i];
			vsData.offset = this->offsets[i];

			// input new vertex shader data so only the individual mesh is changed
			context->Map(vsConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
			memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));
			context->Unmap(vsConstantBuffer.Get(), 0);

			m->Draw(deltaTime, totalTime);

			i++;
		}
		*/

		// materials[3]->GetPixelShader()->SetFloat("time", deltaTime);

		if (timerUp)
		{
			timer += deltaTime;
			materials[3]->GetPixelShader()->SetFloat("timer", timer);
			if (timer >= 1.5f)
			{
				timerUp = false;
			}
		}
		else
		{
			timer -= deltaTime;
			materials[3]->GetPixelShader()->SetFloat("timer", timer);
			if (timer <= 0.0f)
			{
				timerUp = true;
			}
		}
		
		materials[3]->GetPixelShader()->SetFloat("time", totalTime);

		// assignment 7
		// materials[4]->PrepareMaterial(materials[4]->GetRoughness(), this->cameras[activeCamera]->GetTransform().GetPosition());
		// materials[5]->PrepareMaterial(materials[4]->GetRoughness(), this->cameras[activeCamera]->GetTransform().GetPosition());
		// Light (Task 6) (Directional Light)
		// materials[4]->GetPixelShader()->SetData("directionalLight1", &lights[0], sizeof(Light));
		// More Directional Lights (Task 8)
		// materials[4]->GetPixelShader()->SetData("directionalLight2", &lights[1], sizeof(Light));
		// materials[4]->GetPixelShader()->SetData("directionalLight3", &lights[2], sizeof(Light));
		// Point Lights (Task 9)
		// materials[4]->GetPixelShader()->SetData("pointLight1", &lights[3], sizeof(Light));
		// materials[4]->GetPixelShader()->SetData("pointLight2", &lights[4], sizeof(Light));
		// materials[5]->PrepareMaterial(materials[5]->GetRoughness(), this->cameras[activeCamera]->GetTransform().GetPosition());

		// materials[4]->GetPixelShader()->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());

		
		// materials[5]->GetPixelShader()->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());

		for (std::shared_ptr<GameEntity> g : entities)
		{
			// g->GetMaterial()->PrepareMaterial(g->GetMaterial()->GetRoughness(), this->cameras[activeCamera]->GetTransform().GetPosition());
			g->GetMaterial()->GetPixelShader()->SetFloat3("ambient", this->ambientColor);
			g->GetMaterial()->GetPixelShader()->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());
		}

		// assignment 4
		for (std::shared_ptr<GameEntity> g : entities)
		{
			g->Draw(context, this->colorTint, this->cameras[activeCamera]);
		}

		// Assignment 9
		this->sky->Draw(this->cameras[activeCamera]);

	}

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present the back buffer to the user
		//  - Puts the results of what we've drawn onto the window
		//  - Without this, the user never sees anything
		bool vsyncNecessary = vsync || !deviceSupportsTearing || isFullscreen;

		// ImGui
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		swapChain->Present(
			vsyncNecessary ? 1 : 0,
			vsyncNecessary ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Must re-bind buffers after presenting, as they become unbound
		context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthBufferDSV.Get());
	}
}