////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - octree.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "octree.h"
#include "collisionmesh.h"
#include "partition.h"
#include <assert.h>

namespace
{
    const int ROOT_PARTITIONS = 9; ///< Number of partitions for the root
    const int PARTITIONS = 8; ///< Number of partitions allowed within a parent
    const float PARITION_SIZE = 45.0f; ///< Initial dimensions of the root partitions
    const float GROUND_HEIGHT = -23.0f; ///< Height of the ground plane
    const int CORNERS = 8; ///< Number of corners in a cube
    const int MAX_LEVEL = 3; ///< Number of levels allowed from the root
}

Octree::Octree(std::shared_ptr<Engine> engine) :
    m_engine(engine),
    m_octree(new Partition())
{
    BuildInitialTree();
}

Octree::~Octree()
{
}

void Octree::RenderDiagnostics()
{
    if(m_engine->diagnostic()->AllowDiagnostics(Diagnostic::OCTREE))
    {
        const auto& children = m_octree->GetChildren();
        for(const std::unique_ptr<Partition>& child : children)
        {
            RenderPartition(child);
        }
    }
}

void Octree::RenderPartition(const std::unique_ptr<Partition>& partition)
{
    int level = partition->GetLevel();
    const auto& children = partition->GetChildren();
    for(const std::unique_ptr<Partition>& child : children)
    {
        RenderPartition(child);
    }

    if(level >= 0 && partition->HasNodes())
    {
        const D3DXVECTOR3 minBounds = partition->GetMinBounds();
        const float size = partition->GetSize();
        std::array<D3DXVECTOR3, CORNERS> corners =
        {
            // top four corners
            minBounds,
            minBounds + D3DXVECTOR3(size, 0, 0),
            minBounds + D3DXVECTOR3(size, -size, 0),
            minBounds + D3DXVECTOR3(0, -size, 0),
            
            // bottom four corners
            minBounds + D3DXVECTOR3(0, 0, size),
            minBounds + D3DXVECTOR3(size, 0, size),
            minBounds + D3DXVECTOR3(size, -size, size),
            minBounds + D3DXVECTOR3(0, -size, size)
        };
            
        const std::string& id = partition->GetID();
        auto colour = static_cast<Diagnostic::Colour>(min(MAX_LEVEL, level));
        for(unsigned int i = 0; i < CORNERS/2; ++i)
        {
            m_engine->diagnostic()->UpdateLine(Diagnostic::OCTREE, id + StringCast(i)
                + "line1", colour, corners[i], corners[i+1 >= 4 ? 0 : i+1]);
            
            m_engine->diagnostic()->UpdateLine(Diagnostic::OCTREE, id + StringCast(i) 
                + "line2", colour, corners[i+4], corners[i+5 >= CORNERS ? 4 : i+5]);
            
            m_engine->diagnostic()->UpdateLine(Diagnostic::OCTREE, id + StringCast(i) 
                + "line3", colour, corners[i], corners[i+4]);
        }
    }
}

void Octree::BuildInitialTree()
{
    const float size = PARITION_SIZE;
    const D3DXVECTOR3 offset(-size/2.0f, GROUND_HEIGHT, -size/2.0f);

    m_octree->AddChild(size, D3DXVECTOR3(-size, size, -size) + offset, m_octree.get());
    m_octree->AddChild(size, D3DXVECTOR3(0.0, size, -size)   + offset, m_octree.get());
    m_octree->AddChild(size, D3DXVECTOR3(0.0, size, -size)   + offset, m_octree.get());
    m_octree->AddChild(size, D3DXVECTOR3(-size, size, 0.0)   + offset, m_octree.get());
    m_octree->AddChild(size, D3DXVECTOR3(0.0, size, 0.0)     + offset, m_octree.get());
    m_octree->AddChild(size, D3DXVECTOR3(-size, size, size)  + offset, m_octree.get());
    m_octree->AddChild(size, D3DXVECTOR3(0.0, size, size)    + offset, m_octree.get());
    m_octree->AddChild(size, D3DXVECTOR3(size, size, size)   + offset, m_octree.get());
    m_octree->AddChild(size, D3DXVECTOR3(size, size, 0.0)    + offset, m_octree.get());
    m_octree->AddChild(size, D3DXVECTOR3(size, size, -size)  + offset, m_octree.get());

    m_octree->ModifyChildren(std::bind(&Octree::GenerateChildren, this, std::placeholders::_1));

}

void Octree::GenerateChildren(std::unique_ptr<Partition>& parent)
{
    if(parent->GetLevel() != MAX_LEVEL)
    {
        const float size = parent->GetSize() * 0.5f;
        const D3DXVECTOR3 offset((parent->GetMinBounds()
            + parent->GetMaxBounds()) * 0.5f);

        parent->AddChild(size, D3DXVECTOR3(-size, size, -size) + offset, parent.get());
        parent->AddChild(size, D3DXVECTOR3(0.0, size, -size)   + offset, parent.get());
        parent->AddChild(size, D3DXVECTOR3(-size, 0.0, -size)  + offset, parent.get());
        parent->AddChild(size, D3DXVECTOR3(0.0, 0.0, -size)    + offset, parent.get());
        parent->AddChild(size, D3DXVECTOR3(-size, size, 0.0)   + offset, parent.get());
        parent->AddChild(size, D3DXVECTOR3(0.0, size, 0.0)     + offset, parent.get());
        parent->AddChild(size, D3DXVECTOR3(-size, 0.0, 0.0)    + offset, parent.get());
        parent->AddChild(size, D3DXVECTOR3(0.0, 0.0, 0.0)      + offset, parent.get());

        parent->ModifyChildren(std::bind(&Octree::GenerateChildren, this, std::placeholders::_1));
    }
}

void Octree::AddObject(CollisionMesh* object)
{
    Partition* partition = AddToPartition(object, m_octree.get());
    if(!partition)
    {
        ShowMessageBox("Object could not be added to partition");
    }
    object->SetPartition(partition);
}

Partition* Octree::AddToPartition(CollisionMesh* object, Partition* partition)
{
    if(partition->GetLevel() == MAX_LEVEL)
    {
        // No more levels of children
        partition->AddNode(object);
        return partition;
    }
    else
    {
        // Look through children and see if it fits in one
        // If it fits in more than one, make a node of the parent
        Partition* chosenChild = nullptr;
        const auto& children = partition->GetChildren();
        bool inMultiple = false;

        for(const std::unique_ptr<Partition>& child : children)
        {
            if(IsCornerInsidePartition(object, child.get()))
            {
                if(!chosenChild)
                {
                    chosenChild = child.get();
                }
                else
                {
                    inMultiple = true;
                    break;
                }
            }
        }

        assert(chosenChild);
        if(inMultiple)
        {
            partition->AddNode(object);
            return partition;
        }
        else
        {
            return AddToPartition(object, chosenChild);
        }
    }
    return nullptr;
}

void Octree::RemoveObject(CollisionMesh* object)
{
    object->GetPartition()->RemoveNode(object);
    object->SetPartition(nullptr);
}

void Octree::UpdateObject(CollisionMesh* object)
{
    Partition* partition = object->GetPartition();
    if(!IsAllInsidePartition(object, partition))
    {
        partition->RemoveNode(object);
        AddObject(object);
    }
}

bool Octree::IsPointInsidePartition(const D3DXVECTOR3& point, const Partition* partition)
{
    const auto& minBounds = partition->GetMinBounds();
    const auto& maxBounds = partition->GetMaxBounds();
    return point.x > minBounds.x && 
           point.x < maxBounds.x &&
           point.y < minBounds.y && 
           point.y > maxBounds.y &&
           point.z > minBounds.z && 
           point.z < maxBounds.z;
}

bool Octree::IsAllInsidePartition(const CollisionMesh* object, const Partition* partition)
{
    const std::vector<D3DXVECTOR3>& oabb = object->GetOABB();
    for(const D3DXVECTOR3& point : oabb)
    {
        if(!IsPointInsidePartition(point, partition))
        {
            return false;
        }
    }
    return true;
}

bool Octree::IsCornerInsidePartition(const CollisionMesh* object, const Partition* partition)
{
    const std::vector<D3DXVECTOR3>& oabb = object->GetOABB();
    for(const D3DXVECTOR3& point : oabb)
    {
        if(IsPointInsidePartition(point, partition))
        {
            return true;
        }
    }
    return false;
}