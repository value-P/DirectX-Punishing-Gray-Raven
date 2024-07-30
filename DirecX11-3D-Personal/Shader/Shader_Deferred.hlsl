#include "Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix      g_ViewMatrixInv, g_ProjMatrixInv;
matrix      g_LightViewMatrix, g_LightProjMatrix;

texture2D	g_Texture;

float g_fFar;

// 점광원일때
vector      g_vLightPos; 
float       g_fLightRange;

vector		g_LightDir;
vector      g_vLightDiffuse;
vector      g_vLightAmbient;
//vector      g_vLightSpecular;

vector g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
//vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

//texture2D   g_SpecularTexture;
texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
texture2D	g_ShadeTexture;
texture2D   g_DepthTexture;
texture2D g_ShadowTexture;

// outline
float g_fWinSizeX;
float g_fWinSizeY;


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


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

    Out.vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord);

	return Out;
}

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    //vector vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
	
    vector vNormalDesc = g_NormalTexture.Sample(g_PointSampler, In.vTexcoord);
	
	// 0~1 -> -1~1
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	
    
    // NDotL -> 하프램버트 -> floor 툰쉐이딩
    float fNdotL = saturate(dot(normalize(g_LightDir) * -1.f, vNormal));
    float fhalfLambert = fNdotL * 0.5f + 0.5f;
    //fhalfLambert = pow(fhalfLambert, 3.f); // 너무 인위적인느낌이 나지않게 3제곱 해주는 방법이 있다고 한다
    float fFloorToon = floor(fhalfLambert * 5.f) / 5.f;
    
    Out.vShade = g_vLightDiffuse *
    saturate(fFloorToon + (g_vLightAmbient * g_vMtrlAmbient));
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
	
    vector vNormalDesc = g_NormalTexture.Sample(g_PointSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(g_PointSampler, In.vTexcoord);
    vector vWorldPos;
    
    // 텍스쿠드 (0,0) ~ (1,1) -> (-1,1) ~ (1,-1) 투영스페이스로
    // 0~1 -> -1~1
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    // 0~1 -> 1~-1
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    // 투영행렬까지 곱하면 0 ~ far
    // w나누기 0 ~ 1 이기 때문에 다시 내리고자 한다면 far를 곱해줘서 원래의 깊이를 살린다
    // 쉐이더에서 mul은 클라이언트의 transformationcoord와 다르게 w나누기를 자동수행하지 않기 때문
    float fViewZ = vDepthDesc.y * g_fFar;
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
	// 0~1 -> -1~1
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	
    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);
    
    // 빛의 감쇄값 0~1
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
    
    float fNdotL = saturate(dot(normalize(vLightDir) * -1.f, vNormal));
    float fhalfLambert = fNdotL * 0.5f + 0.5f;
    //fhalfLambert = pow(fhalfLambert, 3.f); // 너무 인위적인느낌이 나지않게 3제곱 해주는 방법이 있다고 한다
    float fFloorToon = floor(fhalfLambert * 2.f) / 2.f;
    
    Out.vShade = g_vLightDiffuse * saturate(fFloorToon + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
    
    return Out;
}

PS_OUT PS_MAIN_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    if (0.f == vDiffuse.a)
        discard;
	
    vector vShade = g_ShadeTexture.Sample(g_LinearSampler, In.vTexcoord);
	
    Out.vColor = vDiffuse * vShade;
	
    return Out;
}

PS_OUT PS_MAIN_SHADOW_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
	if(0.f == vDiffuse.a)
        discard;
	
    vector vShade = g_ShadeTexture.Sample(g_LinearSampler, In.vTexcoord);
	
    vector vColor = vDiffuse * vShade;

    vector vDepthDesc = g_DepthTexture.Sample(g_PointSampler, In.vTexcoord);
    vector vWorldPos;
    
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    float fViewZ = vDepthDesc.y * g_fFar;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    // 픽셀의 월드 포지션을 가져와서 광원의 뷰,투영 스페이스까지 픽셀을 가져가 비교
    vector vLightProjPosition = mul(vWorldPos, g_LightViewMatrix);
    vLightProjPosition = mul(vLightProjPosition, g_LightProjMatrix);
    
    float fLightViewZ = vLightProjPosition.w;
    
    vLightProjPosition = vLightProjPosition.xyzw / vLightProjPosition.w;
    
    float2 vUV;
    
    /* -1 -> 0*/
	/* 1 -> 1 */
    vUV.x = vLightProjPosition.x * 0.5f + 0.5f;

	/* 1 -> 0*/
	/* -1 -> 1 */
    vUV.y = vLightProjPosition.y * -0.5f + 0.5f;
    
    // 픽셀을 기록된 깊이랑 비교
    vector vLightDepth = g_ShadowTexture.Sample(g_PointSampler, vUV);

    if (vLightDepth.x * g_fFar + 0.1f <= fLightViewZ)
    {
        vColor = vColor * 0.7f;
    }

    Out.vColor = vColor;
	
    return Out;
}

PS_OUT PS_OUTLINE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    //g_DiffuseTexture
    
    for (int i = 0; i < 9; ++i)
    {
        Out.vColor += fLaplacianMask[i] * g_NormalTexture.Sample(g_LinearClampSampler,
        In.vTexcoord + float2(fCoord[i % 3] / g_fWinSizeX, fCoord[i / 3] / g_fWinSizeY));
    }
    
    float fBlack = 1.f - (Out.vColor.r * 0.3f + Out.vColor.g * 0.59f + Out.vColor * 0.11f);
    Out.vColor = vector(fBlack, fBlack, fBlack, 1.f);
    
    return Out;
}

technique11	DefaultTechnique
{
	pass Debug // 0
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Light_Directional // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Light_Point // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Deferred_Final // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }

    pass Deferred_Shadow_Final // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW_FINAL();
    }

    pass Deferred_Outline // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_OUTLINE_MAIN();
    }
}