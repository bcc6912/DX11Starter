#include "ShaderIncludes.hlsli"

Texture2D Albedo			: register(t0);
Texture2D NormalMap			: register(t1);
Texture2D RoughnessMap		: register(t2);
Texture2D ShadowMap			: register(t3);
Texture2D CelShadeRamp		: register(t4);
Texture2D CelShadeSpecular	: register(t5);
SamplerState BasicSampler	: register(s0);
SamplerState ClampSampler	: register(s1);
SamplerComparisonState ShadowSampler : register(s2);

cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	// float roughness;
	float3 cameraPosition;
	int gammaCorrection;
	int usingAlbedo;
	int usingSpecularMap;
	float3 ambient;

	Light lights[5]; // 3 directional, 2 point IN THAT ORDER; MUST BE EXACT
}

float4 main(VertexToPixel input) : SV_TARGET
{
	input.shadowMapPos /= input.shadowMapPos.w;

	float2 shadowUV = input.shadowMapPos.xy * 0.5f + 0.5f;
	shadowUV.y = 1 - shadowUV.y;

	float distToLight = input.shadowMapPos.z;

	float shadowAmount = ShadowMap.SampleCmpLevelZero(ShadowSampler, shadowUV, distToLight).r;

	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	// Specular Reflections (Task 7)
	float3 viewVector = normalize(cameraPosition - input.worldPosition); // V

	// function normalizes input.normal and input.tangent
	input.normal = NormalMapping(NormalMap, BasicSampler, input.uv, input.normal, input.tangent);

	float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;

	float specularScale = 1.0f;
	if (usingSpecularMap)
	{
		specularScale = CelShadeSpecular.Sample(BasicSampler, input.uv).r;
	}

	float4 surfaceColor = Albedo.Sample(BasicSampler, input.uv);
	// surfaceColor.rgb = gammaCorrection 
	if (gammaCorrection == 1)
	{
		surfaceColor.rgb = pow(surfaceColor.rgb, 2.2f);
	}
	else
	{
		surfaceColor.rgb = surfaceColor.rgb;
	}

	if (usingAlbedo == 0)
	{
		surfaceColor.rgb = colorTint.rgb;
	}

	// float3 finalColor = surfaceColor.rgb * ambient;
	float3 finalColor = float3(0.0f, 0.0f, 0.0f);

	float3 lightResult = DirectionalLightCelShading(lights[0], input.normal, viewVector, roughness, surfaceColor, specularScale, CelShadeRamp, CelShadeSpecular, ClampSampler);
	lightResult *= shadowAmount;

	finalColor += lightResult;
	// finalColor += DirectionalLightPBR(lights[0], input.normal, viewVector, roughness, metalness, surfaceColor.rgb, specularColor);
	finalColor += DirectionalLightCelShading(lights[1], input.normal, viewVector, roughness, surfaceColor, specularScale, CelShadeRamp, CelShadeSpecular, ClampSampler);
	finalColor += DirectionalLightCelShading(lights[2], input.normal, viewVector, roughness, surfaceColor, specularScale, CelShadeRamp, CelShadeSpecular, ClampSampler);
	finalColor += PointLightCelShading(lights[3], input.normal, input.worldPosition, viewVector, roughness, surfaceColor, specularScale, CelShadeRamp, CelShadeSpecular, ClampSampler);
	finalColor += PointLightCelShading(lights[4], input.normal, input.worldPosition, viewVector, roughness, surfaceColor, specularScale, CelShadeRamp, CelShadeSpecular, ClampSampler);

	if (gammaCorrection)
	{
		return float4(pow(finalColor, (1.0f / 2.2f)), 1.0f);
	}
	else
	{
		return float4(finalColor, 1.0f);
	}

}