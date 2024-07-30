#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
vector g_vColor;

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
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

VS_OUT VS_TRAIL_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matVP;

    matVP = mul(g_ViewMatrix, g_ProjMatrix);
	
    Out.vPosition = mul(vector(In.vPosition, 1.f), matVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

/* 정점 세개(TriangleList)가 모이게 되면. */
/* w나누기연산을 수행한다.(원근투영) */
/* 뷰포트변환.(윈도우좌표로 변환한다) */
/* 래스터라이즈.( 픽셀을 생성한다. )*/

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord);
    
    return Out;
}

PS_OUT PS_TRAIL_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord);
    Out.vColor.a *= (1.f - In.vTexcoord.x);
    if (Out.vColor.r < 0.3f)
        discard;
    
    Out.vColor.xyz *= g_vColor.xyz;
    
    return Out;
}


technique11 DefaultTechnique
{
    pass DefaultPass // 0
    {
		/* RenderState설정. */
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default,0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass TrailPass // 1
    {
   		SetRasterizerState(RS_None_Cull);
        //SetDepthStencilState(DSS_Default, 0);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_TRAIL_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TRAIL_MAIN();
    }
}


