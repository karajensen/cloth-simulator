////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - manipulator.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"

class Mesh;
class ShaderManager;
class Picking;
class Input;
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
    */
    void MousePickTest(Picking& input);

    /**
    * Switches the currently active mesh tool
    * @param type The tool to change to
    */
    void ChangeTool(ToolType type);

    /**
    * Updates the state of the manipulator
    * @param mesh the currently selected mesh
    * @param input The simulation input
    */
    void UpdateState(MeshPtr mesh, const Input& input, 
        const Transform& view, const Transform& projection);

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
    };

    /**
    * Prevent copying
    */
    Manipulator(const Manipulator&);
    Manipulator& operator=(const Manipulator&);

    /**
    * @return a string description of the given axis
    */
    std::string GetDescription(ToolAxis axis) const;

    /**
    * @return a string description of the given tool
    */
    std::string GetDescription(ToolType type) const;

    std::vector<std::shared_ptr<Tool>> m_tools; ///< all usable tool
    ToolType m_selectedTool; ///< Currently selected tool
    ToolAxis m_selectedAxis; ///< currently selected axis
};