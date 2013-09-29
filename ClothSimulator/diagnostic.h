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
        BLUE,
        YELLOW,
        GREEN
    };

    /**
    * Available diagnostic groups
    */
    enum Group
    {
        GENERAL,
        CLOTH,
        OCTREE,
        MAX_GROUPS
    };

    /**
    * Constructor
    */
    Diagnostic();

    /**
    * Destructor
    */
    ~Diagnostic();

    /**
    * Initialise the use of diagnostics 
    * @param d3ddev The directX device
    * @param boundsShader The shader to apply to diagnostics meshes
    */
    void Initialise(LPDIRECT3DDEVICE9 d3ddev, 
        std::shared_ptr<Shader> boundsShader);

    /**
    * Toggles whether diagnostics are on
    * @param the diagnostic group to toggle
    */
    void ToggleDiagnostics(Group group);

    /**
    * @return whether diagnostics are on
    * @param the diagnostic group to query
    */
    bool AllowDiagnostics(Group group);

    /**
    * Toggles whether text diagnostics are on
    */
    void ToggleText();

    /**
    * @return whether text diagnostics are on
    */
    bool AllowText();

    /**
    * Adds a sphere for diagnostic rendering. 
    * Will only add once per id and update each call
    * @param group The group the diagnostics belong to
    * @param id The id of the sphere
    * @param color The colour of the sphere
    * @param position The poition in world coordinates
    * @param radius The radius of the sphere
    */
    void UpdateSphere(Group group, const std::string& id, 
        Colour color, const D3DXVECTOR3& position, float radius);

    /**
    * Adds text for diagnostic rendering.
    * Will only add once per id and update each call
    * @param id The id of the text
    * @param color The colour of the text
    * @param text The text to draw
    */
    void UpdateText(const std::string& id, Colour color, const std::string& text);

    /**
    * Adds text for diagnostic rendering.
    * Will only add once per id and update each call
    * @param id The id of the text
    * @param color The colour of the text
    * @param increaseCounter Whether to increase the counter or not
    */
    void UpdateText(const std::string& id, Colour color, bool increaseCounter);

    /**
    * Adds a cylinder line for diagnostic rendering.
    * Will only add once per id and update each call
    * @param group The group the diagnostics belong to
    * @param id The id of the line
    * @param color The colour of the line
    * @param start The start position in world coordinates
    * @param end The end position in world coordinates
    */
    void UpdateLine(Group group, const std::string& id, 
        Colour color, const D3DXVECTOR3& start, const D3DXVECTOR3& end);

    /**
    * Draws all 3D diagnostics
    * @param projection The projection matrix
    * @param view The view matrix
    */
    void DrawAllObjects(const Matrix& projection, const Matrix& view);

    /**
    * Draws all 2D diagnostics
    */
    void DrawAllText();

private: 

    /**
    * Prevent copying
    */
    Diagnostic(const Diagnostic&);
    Diagnostic& operator=(const Diagnostic&);

    /**
    * Diagnostic text data
    */
    struct DiagText
    {
        D3DXVECTOR3 color; ///< Colour of the text
        std::string text;  ///< Actual text to display
        int counter;       ///< Optional counter for text
    };

    /**
    * Diagnostic sphere data
    */
    struct DiagSphere
    {
        D3DXVECTOR3 color;  ///< Colour of the object
        Transform world;    ///< Matrix for sphere
        bool draw;          ///< Whether to render the sphere
    };

    /**
    * Diagnostic line data
    */
    struct DiagLine
    {
        D3DXVECTOR3 color;  ///< Colour of the object
        Matrix world;       ///< Matrix for line
        bool draw;          ///< Whether to render the line
    };

    typedef std::string KeyType;
    typedef std::unordered_map<KeyType, DiagSphere> SphereMap;
    typedef std::unordered_map<KeyType, DiagLine> LineMap;

    /**
    * Holds a map of spheres/lines for the diagnostic group
    */
    struct DiagGroup
    {
        bool render;           ///< Whether to render the group
        SphereMap spheremap;   ///< Map of sphere diagnostics
        LineMap linemap;       ///< Map of cylinder diagnostics
        DiagGroup();           ///< Constructor
    };

    typedef std::vector<DiagGroup> GroupVector;
    typedef std::unordered_map<KeyType, DiagText> TextMap;
    typedef std::unordered_map<Colour, D3DXVECTOR3> ColorMap;

    GroupVector m_groupvector;        ///< Vector of groups of geometry diagnostics
    TextMap m_textmap;                ///< Map of text diagnostics
    ColorMap m_colourmap;             ///< Easy access diagnostic colours
    LPDIRECT3DDEVICE9 m_d3ddev;       ///< DirectX Device
    LPD3DXMESH m_sphere;              ///< Diagnostic geometry sphere
    LPD3DXMESH m_cylinder;            ///< Diagnostic geometry sphere
    std::shared_ptr<Text> m_text;     ///< Diagnostic text
    std::shared_ptr<Shader> m_shader; ///< Global diagnostic mesh shader
    bool m_showText;                  ///< Whether text diagnostics are visible or not
};