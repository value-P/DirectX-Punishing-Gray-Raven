#include "Shader_Defines.hlsli"

matrix g_BoneMatrices[512];

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture;
texture2D g_MaskTexture;
texture2D g_NoiseTexture;

float2 g_UVMove;
float4 g_fPrimaryColor;
float g_fDurationRate;

// 0~1수치
float g_fDistortionPower;

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
    float4 vProjPos : TEXCOORD2;
};

VS_OUT VS_NONANIM_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord + g_UVMove;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

VS_OUT VS_ANIM_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
    
    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
                        g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
                        g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
                        g_BoneMatrices[In.vBlendIndices.w] * fWeightW;
	
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord + g_UVMove;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
    vector vNormal : SV_TARGET1;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vColor = vMtrlDiffuse;
    Out.vColor.a *= saturate(1.f - pow(g_fDurationRate, 2.5f));

	/* -1.0 ~ 1.f -> 0 ~ 1 */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    return Out;
}

PS_OUT PS_ALPHALERP_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 texCoord = In.vTexcoord;
    texCoord.x += 1.f;
    texCoord.x -= g_fDurationRate * 3.f;
    
    if (texCoord.x < 0.f)
        discard;
    if (texCoord.x > 1.f)
        discard;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    
    if(texCoord.x < 0.f)
        discard;
    if (texCoord.x > 0.5f)
        discard;
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vColor = vMtrlDiffuse;
    Out.vColor.a = saturate(1.f - pow(g_fDurationRate, 2.5f));
    
	/* -1.0 ~ 1.f -> 0 ~ 1 */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    return Out;
}

PS_OUT PS_USE_COLOR_MASK_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMaskDesc = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord);
    
    if (vMaskDesc.a < 0.3f)
        discard;
    
    Out.vColor = g_fPrimaryColor;
    Out.vColor.w = vMaskDesc.a;
    Out.vColor.a *= saturate(1.f - pow(g_fDurationRate, 2.5f));
    
	/* -1.0 ~ 1.f -> 0 ~ 1 */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

    return Out;
}

struct PS_DISTORT_OUT
{
    vector vColor : SV_TARGET0;
};

PS_DISTORT_OUT PS_DISTORTION_MAIN(PS_IN In)
{
    PS_DISTORT_OUT Out = (PS_DISTORT_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    if (vDiffuse.a >= 0.3f)
        discard;
    
    // r : 노이즈 강도
    // g : 오브젝트 깊이
    Out.vColor.r = 1.f;
    Out.vColor.g = In.vProjPos.z / In.vProjPos.w; // 깊이 버퍼의 r과 비교할 클립스페이스의 z
    vector vNoise = g_NoiseTexture.Sample(g_LinearSampler, In.vTexcoord);
    
    Out.vColor.r = saturate(Out.vColor.r * vNoise.r * g_fDistortionPower);

    return Out;
}

PS_DISTORT_OUT PS_NONANIM_DISTORTION_MAIN(PS_IN In)
{
    PS_DISTORT_OUT Out = (PS_DISTORT_OUT) 0;
    
    vector vMaskDesc = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord);
    
    if (vMaskDesc.a > 0.3f)
        discard;
    
    // r : 노이즈 강도
    // g : 오브젝트 깊이
    Out.vColor.r = 1.f;
    Out.vColor.g = In.vProjPos.z / In.vProjPos.w; // 깊이 버퍼의 r과 비교할 클립스페이스의 z
    vector vNoise = g_NoiseTexture.Sample(g_LinearSampler, In.vTexcoord);
    
    Out.vColor.r = saturate(Out.vColor.r * vNoise.r * g_fDistortionPower);

    return Out;
}

PS_DISTORT_OUT PS_DARKMODE_MAIN(PS_IN In)
{
    PS_DISTORT_OUT Out = (PS_DISTORT_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    if (vDiffuse.a < 0.3f)
        discard;

    Out.vColor = vector(1.f, 1.f, 1.f, 1.f);

    return Out;
}

PS_DISTORT_OUT PS_NONANIM_DARKMODE_MAIN(PS_IN In)
{
    PS_DISTORT_OUT Out = (PS_DISTORT_OUT) 0;
    
    vector vMaskDesc = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord);
    
    if (vMaskDesc.a < 0.3f)
        discard;

    Out.vColor = vector(1.f, 1.f, 1.f, 1.f);

    return Out;
}

technique11 DefaultTechnique
{
    pass NonAnimEffect //0
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_NONANIM_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass AnimEffect //1
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_ANIM_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ALPHALERP_MAIN();
    }

    pass NonAnimEffectUseMask //2
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_NONANIM_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_USE_COLOR_MASK_MAIN();
    }

    pass AnimEffectUseMask //3
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_ANIM_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_USE_COLOR_MASK_MAIN();
    }

    pass AnimDistortionEffect //4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_ANIM_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION_MAIN();
    }

    pass NonAnimDistortionEffect //5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_NONANIM_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NONANIM_DISTORTION_MAIN();
    }

    pass AnimEffectDarkMode //6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_ANIM_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DARKMODE_MAIN();
    }

    pass NonAnimEffectDarkMode //7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_NONANIM_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NONANIM_DARKMODE_MAIN();
    }
}