#pragma once

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

#include <DirectXMath.h>

struct Light
{
	int Type;						// 0 (directional), 1 (point), 2 (spot)
	DirectX::XMFLOAT3 Direction;	// Directional and Spot lights need direction
	float Range;					// Point and Spot lights have a max range for attenuation
	DirectX::XMFLOAT3 Position;		// Point and Spot lights have a position in space
	float Intensity;				// all lights need an intensity
	DirectX::XMFLOAT3 Color;		// all lights need a color
	float SpotFalloff;				// Spot lights need a value to define their "cone" size
	DirectX::XMFLOAT3 Padding;		// purposefully padding to hit the 16-byte boundary
};