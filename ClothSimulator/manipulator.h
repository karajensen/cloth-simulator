////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - manipulator.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "callbacks.h"

class Mesh;
class ShaderManager;
class Picking;

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
    * @param engine Callbacks from the rendering engine
    */
    explicit Manipulator(EnginePtr engine);

    /**
    * Renders the currently selected tool
    * @param projection The projection matrix for the camera
    * @param view The view matrix for the camera
    * @param position The position of the camera in world space
    * @param selectedMesh The selected mesh the tool is working on
    */
    void Render(const Matrix& projection, const Matrix& view,
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
    * @param direction The mouse movement direction
    * @param world The camera world matrix
    * @param invProjection The camera inverse projection matrix
    * @param pressed Whether the mouse is pressed or not
    * @param deltatime Delta time for the simulation
    */
    void UpdateState(MeshPtr mesh, const D3DXVECTOR2& direction, 
        const Matrix& world, const Matrix& invProjection,
        bool pressed, float deltatime);

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
        * @param name The name of the tool
        * @param index The index of the tool
        * @param engine Callbacks from the rendering engine
        */
        Tool(const std::string& name, int index, EnginePtr engine);

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

    /**
    * Render an animation point/line sphere
    * @param effect The shader to render the mesh with
    * @param projection The projection matrix for the camera
    * @param view The view matrix for the camera
    * @param color The color to render the sphere in
    * @param world The world matrix for scaling/position
    */
    void RenderSphere(LPD3DXEFFECT effect,
        const Matrix& projection, const Matrix& view,
        const D3DXVECTOR3& color, const Transform& world);

    /**
    * Translates the selected mesh
    * @param mesh The mesh to manipulate
    * @param value The value to manipulate by
    */
    void TranslateMesh(MeshPtr mesh, float value);

    /**
    * Rotates the selected mesh
    * @param mesh The mesh to manipulate
    * @param value The value to manipulate by
    */
    void RotateMesh(MeshPtr mesh, float value);

    /**
    * Scales the selected mesh
    * @param mesh The mesh to manipulate
    * @param value The value to manipulate by
    */
    void ScaleMesh(MeshPtr mesh, float value);

    /**
    * Animates the selected mesh
    * @param mesh The mesh to manipulate
    * @param value The value to manipulate by
    */
    void AnimateMesh(MeshPtr mesh, float value);

    EnginePtr m_engine;                         ///< Callbacks for the rendering engine
    std::vector<std::shared_ptr<Tool>> m_tools; ///< all usable tools
    ToolType m_selectedTool;                    ///< Currently selected tool
    ToolAxis m_selectedAxis;                    ///< currently selected axis
    LPD3DXMESH m_sphere;                        ///< Animation geometry sphere
    std::shared_ptr<Shader> m_shader;           ///< Animation point shader
    bool m_saveAnimation;                       ///< Whether to allow the position to be saved

    D3DXMATRIX proj;
    D3DXMATRIX viewmat;
};