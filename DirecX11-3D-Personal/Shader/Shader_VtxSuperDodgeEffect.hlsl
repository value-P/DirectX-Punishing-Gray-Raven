#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
float g_fDurationRate;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    
    uint4 vBlendIndices : BLENDINDEX;
    float4 vBlendWeights : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vTexcoord.x = Out.vTexcoord.y + g_fDurationRate * 0.5f;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(g_LinearSampler,In.vTexcoord);
    
    if(Out.vColor.a < 0.3f)
        discard;
    
    Out.vColor.a = saturate(1.f - pow(g_fDurationRate, 2.5f));

    return Out;
}

PS_OUT PS_DARKMODE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = vector(1.f, 1.f, 1.f, 1.f);

    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        //SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass DarkMode
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DARKMODE_MAIN();
    }
}