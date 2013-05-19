////////////////////////////////////////////////////////////////////////////////////////
//KARA JENSEN - KaraPeaceJensen@gmail.com - TINY TOON TANKS - ToonShader.glsl
////////////////////////////////////////////////////////////////////////////////////////

//========================================================
//TOON VERTEX SHADER
//========================================================

uniform vec3 LightPos;
uniform vec4 LightColor;

varying vec2 UV;
varying vec3 VertexNormal;
varying vec3 LightVector;
varying vec4 DiffuseColor;

void main()
{

    //TRANSFORM VERTEX
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    vec4 WorldViewPos = gl_ModelViewMatrix * gl_Vertex;

    //TRANSFORM NORMAL
    VertexNormal = gl_NormalMatrix * gl_Normal;

    //TRANSFORM LIGHT POSITION
    LightVector = normalize(LightPos - vec3(WorldViewPos));

    //PASS LIGHT
    DiffuseColor = LightColor;

    //PASS UVS
    UV = gl_MultiTexCoord0;
}

//========================================================
//TOON PIXEL SHADER
//========================================================

uniform sampler2D TexSampler;
varying vec2 UV;
varying vec3 VertexNormal;
varying vec3 LightVector;
varying vec4 DiffuseColor;

void main()
{
    //GET TEXTURE COLOR
    vec4 Texture = texture2D(TexSampler, UV);

    //NORMALISE NORMAL
    vec3 normalisedNormal = normalize(VertexNormal);

    //DIFFUSE LIGHTING
    float diffuseValue = (dot(LightVector,normalisedNormal) + 1.0)*0.5;

    //MAKE DISCRETE BANDS BASED ON DIFFUSE
    if (diffuseValue > 0.75)
    {
        Texture -= vec4(0.0,0.0,0.0,0.0); //light bands
    }
    else if (diffuseValue > 0.5)
    {
        Texture -= vec4(0.2,0.2,0.2,0.0); //light bands
    }
    else if (diffuseValue > 0.25)
    {
        Texture -= vec4(0.4,0.4,0.4,0.0); //light bands
    }
    else
    {
        Texture -= vec4(0.6,0.6,0.6,0.0); //light bands
    }

    //FINAL COLOR
    gl_FragColor = Texture;
}
