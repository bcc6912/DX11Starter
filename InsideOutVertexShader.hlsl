#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float outlineSize;
}

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

	float3 worldPos = mul(world, float4(input.localPosition, 1.0f)).xyz;
	float3 worldNormal = normalize(mul((float3x3)world, input.normal));

	worldPos += worldNormal * outlineSize;

	matrix vp = mul(projection, view);
	output.screenPosition = mul(vp, float4(worldPos, 1.0f));

	return output;
}