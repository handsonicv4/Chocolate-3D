#include "../ConstantBuffers.hlsl"
#include "TypeDef.hlsl"

StructuredBuffer<float4x4> boneMatrix : register(t5);
StructuredBuffer<InstanceType> instanceData : register(t6);

float4 BoneTransformPos(uniform float4 vertexPos, uniform float4 weight, uniform uint4 boneID)
{
	float4 result = float4(0, 0, 0, 0);
	for (int i = 0; i < 4; i++)
	{
		if (weight[i] > 0.001)
		{
			result += weight[i] * mul(vertexPos, boneMatrix[boneID[i]]);
		}
	}
	return result;
}


float3 BoneTransformNorm(uniform float3 vertexNormal, uniform float4 weight, uniform uint4 boneID)
{
	float3 result = float3(0, 0, 0);

	for (int i = 0; i < 4; i++)
	{
		if (weight[i] != 0)
		{
			result += weight[i] * normalize(mul(vertexNormal, (float3x3)boneMatrix[boneID[i]]));
		}

	}
	return result;
}


PSinput main(VSinput input)
{
	PSinput output;
    // Change the position vector to be 4 units for proper matrix calculations.
	float4 position = float4(input.position, 1.0f);
	output.position= float4(input.position, 1.0f);
	output.normal = input.normal;
	output.tangent = input.tangent;
	output.bitangent = input.bitangent;
	//Skinning:
	if (g_HasAnimation)
	{
		uint bindOffset = instanceData[input.instanceID].bindMatrixOffset;
		uint4 b=input.boneID + bindOffset;

		output.position = BoneTransformPos(position, input.boneWeight, b);
		output.normal = BoneTransformNorm(input.normal, input.boneWeight, b);

		if (g_HasNormalMap)
		{
			output.tangent = BoneTransformNorm(input.tangent, input.boneWeight, b);
			output.bitangent = BoneTransformNorm(input.bitangent, input.boneWeight, b);
		}
	}
	output.positionWorld = mul(output.position, instanceData[input.instanceID].world);

	float4 hVoxScale = float4( 0.5*g_VoxelDimention*g_VoxelSize,1);
	//To clip space -1~1
	output.position = output.positionWorld / 5.0;

	output.normal = normalize( mul(output.normal, (float3x3)instanceData[input.instanceID].world));
	output.tangent= normalize(mul(output.tangent, (float3x3)instanceData[input.instanceID].world));
	output.bitangent = normalize(mul(output.bitangent, (float3x3)instanceData[input.instanceID].world));
	output.tex = input.tex;
	output.instanceMaterialID = instanceData[input.instanceID].instanceMaterialID;
    return output;
}