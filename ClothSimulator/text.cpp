////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - text.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "text.h"

namespace
{
    static const int FULL_ALPHA = 255; ///< Full transparency value
}

Text::Text() :
    m_color(D3DCOLOR_ARGB(FULL_ALPHA, FULL_ALPHA, FULL_ALPHA, FULL_ALPHA)),
    m_font(nullptr),
    m_align(DT_LEFT)
{
}

Text::~Text()
{ 
    if(m_font != nullptr)
    {
        m_font->Release(); 
    }
}

void Text::SetText(const std::string& text)
{
    m_text = text;
}

bool Text::Load(LPDIRECT3DDEVICE9 d3ddev, bool italic, int weight, 
    int size, UINT align, int xtl, int ytl, int xbr, int ybr)
{
    if(FAILED(D3DXCreateFont(d3ddev, size, 0, weight, 1, italic, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, LPSTR("Tahoma"), &m_font)))
    {
        ShowMessageBox("Text failed to load");
        return false;
    }
    SetRect(&m_textbox,xtl,ytl,xbr,ybr);
    return true;
}

void Text::SetPosition(int xtl, int ytl)
{
    long xdiff = xtl - m_textbox.left;
    long ydiff = ytl - m_textbox.top;
    m_textbox.left = xtl;
    m_textbox.top = ytl;
    m_textbox.right += xdiff;
    m_textbox.bottom += ydiff;
}

void Text::Draw()
{    
    m_font->DrawTextA(nullptr, m_text.c_str(), m_text.length(), &m_textbox,
        DT_WORDBREAK|DT_NOCLIP|m_align, m_color);
}

void Text::SetColour(const D3DXVECTOR3& color)
{
    m_color = D3DCOLOR_ARGB(FULL_ALPHA, static_cast<int>(color.x*FULL_ALPHA), 
        static_cast<int>(color.y*FULL_ALPHA), static_cast<int>(color.z*FULL_ALPHA));
}

