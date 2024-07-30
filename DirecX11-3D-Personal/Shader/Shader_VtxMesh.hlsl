#include "Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D	g_DiffuseTexture;
texture2D g_NormalTexture;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;	
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};



VS_OUT VS_MAIN(VS_IN In)
{	
	VS_OUT		Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);

	return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(g_LinearSampler, In.vTexcoord);

    if(vMtrlDiffuse.a < 0.3f)
        discard;
	
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
    // 0 ~ 1 -> -1 ~ 1
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    vNormal = mul(vNormal, WorldMatrix);

    Out.vColor = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

    return Out;
}

struct PS_SHDOW_OUT
{
    vector vColor : SV_TARGET0;
};

PS_SHDOW_OUT PS_MAIN_SHADOW(PS_IN In)
{
    PS_SHDOW_OUT Out = (PS_SHDOW_OUT) 0;

    vector vLightViewDepth = (vector) 0.f;

    Out.vColor = vector(In.vProjPos.w / 2000.f, 0.f, 0.f, 1.f);

    return Out;
}

technique11	DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Shadow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }
}


