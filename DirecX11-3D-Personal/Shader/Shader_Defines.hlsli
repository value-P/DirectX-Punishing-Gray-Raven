// Sampler State
sampler g_LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

sampler g_LinearClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
};

sampler g_PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

sampler g_PointClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = clamp;
    AddressV = clamp;
};

// Render State
RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
};

RasterizerState RS_CW_Cull
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = true;
};

RasterizerState RS_None_Cull
{
    FillMode = Solid;
    CullMode = None;
    FrontCounterClockwise = false;
};

RasterizerState RS_Wireframe
{
    FillMode = wireFrame;
    FrontCounterClockwise = false;
};

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
};

DepthStencilState DSS_None_ZTestAndWrite
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlending
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
};

float fLaplacianMask[9] = {
    -1.f, -1.f, -1.f,
    -1.f,  8.f, -1.f,
    -1.f, -1.f, -1.f };

float fCoord[3] = { -1.f, 0.f, 1.f };