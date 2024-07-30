#include "Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D	g_BackBufferTexture;
texture2D	g_DistortionTexture;
texture2D   g_DepthTexture;
texture2D   g_OutlineTexture;
texture2D   g_ExceptDarkModeTexture;

float g_fTimeRate;

struct VS_IN
{
	float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;

};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
	
	return Out;
}


struct PS_IN
{
	float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};

PS_OUT PS_DISTORTION_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

    vector vDistortionColor = g_DistortionTexture.Sample(g_LinearSampler, In.vTexcoord);
    
    if (vDistortionColor.r == 0.f)
        discard;
    
    float2 DistortUV = In.vTexcoord + vDistortionColor.r;
    vector vBackBufferColor = g_BackBufferTexture.Sample(g_LinearSampler, DistortUV);

    Out.vColor = vBackBufferColor;
    
	return Out;
}

PS_OUT PS_OUTLINE_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
    
    Out.vColor = g_OutlineTexture.Sample(g_PointSampler, In.vTexcoord);
    
    if(Out.vColor.r != 0.f)
        discard;
    
    Out.vColor = vector(0.f, 0.f, 0.f, 1.f);
    
    return Out;
}

PS_OUT PS_DARKMODE_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
    
    Out.vColor = g_ExceptDarkModeTexture.Sample(g_PointSampler, In.vTexcoord);
    
    if(Out.vColor.r > 0.1f)
        discard;
    
    float alpha = 1.f - pow(g_fTimeRate, 3.f);
    
    Out.vColor = vector(0.f, 0.f, 0.f, alpha);
    
    return Out;
}

technique11	DefaultTechnique
{
    pass Distortion// 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION_MAIN();
    }

    pass OutLine // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_OUTLINE_MAIN();
    }

    pass DarkMode // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DARKMODE_MAIN();
    }

}


