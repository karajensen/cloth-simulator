
#include "sprite.h"
#include "input.h"

LPD3DXSPRITE Sprite::d3dspt = NULL;
D3DXMATRIX Sprite::worldMatrix;

void Sprite::CreateSpriteObject(LPDIRECT3DDEVICE9 d3ddev)
{
    D3DXCreateSprite(d3ddev, &d3dspt);
    D3DXMatrixIdentity(&worldMatrix);
}

void Sprite::ToggleVisibility(float visOverride)
{
    if(vis < FULL_ALPHA)
        vis = FULL_ALPHA;
    else
        vis = int(visOverride);
}

void Sprite::ReleaseSpriteObject()
{
    if(d3dspt != NULL)
        d3dspt->Release();
}

Sprite::~Sprite()
{ 
    if (tex != NULL)
        tex->Release(); 
}

void Sprite::StartSpriteRender()
{
    d3dspt->Begin(D3DXSPRITE_ALPHABLEND);
}

void Sprite::EndSpriteRender()
{
    d3dspt->End();
}

void Sprite::OnDraw(float scale)
{
    ScaleFactor = scale;
    D3DXVECTOR3 position(x, y, 0); //position of sprite
    D3DXVECTOR3 center(0.0f, 0.0f, 0.0f); //center at the upper-left corner

    if(scale != NULL)
    {    
        DrawScaled = true;
        worldMatrix._41 = x;
        worldMatrix._42 = y;
        worldMatrix._43 = 0;
        D3DXMatrixScaling(&worldMatrix,scale,scale,scale);
        d3dspt->SetTransform(&worldMatrix);
        d3dspt->Draw(tex, NULL, &center, &position, D3DCOLOR_ARGB(vis, FULL_ALPHA, FULL_ALPHA, FULL_ALPHA));
        D3DXMatrixIdentity(&worldMatrix);
        d3dspt->SetTransform(&worldMatrix);
    }
    else
    {
        D3DXVECTOR3 position(x, y, 0); //position of sprite
        D3DXVECTOR3 center(0.0f, 0.0f, 0.0f); //center at the upper-left corner
        d3dspt->Draw(tex, NULL, &center, &position, D3DCOLOR_ARGB(vis, FULL_ALPHA, FULL_ALPHA, FULL_ALPHA));
    }
    
}

bool Sprite::Load(LPDIRECT3DDEVICE9 d3ddev, const std::string& filename, int Vis, int Xwidth, int Yheight, float X, float Y, int height, int width)
{
    ScaleFactor = 1.0f;
    DrawScaled = false;

    vis = Vis; //visibility 0-255 (0=transparent, 255=opaque)
    xWidth = float(Xwidth);
    yHeight = float(Yheight);
    x = X;
    y = Y;

    visCounter = 0;
  
    if(FAILED(D3DXCreateTextureFromFileEx(d3ddev, filename.c_str(), width, height, D3DX_DEFAULT, NULL, TEXTURE_FORMAT,
                                          D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, NULL, NULL, NULL, &tex)))
    {
        Diagnostic::ShowMessage(filename + " failed to load");
        return false;
    }
    return true;
}

void Sprite::SetNewPos(float xNew, float yNew)
{
    x = xNew;
    y = yNew;
}

bool Sprite::OnClick(int clickX, int clickY)
{
    float Height = yHeight;
    float Width = xWidth;

    if(DrawScaled) //if last frame the sprite was scaled
    {
        Height = Height * ScaleFactor;
        Width = Width * ScaleFactor;
        DrawScaled = false;
    }

    if (vis > 1)
    {
        if ((clickY >= y) && (clickY <= (y+Height)) 
            && (clickX >= x) && (clickX <= (x+Width)))
        {
            return true; //if clicked
        }
    }
    return false; //if not visible or not clicked
}

bool Sprite::IsVisible()
{
    if (vis <= 0)
        return false;
    else
        return true;
}

void Sprite::SetVisibility(int v)
{
    if (v < 0)
        v = 0;
    else if (v > FULL_ALPHA)
        v = FULL_ALPHA;

    vis = v;
}

void Sprite::FadeVisibility(int speed, int v)
{
    if (vis != v)
    {
        if (v < vis)
        {
            int V = vis - speed;
            SetVisibility(V);
        }
        else
        {
            int V = vis + speed;
            SetVisibility(V);
        }
    }
}


