////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - main.fx
////////////////////////////////////////////////////////////////////////////////////////

float4x4 World                 :World;
float4x4 WorldViewProjection   :WorldViewProjection;
float4x4 WorldInvTrans         :WorldInverseTranspose;

float3 AmbientColor;
float3 LightPosition;
float3 DiffuseColor;
float3 VertexColor;
float AmbientIntensity;     
float DiffuseIntensity;     

Texture DiffuseTexture;
sampler ColorSampler = sampler_state 
{ 
    texture = <DiffuseTexture>; 
    magfilter = POINT; 
    minfilter = POINT; 
    mipfilter = POINT; 
    AddressU = CLAMP; 
    AddressV = CLAMP; 
};

struct VS_OUTPUT
{
    float4 Position     :POSITION;
    float3 Normal       :TEXCOORD0;
    float3 LightVector  :TEXCOORD1;
    float2 UV           :TEXCOORD2;
};                 
                        
VS_OUTPUT VShader(float4 position   :POSITION, 
                  float3 normal     :NORMAL,
                  float2 uv         :TEXCOORD0)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    output.Position = mul(position, WorldViewProjection); 
    output.Normal = mul(normal, WorldInvTrans);
    output.LightVector = LightPosition - mul(position, World);
    output.UV = uv;

    return output;
}

float4 PShader(VS_OUTPUT input) :COLOR0
{   
    input.LightVector = normalize(input.LightVector);
    input.Normal = normalize(input.Normal);
    
    // bring diffuse into range of 0.2->1.0
    float inner = 0.3;
    float3 diffuse = ((dot(input.LightVector, input.Normal)
        +1.0)*((1.0-inner)/(2.0)))+inner;

    diffuse *= DiffuseIntensity * DiffuseColor;
    float3 ambient = AmbientIntensity * AmbientColor;
    float3 color = saturate(tex2D(ColorSampler, input.UV).xyz * VertexColor);
    return float4(color * (diffuse + ambient), 1.0);
}

technique Main
{
    pass Pass0
    {
        LIGHTING = TRUE;
        ZENABLE = TRUE;
        ZWRITEENABLE = TRUE;
        CULLMODE = CCW;

        VertexShader = compile vs_2_0 VShader();
        PixelShader = compile ps_2_0 PShader();
    }
}