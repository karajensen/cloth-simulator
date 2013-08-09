/****************************************************************
* Kara Jensen (mail@karajensen.com) 
* Scene mesh manipulation manager
*****************************************************************/

#pragma once

#include "common.h"
#include <queue>
#include "GUICallbacks.h"

using namespace GUI;
class Shader;
class ClothSolver;
class Mesh;
class Picking;

class Scene
{
public:

    enum Tool
    {
        NONE,
        MOVE,
        ROTATE,
        SCALE,
        ANIMATE
    };

    enum Object
    {
        BOX,
        SPHERE,
        CYLINDER,
        MAX_OBJECT
    };

    /**
    * Constructor
    * @param the directx device
    * @param the shader for the meshes
    */
    Scene(LPDIRECT3DDEVICE9 d3ddev, std::shared_ptr<Shader> meshshader);

    /**
    * Draws all scene meshes
    * @param the camera position
    * @param the camera projection matrix
    * @param the camera view matrix
    */
    void Draw(const D3DXVECTOR3& position, const Transform& projection, const Transform& view);

    /**
    * Draws all scene meshe collisions
    * @param the camera projection matrix
    * @param the camera view matrix
    */
    void DrawCollision(const Transform& projection, const Transform& view);

    /**
    * Removes the currently selected object if possible
    */
    void RemoveObject();

    /**
    * Removes all objects in the scene
    */
    void RemoveScene();

    /**
    * Switches the currently active mesh tool
    * @param the tool to change to
    */
    void ChangeTool(Tool tool);

    /**
    * Adds an object to the scene
    * @param the object to add
    */
    void AddObject(Object object);

    /**
    * Tests all scene objects for mouse picking
    * @param the mouse picking object
    */
    void MousePickingTest(Picking& input);

    /**
    * Solves the collision between scene objects and the cloth
    * @param the cloth solver for object-cloth collisions
    */
    void SolveClothCollision(ClothSolver& solver);

    /**
    * Set the visibility of the collision mesh
    * @param whether the collision mesh if visible or not
    */
    void SetCollisionVisibility(bool visible);

    /**
    * Enables/Disables the gui buttons for mesh creation
    * @param a callback from the GUI to set whether meshes are enabled or not
    */
    void SetMeshEnableCallback(SetFlag enableCreation);

private:

    /**
    * Sets the selected mesh
    * @param the selected mesh
    */
    void SetSelectedMesh(const Mesh* mesh);

    typedef std::shared_ptr<Mesh> MeshPtr;

    LPDIRECT3DDEVICE9 m_d3ddev; ///< DirectX device
    std::queue<unsigned int> m_open; ///< Indices for the avaliable meshes
    std::vector<MeshPtr> m_meshes;  ///< Changable meshes in the scene
    std::vector<MeshPtr> m_templates; ///< Mesh templates for creating mesh instances
    Tool m_selectedTool; ///< Currently selected tool
    int m_selectedMesh; ///< Currently selected object
    SetFlag m_enableCreation; ///< Callback for enabled/disabling gui mesh creation

};
