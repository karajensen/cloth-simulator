/****************************************************************
* Kara Jensen (mail@karajensen.com) 
* Diagnostic static drawing class
*****************************************************************/
#pragma once

#include <unordered_map>
#include <array>
#include "common.h"

class Shader;
class Text;

class Diagnostic
{
public:

    /**
    * Easy access diagnostic colours
    */
    enum Colour
    {
        WHITE,
        RED,
        GREEN,
        BLUE,
        YELLOW
    };

    /**
    * @return the diagnostics object
    */
    static Diagnostic& Get() { return *sm_diag; }

    /**
    * Initialise the use of diagnostics 
    * @param the directX device
    * @param the shader to apply to diagnostics meshes
    */
    static void Initialise(LPDIRECT3DDEVICE9 d3ddev, std::shared_ptr<Shader> boundsShader);

    /**
    * Shows a popup box with a message
    * @param the message to show
    */
    static void ShowMessage(const std::string& message);

    /**
    * Sets/Gets whether to render diagnostics
    */
    static void ToggleDiagnostics();
    static bool AllowDiagnostics();

    /**
    * Sets/Gets whether to render text diagnostics
    */
    static void ToggleText();
    static bool AllowText();

    /**
    * Adds a sphere for diagnostic rendering. Will only add once per id and update each call
    * @param the id of the sphere
    * @param the colour of the sphere
    * @param the poition in world coordinates
    * @param the radius of the sphere
    */
    void UpdateSphere(const std::string& id, Colour color, const D3DXVECTOR3& position, float radius);

    /**
    * Adds text for diagnostic rendering. Will only add once per id and update each call
    * @param the id of the text
    * @param the colour of the text
    * @param the text to draw
    */
    void UpdateText(const std::string& id, Colour color, const std::string& text);

    /**
    * Adds text for diagnostic rendering. Will only add once per id and update each call
    * @param the id of the text
    * @param the colour of the text
    * @param whether to increase the counter or not
    */
    void UpdateText(const std::string& id, Colour color, bool increaseCounter);

    /**
    * Adds a cylinder line for diagnostic rendering. Will only add once per id and update each call
    * @param the id of the line
    * @param the colour of the line
    * @param the start position in world coordinates
    * @param the end position in world coordinates
    */
    void UpdateLine(const std::string& id, Colour color, const D3DXVECTOR3& start, const D3DXVECTOR3& end);

    /**
    * Draws all 3D diagnostics
    * @param the projection matrix
    * @param the view matrix
    */
    void DrawAllObjects(const Transform& projection, const Transform& view);

    /**
    * Draws all 2D diagnostics
    */
    void DrawAllText();

    /**
    * Destructor
    */
    ~Diagnostic();

private: 

    struct DiagText
    {
        D3DXVECTOR3 color; ///< Colour of the text
        std::string text;  ///< Actual text to display
        int counter;       ///< Optional counter for text
    };

    struct DiagSphere
    {
        D3DXVECTOR3 color;  ///< Colour of the object
        Transform world;    ///< Matrix for sphere
        bool draw;          ///< Whether to render the sphere
    };

    struct DiagLine
    {
        D3DXVECTOR3 color;  ///< Colour of the object
        Transform world;    ///< Matrix for line
        bool draw;          ///< Whether to render the line
    };

    static std::shared_ptr<Shader> sm_shader; ///< Global diagnostic mesh shader
    static std::shared_ptr<Diagnostic> sm_diag; ///< Diagnostic singleton pointer
    static bool sm_showText; ///< Whether text diagnostics are visible or not
    static bool sm_showDiagnostics; ///< Whether diagnostics are visible or not

    Diagnostic(LPDIRECT3DDEVICE9 d3ddev);
    Diagnostic& operator=(const Diagnostic&);
    Diagnostic(const Diagnostic&);  

    typedef std::string KeyType;
    typedef std::unordered_map<KeyType, DiagSphere> SphereMap;
    typedef std::unordered_map<KeyType, DiagLine> LineMap;
    typedef std::unordered_map<KeyType, DiagText> TextMap;
    typedef std::unordered_map<Colour, D3DXVECTOR3> ColorMap;

    TextMap m_textmap;       ///< Map of text diagnostics
    SphereMap m_spheremap;   ///< Map of sphere diagnostics
    LineMap m_linemap;       ///< Map of cylinder diagnostics
    ColorMap m_colourmap;    ///< Easy access diagnostic colours

    LPDIRECT3DDEVICE9 m_d3ddev;     ///< DirectX Device
    LPD3DXMESH m_sphere;            ///< Diagnostic geometry sphere
    LPD3DXMESH m_cylinder;          ///< Diagnostic geometry sphere
    std::shared_ptr<Text> m_text;   ///< Diagnostic text
};