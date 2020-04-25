////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - text.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

/**
* Class for displaying text
*/
class Text
{
public:

    /**
    * Constructor
    */
    Text();

    /**
    * Destructor
    */
    ~Text();

    /**
    * Render the text to screen
    */
    void Draw();

    /**
    * Load the text
    * @param d3ddev The directX device
    * @param italic Whether the text is italic
    * @param weight The weight [0-1000] of the text
    * @param size The font size of the text
    * @param align The alignment of the text within the textbox
    * @param xtl/ytl The top left corner of the textbox
    * @param xbr/ybr The bottom right corner of the textbox
    * @return whether creation was successful
    */
    bool Load(LPDIRECT3DDEVICE9 d3ddev, bool italic, int weight, 
        int size, UINT align, int xtl, int ytl, int xbr, int ybr);
    
    /**
    * Set a new position for the text box
    * @param xtl/ytl The top left corner of the textbox
    */
    void SetPosition(int xtl, int ytl);

    /**
    * Set the text to display
    * @param text The new text
    */
    void SetText(const std::string& text);

    /**
    * Set the text colour
    * @param color The new colour from 0.0-1.0
    */
    void SetColour(const D3DXVECTOR3& color);

private:

    /**
    * Prevent copying
    */
    Text(const Text&);
    Text& operator=(const Text&);

private:

    D3DCOLOR m_color;   ///< Colour of the text from 0-255
    RECT m_textbox;     ///< Text bounding area
    LPD3DXFONT m_font;  ///< Font for the text
    std::string m_text; ///< Text to display
    UINT m_align;       ///< Alignment of the text
};