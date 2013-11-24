////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - cloth.fx
////////////////////////////////////////////////////////////////////////////////////////

float4x4 World                :World;
float4x4 WorldViewProjection  :WorldViewProjection;
float4x4 WorldInvTrans        :WorldInverseTranspose;

float3 CameraPosition;
float3 LightPosition;
float3 AmbientColor;
float3 DiffuseColor;
float AmbientIntensity;
float DiffuseIntensity;     
float SpecularIntensity;    
float SpecularSize;

Texture DiffuseTexture;
sampler ColorSampler = sampler_state 
{ 
    texture = <DiffuseTexture>; 
    magfilter = LINEAR; 
    minfilter = LINEAR; 
    mipfilter = LINEAR; 
    AddressU = WRAP; 
    AddressV = WRAP; 
};

struct VS_OUTPUT
{
    float4 Position      :POSITION;
    float3 Normal        :TEXCOORD0;
    float3 LightVector   :TEXCOORD1;
    float3 CameraVector  :TEXCOORD2;
    float2 UV            :TEXCOORD3;
};

VS_OUTPUT VShader(float4 position :POSITION, 
                  float3 normal   :NORMAL,
                  float2 uv       :TEXCOORD0)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    output.Position = mul(position, WorldViewProjection); 
    float3 PosWorld = mul(position, World); 
   
    output.Normal = mul(normal,WorldInvTrans);
    output.LightVector = LightPosition - PosWorld;
    output.CameraVector = CameraPosition - PosWorld;
    output.UV = uv;
    
    return output;
}

float4 PShader(VS_OUTPUT input) :COLOR0
{   
    input.Normal = normalize(input.Normal);
    input.LightVector = normalize(input.LightVector);
    input.CameraVector = normalize(input.CameraVector);
    
    //Brings into range 0-1, abs to prevent one side of cloth being darker
    float3 diffuse = (abs(dot(input.LightVector, input.Normal))+1)*0.5;

    float3 halfVector = normalize(input.LightVector + input.CameraVector);                                         
    float3 specular = SpecularIntensity*(pow(saturate(dot(input.Normal.rgb, halfVector)), SpecularSize));
    
    float4 color = tex2D(ColorSampler, input.UV);
    color *= 0.04; // reduce brightness of grid texture

    return float4(color.xyz + (diffuse * DiffuseIntensity * DiffuseColor) 
        + (AmbientIntensity * AmbientColor) + specular, 1.0);
}

technique Main
{
    pass Pass0
    {
        LIGHTING = TRUE;
        ZENABLE = TRUE;
        ZWRITEENABLE = TRUE;
        CULLMODE = NONE;

        VertexShader = compile vs_2_0 VShader();
        PixelShader = compile ps_2_0 PShader();
    }
}
