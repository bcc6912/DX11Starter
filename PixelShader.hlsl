#include "ShaderIncludes.hlsli"

Texture2D SurfaceTexture	: register(t0);
Texture2D SpecularMap		: register(t1);
Texture2D NormalMap			: register(t2);
SamplerState BasicSampler	: register(s0);

cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	float roughness;
	float3 cameraPosition;
	float3 ambient;
	// Light directionalLight1;
	// Light directionalLight2;
	// Light directionalLight3;
	// Light pointLight1;
	// Light pointLight2;
	int usingSpecularMap;

	Light lights[5]; // 3 directional, 2 point IN THAT ORDER; MUST BE EXACT
}

/*
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 screenPosition	: SV_POSITION;
	// float4 color			: COLOR;
	float3 normal			: NORMAL;
	float2 uv				: TEXCOORD;
};
*/

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering

	// return colorTint;

	// Assignment 8 and 10
	float3 surfaceColor = pow(SurfaceTexture.Sample(BasicSampler, input.uv).rgb, 2.2f) * (float3)colorTint;
	
	float specularScale = 1.0f;
	if (usingSpecularMap)
	{
		specularScale = SpecularMap.Sample(BasicSampler, input.uv).r;
	}

	// Assignment 7
	// input.normal = normalize(input.normal);

	// Specular Reflections (Task 7)
	float3 viewVector = normalize(cameraPosition - input.worldPosition); // V

	// Below commented out because the directional light is now calculated in CalculateLightColor helper method
	/*
	float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
	float3 viewVector = normalize(cameraPosition - input.worldPosition); // V
	float3 reflection = reflect(normalizedDirToLight, input.normal); // R
	float spec = pow(saturate(dot(reflection, viewVector)), specExponent);
	*/

	// directionalLight1
	// float3 dirLight1Color = CalculateLightColor(directionalLight1.Color, directionalLight1.Direction, input.normal, viewVector, roughness);
	// float3 dirLight1Color = DirectionalLight(lights[0], input.normal, viewVector, roughness, (float3)colorTint);

	// directionalLight2
	// float3 dirLight2Color = CalculateLightColor(directionalLight2.Color, directionalLight2.Direction, input.normal, viewVector, roughness);
	// float3 dirLight2Color = DirectionalLight(lights[1], input.normal, viewVector, roughness, (float3)colorTint);

	// directionalLight3
	// float3 dirLight3Color = CalculateLightColor(directionalLight3.Color, directionalLight3.Direction, input.normal, viewVector, roughness);
	// float3 dirLight3Color = DirectionalLight(lights[2], input.normal, viewVector, roughness, (float3)colorTint);

	// pointLight1
	// float3 pointLight1Color = CalculateLightColor(pointLight1.Color, CalculatePointLightDirection(pointLight1, input.worldPosition), input.normal, viewVector, roughness);
	// pointLight1Color = pointLight1Color * Attenuate(pointLight1, input.worldPosition);
	// float3 pointLight1Color = PointLight(lights[3], input.normal, input.worldPosition, viewVector, roughness, (float3)colorTint);

	// pointLight2
	// float3 pointLight2Color = CalculateLightColor(pointLight2.Color, CalculatePointLightDirection(pointLight2, input.worldPosition), input.normal, viewVector, roughness);
	// pointLight2Color = pointLight2Color * Attenuate(pointLight2, input.worldPosition);
	// float3 pointLight2Color = PointLight(lights[4], input.normal, input.worldPosition, viewVector, roughness, (float3)colorTint);

	// Assignment 9
	/*
	float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1;
	unpackedNormal = normalize(unpackedNormal);

	// TBN Matrix
	float3 N = normalize(input.normal);
	float3 T = normalize(input.tangent);
	T = normalize(T - N * dot(T, N));
	float3 B = cross(T, N);
	float3x3 TBN = float3x3(T, B, N);
	*/

	input.normal = NormalMapping(NormalMap, BasicSampler, input.uv, input.normal, input.tangent);

	float3 finalColor = ambient * surfaceColor;
	// finalColor += (directionalLight1.Color * DiffuseBRDF(input.normal, normalizedDirToLight));
	// finalColor += specular;
	// finalColor += dirLight1Color;
	// finalColor += dirLight2Color;
	// finalColor += dirLight3Color;
	// finalColor += pointLight1Color;
	// finalColor += pointLight2Color;
	finalColor += DirectionalLight(lights[0], input.normal, viewVector, roughness, surfaceColor, specularScale);
	finalColor += DirectionalLight(lights[1], input.normal, viewVector, roughness, surfaceColor, specularScale);
	finalColor += DirectionalLight(lights[2], input.normal, viewVector, roughness, surfaceColor, specularScale);
	finalColor += PointLight(lights[3], input.normal, input.worldPosition, viewVector, roughness, surfaceColor, specularScale);
	finalColor += PointLight(lights[4], input.normal, input.worldPosition, viewVector, roughness, surfaceColor, specularScale);

	return float4(pow(finalColor, (1.0f / 2.2f)), 1.0f);
}