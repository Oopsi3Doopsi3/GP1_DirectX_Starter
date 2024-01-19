//------------------------------------------
//	Global Variables
//------------------------------------------
float4x4 gWorldViewProj : WorldViewProjection;

Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;

float4x4 gWorldMatrix : WorldMatrix;
float4x4 gViewInverseMatrix : ViewInverse;

bool gUseNormalMap : UseNormalMap;
float3 gLightDirection = normalize(float3(.577f, -.577f, .577f));
float gPi = 3.141592653589793f;
float gLightIntensity = 7.f;
float gShininess = 25.f;
float3 gAmbient = float3(.03f, .03f, .03f);

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
    output.Normal = mul(normalize(input.Normal), (float3x3) gWorldMatrix);
    output.Tangent = mul(normalize(input.Tangent), (float3x3) gWorldMatrix);
	return output;
}

//------------------------------------------
//	Pixel Shader
//------------------------------------------
float3 ShadePixel(VS_OUTPUT input, SamplerState state)
{
    //Normal
    float3 normal = input.Normal;
    if(gUseNormalMap)
    {
        const float3 binormal = cross(input.Normal, input.Tangent);
        const float4x4 tangentSpaceAxis = float4x4
        (
            float4(input.Tangent, 0.f),
	    	float4(binormal, 0.f),
	    	float4(input.Normal, 0.f),
	    	float4(0.f, 0.f, 0.f, 1.f)
	    );
        const float3 sampledNormal = 2.f * gNormalMap.Sample(state, input.UV).rgb - float3(1.f, 1.f, 1.f);
        normal = mul(float4(sampledNormal, 0.f), tangentSpaceAxis);
    }
    
    //OA and lambertDiffuse
    const float observedArea = saturate(dot(normal, -gLightDirection));
    const float3 lambertDiffuse = gDiffuseMap.Sample(state, input.UV) * gLightIntensity / gPi;
    
    //Specular
    const float3 viewDir = normalize(input.Position.xyz - gViewInverseMatrix[3].xyz);
    const float specularColor = gSpecularMap.Sample(state, input.UV);
    const float phongExponent = gGlossinessMap.Sample(state, input.UV).r * gShininess;
    const float3 reflection = reflect(-gLightDirection, normal);
    const float cosAlpha = saturate(dot(reflection, -viewDir));
    const float specularValue = specularColor * pow(cosAlpha, phongExponent);
    
    return saturate((specularValue + lambertDiffuse) * observedArea + gAmbient);
}

float4 PS_Point(VS_OUTPUT input) : SV_TARGET
{
    float3 color = ShadePixel(input, samPoint);
    return float4(color, 1.f);
}

float4 PS_Linear(VS_OUTPUT input) : SV_TARGET
{
    float3 color = ShadePixel(input, samLinear);
    return float4(color, 1.f);
}

float4 PS_Anisotropic(VS_OUTPUT input) : SV_TARGET
{
    float3 color = ShadePixel(input, samAnisotropic);
    return float4(color, 1.f);
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