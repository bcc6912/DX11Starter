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
	float3 tangent			: TANGENT;
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
	float2 uv				: TEXCOORD;
	float3 normal			: NORMAL;
	float3 worldPosition	: POSITION;
	float3 tangent			: TANGENT;
};

// Assignment 9
struct VertexToPixelSky
{
	float4 screenPosition	: SV_POSITION;
	float3 sampleDir		: DIRECTION;
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
float3 DirectionalLight(Light light, float3 normal, float3 viewVector, float roughness, float3 surfaceColor, float specularScale)
{
	float3 direction = light.Direction * -1.0f;
	float3 diffuse = DiffuseBRDF(normal, normalize(direction));

	float specular = SpecularBRDF(roughness, normal, normalize(direction), viewVector) * specularScale;

	specular *= any(diffuse);

	return ((surfaceColor * diffuse) + specular) * light.Intensity * light.Color;
}

// calculate viewVector using : normalize(cameraPosition - input.worldPosition)
// surfaceColor should be a float3 of colorTint
float3 PointLight(Light light, float3 normal, float3 worldPosition, float3 viewVector, float roughness, float3 surfaceColor, float specularScale)
{
	float3 pointDirection = normalize(light.Position - worldPosition);

	float3 diffuse = DiffuseBRDF(normal, normalize(pointDirection));

	float specular = SpecularBRDF(roughness, normal, normalize(pointDirection), viewVector) * specularScale;

	specular *= any(diffuse);

	float attenuation = Attenuate(light, worldPosition);

	return ((surfaceColor * diffuse) + specular) * light.Intensity * light.Color * attenuation;
}

// normal and tangent get normalized when using this
float3 NormalMapping(Texture2D normalMap, SamplerState basicSampler, float2 uv, float3 normal, float3 tangent)
{
	float3 unpackedNormal = normalMap.Sample(basicSampler, uv).rgb * 2.0f - 1.0f;
	unpackedNormal = normalize(unpackedNormal);

	// TBN Matrix
	float3 N = normalize(normal);
	float3 T = normalize(tangent);
	T = normalize(T - N * dot(T, N));
	float3 B = cross(T, N);
	float3x3 TBN = float3x3(T, B, N);

	return normalize(mul(unpackedNormal, TBN));
}


// CONSTANTS ===================

// Make sure to place these at the top of your shader(s) or shader include file
// - You don't necessarily have to keep all the comments; they're here for your reference

// The fresnel value for non-metals (dielectrics)
// Page 9: "F0 of nonmetals is now a constant 0.04"
// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
static const float F0_NON_METAL = 0.04f;

// Minimum roughness for when spec distribution function denominator goes to zero
static const float MIN_ROUGHNESS = 0.0000001f; // 6 zeros after decimal

// Handy to have this as a constant
static const float PI = 3.14159265359f;

// PBR FUNCTIONS ================

// Lambert diffuse BRDF - Same as the basic lighting diffuse calculation!
// - NOTE: this function assumes the vectors are already NORMALIZED!
float DiffusePBR(float3 normal, float3 dirToLight)
{
	return saturate(dot(normal, dirToLight));
}

// Calculates diffuse amount based on energy conservation
//
// diffuse   - Diffuse amount
// F         - Fresnel result from microfacet BRDF
// metalness - surface metalness amount 
float3 DiffuseEnergyConserve(float3 diffuse, float3 F, float metalness)
{
	return diffuse * (1 - F) * (1 - metalness);
}

// Normal Distribution Function: GGX (Trowbridge-Reitz)
//
// a - Roughness
// h - Half vector: (V + L)/2
// n - Normal
// 
// D(h, n, a) = a^2 / pi * ((n dot h)^2 * (a^2 - 1) + 1)^2
float D_GGX(float3 n, float3 h, float roughness)
{
	// Pre-calculations
	float NdotH = saturate(dot(n, h));
	float NdotH2 = NdotH * NdotH;
	float a = roughness * roughness;
	float a2 = max(a * a, MIN_ROUGHNESS); // Applied after remap!

	// ((n dot h)^2 * (a^2 - 1) + 1) 
	// Can go to zero if roughness is 0 and NdotH is 1; MIN_ROUGHNESS helps here
	float denomToSquare = NdotH2 * (a2 - 1) + 1;

	// Final value
	return a2 / (PI * denomToSquare * denomToSquare);
}

// Fresnel term - Schlick approx.
// 
// v - View vector
// h - Half vector
// f0 - Value when l = n
//
// F(v,h,f0) = f0 + (1-f0)(1 - (v dot h))^5
float3 Fresnel(float3 v, float3 h, float3 f0)
{
	// Pre-calculations
	float VdotH = saturate(dot(v, h));

	// Final value
	return f0 + (1 - f0) * pow(1 - VdotH, 5);
}

// Geometric Shadowing - Schlick-GGX
// - k is remapped to a / 2, roughness remapped to (r+1)/2 before squaring!
//
// n - Normal
// v - View vector
//
// G_Schlick(n,v,a) = (n dot v) / ((n dot v) * (1 - k) * k)
//
// Full G(n,v,l,a) term = G_SchlickGGX(n,v,a) * G_SchlickGGX(n,l,a)
float G_SchlickGGX(float3 n, float3 v, float roughness)
{
	// End result of remapping:
	float k = pow(roughness + 1, 2) / 8.0f;
	float NdotV = saturate(dot(n, v));

	// Final value
	// Note: Numerator should be NdotV (or NdotL depending on parameters).
	// However, these are also in the BRDF's denominator, so they'll cancel!
	// We're leaving them out here AND in the BRDF function as the
	// dot products can get VERY small and cause rounding errors.
	return /*NdotV*/ 1 / (NdotV * (1 - k) + k);
}

// Cook-Torrance Microfacet BRDF (Specular)
//
// f(l,v) = D(h)F(v,h)G(l,v,h) / 4(n dot l)(n dot v)
// - parts of the denominator are canceled out by numerator (see below)
//
// D() - Normal Distribution Function - Trowbridge-Reitz (GGX)
// F() - Fresnel - Schlick approx
// G() - Geometric Shadowing - Schlick-GGX
float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 specColor, out float3 F_out)
{
	// Other vectors
	float3 h = normalize(v + l); // That's an L, not a 1! Careful copy/pasting from a PDF!

	// Run numerator functions
	float  D = D_GGX(n, h, roughness);
	float3 F = Fresnel(v, h, specColor);
	float  G = G_SchlickGGX(n, v, roughness) * G_SchlickGGX(n, l, roughness);

	// Pass F out of the function for diffuse balance
	F_out = F;

	// Final specular formula
	// Note: The denominator SHOULD contain (NdotV)(NdotL), but they'd be
	// canceled out by our G() term.  As such, they have been removed
	// from BOTH places to prevent floating point rounding errors.
	float3 specularResult = (D * F * G) / 4;

	// One last non-obvious requirement: According to the rendering equation,
	// specular must have the same NdotL applied as diffuse!  We'll apply
	// that here so that minimal changes are required elsewhere.
	return specularResult * max(dot(n, l), 0);
}

// Assingment 10

// calculate viewVector using : normalize(cameraPosition - input.worldPosition) in pixel shader
float3 DirectionalLightPBR(Light light, float3 normal, float3 viewVector, float roughness, float metalness, float3 surfaceColor, float3 specularColor)
{
	float3 direction = normalize(light.Direction * -1.0f);

	float3 diffuse = DiffusePBR(normal, direction);

	float3 F;

	float3 specular = MicrofacetBRDF(normal, direction, viewVector, roughness, specularColor, F);

	float3 balancedDiffuse = DiffuseEnergyConserve(diffuse, specular, metalness);

	return ((surfaceColor * balancedDiffuse) + specular) * light.Intensity * light.Color;
}

// calculate viewVector using : normalize(cameraPosition - input.worldPosition) in pixel shader
float3 PointLightPBR(Light light, float3 normal, float3 worldPosition, float3 viewVector, float roughness, float metalness, float3 surfaceColor, float3 specularColor)
{
	float3 pointDirection = normalize(light.Position - worldPosition);

	float3 diffuse = DiffusePBR(normal, pointDirection);

	float3 F;

	float3 specular = MicrofacetBRDF(normal, pointDirection, viewVector, roughness, specularColor, F);

	float attenuation = Attenuate(light, worldPosition);

	float3 balancedDiffuse = DiffuseEnergyConserve(diffuse, specular, metalness);

	return ((surfaceColor * balancedDiffuse) + specular) * light.Intensity * light.Color * attenuation;
}

#endif