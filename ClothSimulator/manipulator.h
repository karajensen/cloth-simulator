////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"

class Mesh;
class ShaderManager;
class Picking;
struct RenderCallbacks;

/**
* Tools for manipulating selected objects
*/
class Manipulator
{
public:

    typedef std::shared_ptr<Mesh> MeshPtr;

    /**
    * Available tools for manipulating
    */
    enum ToolType
    {
        MOVE,
        ROTATE,
        SCALE,
        ANIMATE,
        MAX_TOOLS,
        NONE
    };

    /**
    * Constructor
    * @param d3ddev The directx device
    * @param callbacks The callbacks for rendering a mesh
    */
    Manipulator(LPDIRECT3DDEVICE9 d3ddev, const RenderCallbacks& callbacks);

    /**
    * Renders the currently selected tool
    * @param projection The projection matrix for the camera
    * @param view The view matrix for the camera
    * @param position The position of the camera in world space
    * @param selectedMesh The selected mesh the tool is working on
    */
    void Render(const Transform& projection, const Transform& view,
        const D3DXVECTOR3& position, const MeshPtr& selectedMesh);

    /**
    * Updates the tool through mouse picking and movement
    * @param input The picking input object
    * @param selectedMesh The selected mesh the tool is working on
    */
    void Update(Picking& input, MeshPtr selectedMesh);

    /**
    * Switches the currently active mesh tool
    * @param type The tool to change to
    */
    void ChangeTool(ToolType type);

private:

    /**
    * The axis associated with a tool mesh
    */
    enum ToolAxis
    {
        X_AXIS,
        Y_AXIS,
        Z_AXIS,
        MAX_AXIS,
        NO_AXIS
    };

    struct Tool
    {
        /**
        * Constructor
        * @param d3ddev The directx device
        * @param name The name of the tool
        * @param index The index of the tool
        * @param callbacks The callbacks for rendering a mesh
        */
        Tool(LPDIRECT3DDEVICE9 d3ddev, const std::string& name, 
            int index, const RenderCallbacks& callbacks);

        std::vector<MeshPtr> axis; ///< meshes for the tool
        ToolAxis selectedAxis; ///< currently selected axis
    };

    /**
    * Prevent copying
    */
    Manipulator(const Manipulator&);
    Manipulator& operator=(const Manipulator&);

    /**
    * @return a string description of the given tool
    */
    std::string GetDescription(ToolType type) const;

    std::vector<std::shared_ptr<Tool>> m_tools; ///< all usable tool
    ToolType m_selectedTool; ///< Currently selected tool
};