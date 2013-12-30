////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - bounds.fx
////////////////////////////////////////////////////////////////////////////////////////

float4x4 WorldViewProjection : WorldViewProjection;
float3 VertexColor;

struct VS_OUTPUT
{
    float4 Position :POSITION;
};

VS_OUTPUT VShader(float4 position :POSITION)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(position, WorldViewProjection);
    return output;
}

float4 PShader(VS_OUTPUT input) :COLOR0
{   
    return float4(VertexColor.r, VertexColor.g, VertexColor.b, 0.2);
}

technique Main
{
    pass Pass0
    {
        LIGHTING = FALSE;
        ZENABLE = TRUE;
        ZWRITEENABLE = TRUE;
        CULLMODE = CCW;
        
        AlphaBlendEnable = true; 
        SrcBlend = SrcAlpha; 
        DestBlend = InvSrcAlpha;
        FillMode = Solid;

        VertexShader = compile vs_2_0 VShader();
        PixelShader = compile ps_2_0 PShader();
    }
}
