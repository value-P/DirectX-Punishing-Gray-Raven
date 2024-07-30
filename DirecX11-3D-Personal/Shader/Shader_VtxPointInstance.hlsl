#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector g_vCamPosition;
texture2D g_Texture;
vector g_vPivotPos;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;

    row_major float4x4 TransformMatrix : WORLD;

    float4 vColor : COLOR0;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float4 vColor : COLOR0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    vector vPosition = mul(vector(In.vPosition, 1.f), In.TransformMatrix);
	
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.vPSize = float2(length(In.TransformMatrix._11_12_13) * In.vPSize.x,
		length(In.TransformMatrix._21_22_23) * In.vPSize.y);
    Out.vColor = In.vColor;

    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float4 vColor : COLOR0;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vColor : COLOR0;
};

// void GS_MAIN(point GS_IN[1], inout LineStream<GS_OUT> DataStream)
// void GS_MAIN(triangle GS_IN[3], inout TriangleStream<GS_OUT> DataStream)
// void GS_MAIN(line GS_IN[2], inout TriangleStream<GS_OUT> DataStream)
[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];
    
    float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = In[0].vPosition + float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vColor = In[0].vColor;


    Out[1].vPosition = In[0].vPosition - float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vColor = In[0].vColor;

    Out[2].vPosition = In[0].vPosition - float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vColor = In[0].vColor;

    Out[3].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vColor = In[0].vColor;



    DataStream.Append(Out[0]);
    DataStream.Append(Out[1]);
    DataStream.Append(Out[2]);
    DataStream.RestartStrip();

    DataStream.Append(Out[0]);
    DataStream.Append(Out[2]);
    DataStream.Append(Out[3]);
}

[maxvertexcount(6)]
void GS_ROTATION_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];

    vector vPivotPos = mul(g_vPivotPos, g_WorldMatrix);
    
    //float3 vUp = In[0].vPosition.xyz - g_vPivotPos.xyz;
    float3 vUp = In[0].vPosition.xyz - vPivotPos.xyz;

    vUp = normalize(vUp);
    
    float3 vLook = normalize((g_vCamPosition - In[0].vPosition).xyz) * In[0].vPSize.y * 0.5f;
    float3 vRight = normalize(cross(vUp, vLook)) * In[0].vPSize.x * 0.5f;
    vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = In[0].vPosition + float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vColor = In[0].vColor;


    Out[1].vPosition = In[0].vPosition - float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vColor = In[0].vColor;

    Out[2].vPosition = In[0].vPosition - float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vColor = In[0].vColor;

    Out[3].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vColor = In[0].vColor;

    DataStream.Append(Out[0]);
    DataStream.Append(Out[1]);
    DataStream.Append(Out[2]);
    DataStream.RestartStrip();

    DataStream.Append(Out[0]);
    DataStream.Append(Out[2]);
    DataStream.Append(Out[3]);
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vColor : COLOR0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vColor.a == 0.f)
        discard;

    Out.vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;

    if (Out.vColor.a < 0.3f)
        discard;

    Out.vColor = In.vColor;

    return Out;
}

PS_OUT PS_DARKMODE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;

    if (Out.vColor.a < 0.3f)
        discard;

    Out.vColor = vector(1.f, 1.f, 1.f, 1.f);

    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass RoatationPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_ROTATION_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass DefaultPassDarkMode
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_DARKMODE_MAIN();
    }

    pass RoatationPassDarkMode
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_ROTATION_MAIN();
        PixelShader = compile ps_5_0 PS_DARKMODE_MAIN();
    }
}