/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* Class for displaying text
*****************************************************************/

#pragma once

#include "common.h"

class Text
{
public:

    Text();
    ~Text();

    /**
    * Draw the text
    */
    void Draw();

    /**
    * Load the text
    * @param the directX device
    * @param whether the text is italic
    * @param the weight [0-1000] of the text
    * @param the font size of the text
    * @param the alignment of the text within the textbox
    * @param the top left corner of the textbox
    * @param the bottom right corner of the textbox
    */
    bool Load(LPDIRECT3DDEVICE9 d3ddev, bool italic, int weight, int size, UINT align, int xtl, int ytl, int xbr, int ybr);
    
    /**
    * Set a new position for the text box
    * @param the top left corner of the textbox
    */
    void SetPosition(int xtl, int ytl);

    /**
    * Set the text to display
    * @param the new text
    */
    void SetText(const std::string& text);

    /**
    * Set the text colour
    * @param the new colour from 0.0-1.0
    */
    void SetColour(const D3DXVECTOR3& color);

private:

    D3DCOLOR m_color;   ///< Colour of the text from 0-255
    RECT m_textbox;     ///< Text bounding area
    LPD3DXFONT m_font;  ///< Font for the text
    std::string m_text; ///< Text to display
    UINT m_align;       ///< Alignment of the text
};