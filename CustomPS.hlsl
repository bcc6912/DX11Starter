cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	float time;
	float timer;
}

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

float4 main(VertexToPixel input) : SV_TARGET
{
	float timeMultiplier = 0.5f;

	float sine = sin(input.uv.x * 10.0f + timer * 3.5f + sin(time)) * 1.5f;

	// noise code from Assignment 6
	float2 random = frac(sin(dot(float2(input.uv.x * (time * timeMultiplier), input.uv.y * (time * timeMultiplier)), float2(12.9898, 78.233))) * 43758.5453123);

	return float4(random.x + colorTint.x + sine, random.y + colorTint.y + sine, colorTint.z, 1.0f);
}