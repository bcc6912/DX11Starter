#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
	matrix view;
	matrix projection;
}

VertexToPixelSky main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixelSky output;

	matrix viewNoTranslation = view;
	viewNoTranslation._14 = 0;
	viewNoTranslation._24 = 0;
	viewNoTranslation._34 = 0;

	matrix vp = mul(projection, viewNoTranslation);
	output.screenPosition = mul(vp, float4(input.localPosition, 1.0f));

	output.screenPosition.z = output.screenPosition.w;

	output.sampleDir = input.localPosition;

	return output;
}