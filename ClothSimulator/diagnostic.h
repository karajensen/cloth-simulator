////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - diagnostic.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

#include <unordered_map>
#include <array>

class Text;

/**
* Diagnostic drawing class
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
        GREEN,
        CYAN,
        MAGENTA,
        BLACK,
        PURPLE,
        MAX_COLORS
    };

    /**
    * Available diagnostic groups
    */
    enum Group
    {
        MESH,
        CLOTH,
        OCTREE,
        TEXT,
        COLLISION,
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
    void Initialise(LPDIRECT3DDEVICE9 d3ddev, LPD3DXEFFECT boundsShader);

    /**
    * Toggles whether diagnostics are on
    * @param group the diagnostic group to toggle
    */
    void ToggleDiagnostics(Group group);

    /**
    * @param wireframe Set whether wireframe is on or not
    */
    void SetWireframe(bool wireframe);

    /**
    * @return whether wireframe is enabled or not
    */
    bool AllowWireframe() const;

    /**
    * @return whether diagnostics are on
    * @param group The diagnostic group to query
    */
    bool AllowDiagnostics(Group group) const;

    /**
    * Adds a sphere for diagnostic rendering. 
    * Will only add once per id and update each call
    * @param group The group the diagnostics belong to
    * @param id The id of the sphere
    * @param color The colour of the sphere
    * @param position The poition in world coordinates
    * @param radius The radius of the sphere
    */
    void UpdateSphere(Group group, 
                      const std::string& id, 
                      Colour color, 
                      const D3DXVECTOR3& position, 
                      float radius);

    /**
    * Adds text for diagnostic rendering.
    * Will only add once per id and update each call
    * @param group The group the diagnostics belong to
    * @param id The id of the text
    * @param color The colour of the text
    * @param text The text to draw
    * @param cleardraw Whether to remove the text once drawn or not
    */
    void UpdateText(Group group, 
                    const std::string& id,
                    Colour color, 
                    const std::string& text, 
                    bool cleardraw = false);

    /**
    * Adds text for diagnostic rendering.
    * Will only add once per id and update each call
    * @param group The group the diagnostics belong to
    * @param id The id of the text
    * @param color The colour of the text
    * @param increaseCounter Whether to increase the counter or not
    * @param cleardraw Whether to remove the text once drawn or not
    */
    void UpdateText(Group group, 
                    const std::string& id,
                    Colour color, 
                    bool increaseCounter, 
                    bool cleardraw = false);

    /**
    * Adds a cylinder line for diagnostic rendering.
    * Will only add once per id and update each call
    * @param group The group the diagnostics belong to
    * @param id The id of the line
    * @param color The colour of the line
    * @param start The start position in world coordinates
    * @param end The end position in world coordinates
    */
    void UpdateLine(Group group, 
                    const std::string& id, 
                    Colour color, 
                    const D3DXVECTOR3& start, 
                    const D3DXVECTOR3& end);

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

    /**
    * @param color The color type to query
    * @return the color value for the given color type
    */
    const D3DXVECTOR3& GetColor(Colour color);

private: 

    /**
    * Prevent copying
    */
    Diagnostic(const Diagnostic&);
    Diagnostic& operator=(const Diagnostic&);

    /**
    * Renders a 3D object
    * @param effect The shader effect to render with
    * @param mesh The mesh to render
    * @param color The color to render the mesh in
    * @param world The mesh world matrix
    * @param projection The projection matrix
    * @param view The view matrix
    */
    void RenderObject(LPD3DXEFFECT effect, 
                      LPD3DXMESH mesh, 
                      const D3DXVECTOR3& color, 
                      const Matrix& world,
                      const Matrix& projection, 
                      const Matrix& view);

    /**
    * Diagnostic text data
    */
    struct DiagText
    {
        Colour color;      ///< Colour of the text
        std::string text;  ///< Actual text to display
        int counter;       ///< Optional counter for text
        bool draw;         ///< Whether to render the text
        bool cleardraw;    ///< Whether to set draw to false once rendered
    };

    /**
    * Diagnostic sphere data
    */
    struct DiagSphere
    {
        Colour color;     ///< Colour of the object
        Transform world;  ///< Matrix for sphere
        bool draw;        ///< Whether to render the sphere
    };

    /**
    * Diagnostic line data
    */
    struct DiagLine
    {
        Colour color;  ///< Colour of the object
        Matrix world;  ///< Matrix for line
        bool draw;     ///< Whether to render the line
    };

    typedef std::string KeyType;
    typedef std::unordered_map<KeyType, DiagSphere> SphereMap;
    typedef std::unordered_map<KeyType, DiagLine> LineMap;
    typedef std::unordered_map<KeyType, DiagText> TextMap;

    /**
    * Holds a map of spheres/lines for the diagnostic group
    */
    struct DiagGroup
    {
        bool render;           ///< Whether to render the group
        TextMap textmap;       ///< Map of text diagnostics
        SphereMap spheremap;   ///< Map of sphere diagnostics
        LineMap linemap;       ///< Map of cylinder diagnostics
        DiagGroup();           ///< Constructor
    };

    typedef std::vector<DiagGroup> GroupVector;
    typedef std::vector<D3DXVECTOR3> ColorVector;

private:

    bool m_wireframe;                 ///< Whether or not wireframe is being rendered
    GroupVector m_groupvector;        ///< Vector of groups of geometry diagnostics
    ColorVector m_colours ;           ///< Available diagnostic colours
    LPDIRECT3DDEVICE9 m_d3ddev;       ///< DirectX Device
    LPD3DXMESH m_sphere;              ///< Diagnostic geometry sphere
    LPD3DXMESH m_cylinder;            ///< Diagnostic geometry cylinder/line
    std::unique_ptr<Text> m_text;     ///< Diagnostic text
    LPD3DXEFFECT m_shader;            ///< Diagnostic mesh shader
};