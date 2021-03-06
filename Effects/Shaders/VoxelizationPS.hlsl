#include "PixelShaderBase.hlsl"

RWTexture3D<float4> voxels:register(u4);

bool IsInsideScene(const float3 pos)
{
	float3 bound = 0.5*g_VoxelDimention*g_VoxelSize;
	return abs(pos.x) < bound.x && abs(pos.y) < bound.y && abs(pos.z) < bound.z;
}

uint3 WorldToVoxelPosition(float3 pos)
{
	return (pos + 0.5*g_VoxelDimention.x*g_VoxelSize.x)/g_VoxelSize.x;
}

float4 main(PSinput input) : SV_TARGET
{
	if (!IsInsideScene(input.positionWorld.xyz))
	{
		discard;
	}

	Pixel p = GetPixel(input);

	if (p.opacity < 0.05)
		discard;

	float3 color = p.emissivity*p.emissiveColor;
	if (!IsInShadow(input.positionLight, 0.0001f))
	{
		color = Diffuse(p)*p.diffuseColor + color;
	}

	float4 result = float4(color, p.opacity);

	uint3 vposition = WorldToVoxelPosition(p.position);
	voxels[vposition] = result;
	discard;
	return result;
}