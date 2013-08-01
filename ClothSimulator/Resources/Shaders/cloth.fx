/****************************************************************
* Kara Jensen (mail@karajensen.com) 
* Cloth shader
*****************************************************************/

float4x4 World                : World;
float4x4 WorldViewProjection  : WorldViewProjection;
float4x4 WorldInvTrans        : WorldInverseTranspose;

float3 CameraPos;
float3 LightPos;
float AmbientIntensity;     
float4 AmbientColor;
float DiffuseIntensity;     
float4 DiffuseColor;
float SpecularIntensity;    
float4 SpecularColor;       
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
    float4 Pos          : POSITION;
    float3 Normal       : TEXCOORD0;
    float3 LightVector  : TEXCOORD1;
    float3 CameraVector : TEXCOORD2;
    float2 UV           : TEXCOORD3;
};

//Vertex Shader
VS_OUTPUT VShader(float4 inPos    : POSITION, 
                  float3 inNormal : NORMAL,
                  float2 inUV     : TEXCOORD0)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    output.Pos = mul(inPos, WorldViewProjection); 
    float3 PosWorld = mul(inPos, World); 
   
    output.Normal = mul(inNormal,WorldInvTrans);
    output.LightVector = LightPos - PosWorld;
    output.CameraVector = CameraPos - PosWorld;
    output.UV = inUV;
    
    return output;
}

//Pixel Shader
float4 PShader(VS_OUTPUT input) : COLOR0
{   
    input.Normal = normalize(input.Normal);
    input.LightVector = normalize(input.LightVector);
    input.CameraVector = normalize(input.CameraVector);
    
    //Brings into range 0-1, abs to prevent one side of cloth being darker
    float4 Diffuse = (abs(dot(input.LightVector, input.Normal))+1)*0.5;

    float3 HalfVector = -normalize(input.LightVector + input.CameraVector);                                         
    float4 Specular = SpecularIntensity*(pow(saturate(dot(input.Normal, HalfVector)), SpecularSize));
    
    float4 Texture = tex2D(ColorSampler, input.UV);
    Texture *= 0.04; // reduce brightness of grid texture
    
    return Texture + (Diffuse * DiffuseIntensity * DiffuseColor) 
        + (AmbientIntensity * AmbientColor) + Specular;
}

//Techniques
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
