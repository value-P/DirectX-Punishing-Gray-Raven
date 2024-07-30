#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_RotationMatrix;

texture2D g_Texture;
texture2D g_DistortionTexture;
float g_fDistortPower;
float2 g_vUVMove;

vector g_vCamPosition;
float g_fSlideRate;
vector g_vColor;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    Out.vPosition = mul(vector(In.vPosition,1.f), g_WorldMatrix);
    Out.vPSize = float2(length(g_WorldMatrix._11_12_13) * In.vPSize.x,
		length(g_WorldMatrix._21_22_23) * In.vPSize.y);

    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct GS_WORLD_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPosition : TEXCOORD1;
};

// void GS_MAIN(point GS_IN[1], inout LineStream<GS_OUT> DataStream)
// void GS_MAIN(triangle GS_IN[3], inout TriangleStream<GS_OUT> DataStream)
// void GS_MAIN(line GS_IN[2], inout TriangleStream<GS_OUT> DataStream)
[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];
    
    //float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;
    //float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
    //float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

    float3 vRight = float3(-1.f, 0.f, 0.f) * In[0].vPSize.x * 0.5f;
    float3 vUp = float3(0.f, 1.f, 0.f) * In[0].vPSize.y * 0.5f;
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = In[0].vPosition + float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);


    Out[1].vPosition = In[0].vPosition - float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);

    Out[2].vPosition = In[0].vPosition - float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);

    Out[3].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);

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
    
    //float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;
    //float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
    //float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;
    
    float4 vRight;
    vRight.xyz = float3(-1.f, 0.f, 0.f) * In[0].vPSize.x * 0.5f;
    vRight.w = 0.f;
    float4 vUp;
    vUp.xyz = float3(0.f, 1.f, 0.f) * In[0].vPSize.y * 0.5f;
    vUp.w = 0.f;
    
    vRight = mul(vRight, g_RotationMatrix);
    vUp = mul(vUp, g_RotationMatrix);
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Out[0].vPosition = In[0].vPosition + vRight + vUp;
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);

    Out[1].vPosition = In[0].vPosition - vRight + vUp;
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);

    Out[2].vPosition = In[0].vPosition - vRight - vUp;
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);

    Out[3].vPosition = In[0].vPosition + vRight - vUp;
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);

    DataStream.Append(Out[0]);
    DataStream.Append(Out[1]);
    DataStream.Append(Out[2]);
    DataStream.RestartStrip();

    DataStream.Append(Out[0]);
    DataStream.Append(Out[2]);
    DataStream.Append(Out[3]);
}

[maxvertexcount(6)]
void GS_WORLD_MAIN(point GS_IN In[1], inout TriangleStream<GS_WORLD_OUT> DataStream)
{
    GS_WORLD_OUT Out[4];
    
    float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = In[0].vPosition + float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vProjPosition = Out[0].vPosition;


    Out[1].vPosition = In[0].vPosition - float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vProjPosition = Out[1].vPosition;

    Out[2].vPosition = In[0].vPosition - float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vProjPosition = Out[2].vPosition;

    Out[3].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vProjPosition = Out[3].vPosition;

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
};

struct PS_WORLD_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPosition : TEXCOORD1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord);

    if (Out.vColor.a < 0.3f)
        discard;
    
    return Out;
}

PS_OUT PS_SLIDER_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
        
    if (g_fSlideRate < In.vTexcoord.x)
        discard;

    Out.vColor = g_vColor;

    if (Out.vColor.a < 0.3f)
        discard;

    return Out;
}

PS_OUT PS_WORLD_DISTORTION_MAIN(PS_WORLD_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vShape = g_Texture.Sample(g_LinearSampler, In.vTexcoord + g_vUVMove);
    if(vShape.r < 0.2)
        discard;

    Out.vColor.r = 1.f;
    Out.vColor.g = In.vProjPosition.z / In.vProjPosition.w; // 깊이 버퍼의 r과 비교할 클립스페이스의 z
    vector vNoise = g_DistortionTexture.Sample(g_LinearSampler, In.vTexcoord + g_vUVMove);
    
    Out.vColor.r = saturate(Out.vColor.r * vNoise.r * g_fDistortPower);
   
    return Out;
}


technique11 DefaultTechnique
{
    pass DefaultPass // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        //SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass SliderPass // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_SLIDER_MAIN();
    }

    pass RotationPass // 2
    {
        //SetRasterizerState(RS_Default);
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_ROTATION_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass WorldRectDistortionPass // 3
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_WORLD_MAIN();
        PixelShader = compile ps_5_0 PS_WORLD_DISTORTION_MAIN();
    }
}