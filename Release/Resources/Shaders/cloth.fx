//===============================================================================
//								= MAIN SHADER =
//===============================================================================
//===============================================================================
//FEATURES:
//		- Diffuse/ambient lighting
//		- No texturing
//===============================================================================

float4x4 World 					: World;
float4x4 WorldViewProjection 	: WorldViewProjection;
float4x4 WorldInvTrans 			: WorldInverseTranspose;

float3 CameraPos;
float3 LightPos;
float AmbientIntensity;		float4 AmbientColor;
float DiffuseIntensity;		float4 DiffuseColor;
float SpecularIntensity;	float4 SpecularColor;		float SpecularSize;

Texture texColor; //COLOR
sampler ColorSampler = sampler_state 
{texture = <texColor>; magfilter = LINEAR; minfilter = LINEAR; mipfilter = LINEAR; AddressU = WRAP; AddressV = WRAP; };


//===============================================================================
//STRUCTURES
//===============================================================================

struct VS_OUTPUT
{
    float4 Pos 			: POSITION;
    float3 Normal		: TEXCOORD0;
    float3 LightVector	: TEXCOORD1;
    float3 CameraVector	: TEXCOORD2;
    float2 UV			: TEXCOORD3;
};

//===============================================================================
//VERTEX SHADER
//===============================================================================

VS_OUTPUT VShader(	float4 inPos : POSITION, 
					float3 inNormal : NORMAL,
					float2 inUV		: TEXCOORD0 )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    //TRANSFORM
    output.Pos = mul(inPos, WorldViewProjection); 
    float3 PosWorld = mul(inPos, World); 
   
	//NORMAL
	output.Normal = mul(inNormal,WorldInvTrans);
	
	//LIGHT VECTOR
	output.LightVector = (LightPos - PosWorld);
	
	//CAMERA VECTOR
	output.CameraVector = (CameraPos - PosWorld);
	
	output.UV = inUV;
    
    return output;
}

//===============================================================================
//PIXEL SHADER
//===============================================================================

float4 PShader(VS_OUTPUT input) : COLOR0
{	
	input.Normal = normalize(input.Normal);
	input.LightVector = normalize(input.LightVector);
	input.CameraVector = normalize(input.CameraVector);
	
	//DIFFUSE LIGHT
	float4 Diffuse = dot(input.LightVector, input.Normal);
	if(Diffuse.r < 0)
	{
		Diffuse *= -1;
	}
	Diffuse = (Diffuse+1)*0.5; //Brings into range 0-1
	Diffuse = Diffuse * DiffuseIntensity * DiffuseColor;
	
	float3 HalfVector = -normalize(input.LightVector + input.CameraVector);											
	float4 Specular = 0.3*(pow(saturate(dot(input.Normal, HalfVector)), 10.0));

	//AMBIENT LIGHT
	float4 Ambient = (AmbientIntensity * AmbientColor);
	
	//TEXTURE
	float4 Texture = tex2D(ColorSampler, input.UV);
	Texture *= 0.03;
	
	return (Texture + Diffuse + Ambient + Specular);
}


//===============================================================================
//TECHNIQUES
//===============================================================================

technique MAIN
{
    pass Pass0
    {
        LIGHTING = TRUE;
        ZENABLE = TRUE;
        ZWRITEENABLE = TRUE;
        CULLMODE = NONE;

        VertexShader = compile vs_3_0 VShader();
        PixelShader = compile ps_3_0 PShader();

    }
}
