////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - diagnostic.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <unordered_map>
#include <array>
#include "common.h"

class Shader;
class Text;

/**
* Diagnostic static drawing class
*/
class Diagnostic
{
public:

    /**
    * Available diagnostic colours
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
    * Initialise the use of diagnostics 
    * @param d3ddev The directX device
    * @param boundsShader The shader to apply to diagnostics meshes
    */
    static void Initialise(LPDIRECT3DDEVICE9 d3ddev, 
        std::shared_ptr<Shader> boundsShader);

    /**
    * Shows a popup box with a message
    * @param message The message to show
    */
    static void ShowMessage(const std::string& message);

    /**
    * Toggles whether diagnostics are on
    */
    static void ToggleDiagnostics();

    /**
    * @return whether diagnostics are on
    */
    static bool AllowDiagnostics();

    /**
    * Toggles whether text diagnostics are on
    */
    static void ToggleText();

    /**
    * @return whether text diagnostics are on
    */
    static bool AllowText();

    /**
    * Adds a sphere for diagnostic rendering. 
    * Will only add once per id and update each call
    * @param id The id of the sphere
    * @param color The colour of the sphere
    * @param position The poition in world coordinates
    * @param radius The radius of the sphere
    */
    static void UpdateSphere(const std::string& id, Colour color, 
        const D3DXVECTOR3& position, float radius);

    /**
    * Adds text for diagnostic rendering.
    * Will only add once per id and update each call
    * @param id The id of the text
    * @param color The colour of the text
    * @param text The text to draw
    */
    static void UpdateText(const std::string& id, Colour color, const std::string& text);

    /**
    * Adds text for diagnostic rendering.
    * Will only add once per id and update each call
    * @param id The id of the text
    * @param color The colour of the text
    * @param increaseCounter Whether to increase the counter or not
    */
    static void UpdateText(const std::string& id, Colour color, bool increaseCounter);

    /**
    * Adds a cylinder line for diagnostic rendering.
    * Will only add once per id and update each call
    * @param id The id of the line
    * @param color The colour of the line
    * @param start The start position in world coordinates
    * @param end The end position in world coordinates
    */
    static void UpdateLine(const std::string& id, Colour color, 
        const D3DXVECTOR3& start, const D3DXVECTOR3& end);

    /**
    * Draws all 3D diagnostics
    * @param projection The projection matrix
    * @param view The view matrix
    */
    static void DrawAllObjects(const Transform& projection, const Transform& view);

    /**
    * Draws all 2D diagnostics
    */
    static void DrawAllText();

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

    /**
    * Constructor
    * @param d3ddev The directX device
    * @param boundsShader The shader to render diagnostics
    */
    Diagnostic(LPDIRECT3DDEVICE9 d3ddev, std::shared_ptr<Shader> boundsShader);

    /**
    * Prevent copying
    */
    Diagnostic(const Diagnostic&);
    Diagnostic& operator=(const Diagnostic&);

    typedef std::string KeyType;
    typedef std::unordered_map<KeyType, DiagSphere> SphereMap;
    typedef std::unordered_map<KeyType, DiagLine> LineMap;
    typedef std::unordered_map<KeyType, DiagText> TextMap;
    typedef std::unordered_map<Colour, D3DXVECTOR3> ColorMap;

    TextMap m_textmap;       ///< Map of text diagnostics
    SphereMap m_spheremap;   ///< Map of sphere diagnostics
    LineMap m_linemap;       ///< Map of cylinder diagnostics
    ColorMap m_colourmap;    ///< Easy access diagnostic colours

    LPDIRECT3DDEVICE9 m_d3ddev;       ///< DirectX Device
    LPD3DXMESH m_sphere;              ///< Diagnostic geometry sphere
    LPD3DXMESH m_cylinder;            ///< Diagnostic geometry sphere
    std::shared_ptr<Text> m_text;     ///< Diagnostic text
    std::shared_ptr<Shader> m_shader; ///< Global diagnostic mesh shader
    bool m_showText;                  ///< Whether text diagnostics are visible or not
    bool m_showDiagnostics;           ///< Whether diagnostics are visible or not

    static std::shared_ptr<Diagnostic> sm_diag; ///< Diagnostic singleton pointer
};