////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - BVHTree.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "BVHTree.h"
#include "collisionmesh.h"
#include "partition.h"
#include <assert.h>

namespace
{
    const float PARITION_SIZE = 45.0f;   ///< Initial dimensions of the root partitions
    const float GROUND_HEIGHT = -23.0f;  ///< Height of the ground plane
    const int CUBE_POINTS = 8;           ///< Number of corners in a cube
    const int SQUARE_POINTS = 4;         ///< Number of corners in a square
    const int MAX_LEVEL = 3;             ///< Number of levels allowed from the root
}

BVHTree::BVHTree(std::shared_ptr<Engine> engine) :
    m_iteratorFn(nullptr),
    m_engine(engine),
    m_tree(new Partition())
{
}

BVHTree::~BVHTree()
{
}

void BVHTree::BuildInitialTree()
{
    const float size = PARITION_SIZE;
    const D3DXVECTOR3 offset(-size / 2.0f, GROUND_HEIGHT, -size / 2.0f);

    m_tree->AddChild(size, D3DXVECTOR3(-size, size, -size) + offset);
    m_tree->AddChild(size, D3DXVECTOR3(0.0, size, -size)   + offset);
    m_tree->AddChild(size, D3DXVECTOR3(0.0, size, -size)   + offset);
    m_tree->AddChild(size, D3DXVECTOR3(-size, size, 0.0)   + offset);
    m_tree->AddChild(size, D3DXVECTOR3(0.0, size, 0.0)     + offset);
    m_tree->AddChild(size, D3DXVECTOR3(-size, size, size)  + offset);
    m_tree->AddChild(size, D3DXVECTOR3(0.0, size, size)    + offset);
    m_tree->AddChild(size, D3DXVECTOR3(size, size, size)   + offset);
    m_tree->AddChild(size, D3DXVECTOR3(size, size, 0.0)    + offset);
    m_tree->AddChild(size, D3DXVECTOR3(size, size, -size)  + offset);

    m_tree->ModifyChildren(std::bind(
        &BVHTree::GenerateChildren, this, std::placeholders::_1));
}

void BVHTree::GenerateChildren(std::unique_ptr<Partition>& parent)
{
    if(parent->GetLevel() != MAX_LEVEL)
    {
        // Center/size of child partition is half of parent
        const float size = parent->GetSize() * 0.5f;
        const D3DXVECTOR3 offset((parent->GetMinBounds()
            + parent->GetMaxBounds()) * 0.5f);

        parent->AddChild(size, D3DXVECTOR3(-size, size, -size) + offset);
        parent->AddChild(size, D3DXVECTOR3(0.0, size, -size)   + offset);
        parent->AddChild(size, D3DXVECTOR3(-size, 0.0, -size)  + offset);
        parent->AddChild(size, D3DXVECTOR3(0.0, 0.0, -size)    + offset);
        parent->AddChild(size, D3DXVECTOR3(-size, size, 0.0)   + offset);
        parent->AddChild(size, D3DXVECTOR3(0.0, size, 0.0)     + offset);
        parent->AddChild(size, D3DXVECTOR3(-size, 0.0, 0.0)    + offset);
        parent->AddChild(size, D3DXVECTOR3(0.0, 0.0, 0.0)      + offset);

        parent->ModifyChildren(std::bind(
            &BVHTree::GenerateChildren, this, std::placeholders::_1));
    }
}

Partition* BVHTree::FindPartition(CollisionMesh& object, Partition& partition)
{
    if(partition.GetLevel() == MAX_LEVEL)
    {
        // No more levels of children
        return &partition;
    }
    else
    {
        // Look through children and see if it fits in at least one
        // If it fits in more than one, parent is desired partition
        Partition* chosenChild = nullptr;
        const auto& children = partition.GetChildren();
        bool inMultiplePartitions = false;

        for(const std::unique_ptr<Partition>& child : children)
        {
            if(IsCornerInsidePartition(object, *child))
            {
                if(!chosenChild)
                {
                    chosenChild = child.get();
                }
                else
                {
                    inMultiplePartitions = true;
                    break;
                }
            }
        }

        if(!chosenChild)
        {
            return m_tree.get();
        }
        else if(inMultiplePartitions)
        {
            return &partition;
        }
        return FindPartition(object, *chosenChild);
    }
    return nullptr;
}

bool BVHTree::IsPointInsidePartition(const D3DXVECTOR3& point, const Partition& partition) const
{
    const auto& minBounds = partition.GetMinBounds();
    const auto& maxBounds = partition.GetMaxBounds();
    return point.x > minBounds.x && point.x < maxBounds.x &&
           point.y < minBounds.y && point.y > maxBounds.y &&
           point.z > minBounds.z && point.z < maxBounds.z;
}

bool BVHTree::IsAllInsidePartition(const CollisionMesh& object, const Partition& partition) const
{
    const std::vector<D3DXVECTOR3>& oabb = object.GetOABB();
    for(const D3DXVECTOR3& point : oabb)
    {
        if(!IsPointInsidePartition(point, partition))
        {
            return false;
        }
    }
    return true;
}

bool BVHTree::IsCornerInsidePartition(const CollisionMesh& object, const Partition& partition) const
{
    const std::vector<D3DXVECTOR3>& oabb = object.GetOABB();
    for(const D3DXVECTOR3& point : oabb)
    {
        if(IsPointInsidePartition(point, partition))
        {
            return true;
        }
    }
    return false;
}

void BVHTree::RemoveObject(CollisionMesh& object)
{
    object.GetPartition()->RemoveNode(object);
    object.SetPartition(nullptr);
}

void BVHTree::UpdateObject(CollisionMesh& object)
{
    Partition* partition = object.GetPartition();
    Partition* newPartition = nullptr;
    assert(partition);

    if(!IsAllInsidePartition(object, *partition))
    {
        // Move upwards until object is fully inside a single partition
        Partition* parent = partition->GetParent();
        while(parent && !IsAllInsidePartition(object, *parent))
        {
            parent = parent->GetParent();
        }

        // Will be in found partition or one of the children
        newPartition = FindPartition(object, parent ? *parent : *m_tree);
    }
    else
    {
        // Can only be in partition and partition's children
        newPartition = FindPartition(object, *partition);
    }

    assert(newPartition);
    if(newPartition != partition)
    {
        // connect object and new partition together
        partition->RemoveNode(object);
        newPartition->AddNode(object);
        object.SetPartition(newPartition);
    }
}

void BVHTree::AddObject(CollisionMesh& object)
{
    Partition* partition = FindPartition(object, *m_tree);
    assert(partition);    

    // connect object and new partition together
    partition->AddNode(object);
    object.SetPartition(partition);
}

void BVHTree::SetIteratorFunction(IterateTreeFn iteratorFn)
{
    m_iteratorFn = iteratorFn;
}

void BVHTree::IterateTree(CollisionMesh& node)
{
    if(m_iteratorFn)
    {
        auto& partition = *node.GetPartition();
        IterateUpTree(node, partition);
        IterateDownTree(node, partition);
    }
}

void BVHTree::IterateUpTree(CollisionMesh& node, Partition& partition)
{
    auto& nodes = partition.GetNodes();
    for(unsigned int i = 0; i < nodes.size(); ++i)
    {
        m_iteratorFn(*nodes[i], node);
    }

    if(partition.GetParent())
    {
        IterateUpTree(node, *partition.GetParent());
    }
}

void BVHTree::IterateDownTree(CollisionMesh& node, Partition& partition)
{
    const auto& children = partition.GetChildren();
    for(const std::unique_ptr<Partition>& child : children)
    {
        auto& nodes = child->GetNodes();
        for(unsigned int i = 0; i < nodes.size(); ++i)
        {
            m_iteratorFn(*nodes[i], node);
        }

        IterateDownTree(node, *child);
    }
}

void BVHTree::RenderDiagnostics()
{
    if(m_engine->diagnostic()->AllowDiagnostics(Diagnostic::PARTITION))
    {
        int nodeCount = static_cast<int>(m_tree->GetNodes().size());

        const auto& children = m_tree->GetChildren();
        for(const std::unique_ptr<Partition>& child : children)
        {
            nodeCount += RenderPartition(child);
        }

        m_engine->diagnostic()->UpdateText(Diagnostic::PARTITION,
            "NodeCount", Diagnostic::WHITE, StringCast(nodeCount));
    }
}

int BVHTree::RenderPartition(const std::unique_ptr<Partition>& partition)
{
    int nodeCount = 0;

    // Render all children of this partition
    const auto& children = partition->GetChildren();
    for(const std::unique_ptr<Partition>& child : children)
    {
        nodeCount += RenderPartition(child);
    }

    // Only render the root children and onwards if they have nodes
    if(partition->GetLevel() > 0 && partition->HasNodes())
    {
        const float size = partition->GetSize();
        const D3DXVECTOR3 minBounds = partition->GetMinBounds();
        std::array<D3DXVECTOR3, CUBE_POINTS> corners =
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
        auto colour = partition->GetColor();

        m_engine->diagnostic()->UpdateText(Diagnostic::PARTITION, id, colour,
            StringCast(partition->GetNodes().size()), true);

        for(int i = 0, j = SQUARE_POINTS; i < SQUARE_POINTS; ++i, ++j)
        {
            m_engine->diagnostic()->UpdateLine(Diagnostic::PARTITION, 
                id + StringCast(i) + "1", colour, 
                corners[i], corners[i+1 >= SQUARE_POINTS ? 0 : i+1]);
            
            m_engine->diagnostic()->UpdateLine(Diagnostic::PARTITION, 
                id + StringCast(i) + "2", colour, 
                corners[j], corners[j+1 >= CUBE_POINTS ? SQUARE_POINTS : j+1]);
            
            m_engine->diagnostic()->UpdateLine(Diagnostic::PARTITION,
                id + StringCast(i) + "3", colour, 
                corners[i], corners[j]);
        }
    }
    return nodeCount + static_cast<int>(partition->GetNodes().size());
}