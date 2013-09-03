////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - tool.fx
////////////////////////////////////////////////////////////////////////////////////////

float4x4 WorldViewProjection : WorldViewProjection;
float3 VertexColor;

struct VS_OUTPUT
{
    float4 Position :POSITION;
};

//Vertex Shader
VS_OUTPUT VShader(float4 inPos :POSITION)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(inPos, WorldViewProjection);
    return output;
}

//Pixel Shader
float4 PShader(VS_OUTPUT input) :COLOR0
{   
    return float4(VertexColor.r, VertexColor.g, VertexColor.b, 0.5);
}

//Techniques
technique Main
{
    pass Pass0
    {
        LIGHTING = FALSE;
        ZENABLE = FALSE;
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