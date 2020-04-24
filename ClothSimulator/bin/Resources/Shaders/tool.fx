////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - tool.fx
////////////////////////////////////////////////////////////////////////////////////////

float4x4 World                 :World;
float4x4 WorldViewProjection   :WorldViewProjection;
float4x4 WorldInvTrans         :WorldInverseTranspose;

float3 VertexColor;
float3 LightPosition;

struct VS_OUTPUT
{
    float4 Position     :POSITION;
    float3 Normal       :TEXCOORD0;
    float3 LightVector  :TEXCOORD1;
};

VS_OUTPUT VShader(float4 position  :POSITION,
                  float3 normal    :NORMAL)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(position, WorldViewProjection); 
    output.Normal = mul(normal, WorldInvTrans);
    output.LightVector = LightPosition - mul(position, World);
    return output;
}

float4 PShader(VS_OUTPUT input) :COLOR0
{   
    input.LightVector = normalize(input.LightVector);
    input.Normal = normalize(input.Normal);
    
    // bring diffuse into range of 0.6->1.0
    float inner = 0.6;
    float3 diffuse = ((dot(input.LightVector, input.Normal)
        +1.0)*((1.0-inner)/(2.0)))+inner;
    diffuse *= VertexColor;

    return float4(diffuse.r, diffuse.g, diffuse.b, 0.7);
}

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
