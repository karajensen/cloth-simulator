/****************************************************************
* Kara Jensen (mail@karajensen.com) 
* Tools for manipulating selected objects
*****************************************************************/

#pragma once

#include "common.h"

class Mesh;
class Shader;
class Picking;

class Manipulator
{
public:

    typedef std::shared_ptr<Mesh> MeshPtr;

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
    * @param the directx device
    * @param the shader to use for the tools
    */
    Manipulator(LPDIRECT3DDEVICE9 d3ddev, std::shared_ptr<Shader> toolshader);

    /**
    * Renders the currently selected tool
    * @param the projection matrix for the camera
    * @param the view matrix for the camera
    * @param the position of the camera in world space
    * @param the selected mesh the tool is working on
    */
    void Render(const Transform& projection, const Transform& view,
        const D3DXVECTOR3& position, const Manipulator::MeshPtr& selectedMesh);

    /**
    * Updates the tool through mouse picking and movement
    * @param the picking input object
    * @param the selected mesh the tool is working on
    */
    void Update(Picking& input, MeshPtr selectedMesh);

    /**
    * Switches the currently active mesh tool
    * @param the tool to change to
    */
    void ChangeTool(ToolType type);

private:

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
        * @param the directx device
        * @param the name of the tool
        * @param the index of the tool
        * @param the shader to use for the tools
        */
        Tool(LPDIRECT3DDEVICE9 d3ddev, const std::string& name, 
            int index, std::shared_ptr<Shader> toolshader);

        std::vector<MeshPtr> axis; ///< meshes for the tool
        ToolAxis selectedAxis; ///< currently selected axis
    };

    /**
    * @return a string description of the given tool
    */
    std::string GetDescription(ToolType type) const;

    std::vector<std::shared_ptr<Tool>> m_tools; ///< all usable tool
    ToolType m_selectedTool; ///< Currently selected tool
};