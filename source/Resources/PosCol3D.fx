//------------------------------------------
//	Global Variables
//------------------------------------------
float4x4 gWorldViewProj : WorldViewProjection;

Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;

float3 gLightDirection : LightDirection;
float4x4 gWorldMatrix : WorldMatrix;
float3 gCameraPostition : CameraPosition;
float gLightIntensity : LightIntensity;
float gShininess : Shininess;

//------------------------------------------
//	Sampler State
//------------------------------------------
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap; //or Mirror, Clamp, Border
    AddressV = Wrap; //or Mirror, Clamp, Border
};

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};

//------------------------------------------
//	Input/Output Structs
//------------------------------------------
struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Color : COLOR;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float3 Color : COLOR;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

//------------------------------------------
//	Vertex Shader
//------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
	output.Color = input.Color;
    output.UV = input.UV;
	return output;
}

//------------------------------------------
//	Pixel Shader
//------------------------------------------
float4 PS_Point(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuseMap.Sample(samPoint, input.UV);
}

float4 PS_Linear(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuseMap.Sample(samLinear, input.UV);
}

float4 PS_Anisotropic(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuseMap.Sample(samAnisotropic, input.UV);
}

//------------------------------------------
//	Technique
//------------------------------------------
technique11 DefaultTechnique
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_5_0, PS_Point() ) );
	}

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Linear()));
    }

    pass P2
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Anisotropic()));
    }
}