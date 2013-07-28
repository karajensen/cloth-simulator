#include "meshmanager.h"
#include "mesh.h"
#include "cloth.h"
#include "picking.h"

namespace
{
    const int MAX_OBJECTS = 20;
    const int GROUND_INDEX = 0;
    const std::string MODEL_FOLDER(".\\Resources\\Models\\");
}

MeshManager::MeshManager(LPDIRECT3DDEVICE9 d3ddev, std::shared_ptr<Shader> meshshader) :
    m_selectedTool(NONE),
    m_d3ddev(d3ddev),
    m_selectedMesh(NO_INDEX)
{
    m_meshes.resize(MAX_OBJECTS);
    //std::generate(m_meshes.begin(), m_meshes.end(), [&](){ return MeshManager::MeshPtr(new Mesh()); });

    for(unsigned int i = 1; i < m_meshes.size(); ++i)
    {
        m_open.push(i);
    }

    // Create the ground plain
    m_meshes[GROUND_INDEX].reset(new Mesh());
    if(!m_meshes[GROUND_INDEX]->Load(d3ddev, MODEL_FOLDER+"ground.obj", meshshader))
    {
        Diagnostic::ShowMessage("Ground plain failed to load");
    }
    m_meshes[GROUND_INDEX]->SetPickable(false);
    m_meshes[GROUND_INDEX]->SetPosition(0,-20,0);
    m_meshes[GROUND_INDEX]->CreateCollision(d3ddev,150.0f,1.0f,150.0f);
}

void MeshManager::ChangeTool(Tool tool)
{
    m_selectedTool = tool;
}

bool MeshManager::AddObject(Object object)
{
    if(m_open.empty())
    {
        return false;
    }

    unsigned int index = m_open.front();
    m_open.pop();

    switch(object)
    {
    case BOX:
        break;
    case SPHERE:
        break;
    case CYLINDER:
        break;
    }

    return true;
}

void MeshManager::RemoveScene()
{
    m_selectedMesh = NO_INDEX;
}

void MeshManager::RemoveObject()
{
    if(m_selectedMesh != NO_INDEX)
    {
        m_open.push(m_selectedMesh);
    }
}

void MeshManager::Draw(const D3DXVECTOR3& position, const Transform& projection, const Transform& view)
{
    std::for_each(m_meshes.begin(), m_meshes.end(), [&](MeshPtr& mesh)
    {
        if(mesh.get() && mesh->IsVisible())
        {
            mesh->DrawMesh(position, projection, view);
        }
    });
}

void MeshManager::DrawCollision(const Transform& projection, const Transform& view)
{
    std::for_each(m_meshes.begin(), m_meshes.end(), [&](MeshPtr& mesh)
    {
        if(mesh.get() && mesh->IsVisible())
        {
            mesh->DrawCollision(projection, view);
        }
    });
}

void MeshManager::MousePickingTest(Picking& input)
{
    std::for_each(m_meshes.begin(), m_meshes.end(), [&](MeshPtr& mesh)
    {
        if(mesh.get() && mesh->IsVisible())
        {
            mesh->MousePickingTest(input);
        }
    });
}

void MeshManager::SolveClothCollision(Cloth& cloth)
{
    std::for_each(m_meshes.begin(), m_meshes.end(), [&](MeshPtr& mesh)
    {
        if(mesh.get() && mesh->IsVisible())
        {
            cloth.SolveCollision(mesh->GetCollision());
        }
    });
}

void MeshManager::SetCollisionVisibility(bool visible)
{
    std::for_each(m_meshes.begin(), m_meshes.end(), [&](MeshPtr& mesh)
    {
        if(mesh.get())
        {
            mesh->SetCollisionVisibility(visible);
        }
    });
}