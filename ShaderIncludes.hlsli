#ifndef ShaderIncludes // Each .hlsli file needs a unique identifier!
#define ShaderIncludes

// Assignment 7
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
#define MAX_SPECULAR_EXPONENT 256.0f

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 localPosition	: POSITION;     // XYZ position
	// float4 color			: COLOR;        // RGBA color
	float3 normal			: NORMAL;
	float2 uv				: TEXCOORD;
};

// ALL of your code pieces (structs, functions, etc.) go here!
// Struct representing the data we're sending down the pipeline
// - Should match our pixel shader's input (hence the name: Vertex to Pixel)
// - At a minimum, we need a piece of data defined tagged as SV_POSITION
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 screenPosition	: SV_POSITION;	// XYZW position (System Value Position)
	// float4 color			: COLOR;        // RGBA color
	float3 normal			: NORMAL;
	float2 uv				: TEXCOORD;
	float3 worldPosition	: POSITION;
};

struct Light
{
	int Type;						// 0 (directional), 1 (point), 2 (spot)
	float3 Direction;	// Directional and Spot lights need direction
	float Range;					// Point and Spot lights have a max range for attenuation
	float3 Position;		// Point and Spot lights have a position in space
	float Intensity;				// all lights need an intensity
	float3 Color;		// all lights need a color
	float SpotFalloff;				// Spot lights need a value to define their "cone" size
	float3 Padding;		// purposefully padding to hit the 16-byte boundary
};

// make sure dirToLight is normalized
// SHOULD BE INVERSE OF LIGHT DIRECTION
float3 DiffuseBRDF(float3 normal, float3 dirToLight)
{
	return saturate(dot(normal, dirToLight));
}

// make sure dirFromLight is normalized
// calculate viewVector using : normalize(cameraPosition - input.worldPosition)
float SpecularBRDF(float roughness, float3 normal, float3 dirFromLight, float3 viewVector)
{
	float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
	// viewVector = V
	float3 reflection = reflect(dirFromLight * -1.0f, normal); // R
	float specular = pow(saturate(dot(reflection, viewVector)), specExponent);
	return specular;
}

float Attenuate(Light light, float3 worldPosition)
{
	float dist = distance(light.Position, worldPosition);
	float attenuation = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
	return attenuation * attenuation;
}

// calculate viewVector using : normalize(cameraPosition - input.worldPosition)
// surfaceColor should be a float3 of colorTint
float3 DirectionalLight(Light light, float3 normal, float3 viewVector, float roughness, float3 surfaceColor)
{
	float3 direction = light.Direction * -1.0f;
	float3 diffuse = DiffuseBRDF(normal, normalize(direction));

	float specular = SpecularBRDF(roughness, normal, normalize(direction), viewVector);

	return ((surfaceColor * diffuse) + specular) * light.Intensity * light.Color;
}

// calculate viewVector using : normalize(cameraPosition - input.worldPosition)
// surfaceColor should be a float3 of colorTint
float3 PointLight(Light light, float3 normal, float3 worldPosition, float3 viewVector, float roughness, float3 surfaceColor)
{
	float3 pointDirection = normalize(light.Position - worldPosition);

	float3 diffuse = DiffuseBRDF(normal, normalize(pointDirection));

	float specular = SpecularBRDF(roughness, normal, normalize(pointDirection), viewVector);

	float attenuation = Attenuate(light, worldPosition);

	return ((surfaceColor * diffuse) + specular) * light.Intensity * light.Color * attenuation;
}

#endif