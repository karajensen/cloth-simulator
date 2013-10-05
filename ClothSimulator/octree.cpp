////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - octree.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "octree.h"
#include "collisionmesh.h"
#include <assert.h>

namespace
{
    const int ROOT_PARTITIONS = 9; ///< Number of partitions for the root
    const int PARTITIONS = 8; ///< Number of partitions allowed within a parent
    const float PARITION_SIZE = 45.0f; ///< Initial dimensions of the root partitions
    const float GROUND_HEIGHT = -23.0f; ///< Height of the ground plane
    const int CORNERS = 8; ///< Number of corners in a cube
    const int MAX_LEVELS = 3; ///< Number of levels allowed from the root
}

Octree::Octree(EnginePtr engine) :
    m_engine(engine)
{
}

Octree::Partition::Partition(float size, const D3DXVECTOR3& minPoint, PartitionPtr owner) :
    parent(owner),
    id(StringCast(this)),
    minBounds(minPoint)
{
    const D3DXVECTOR3 minToMax(size, -size, size);
    maxBounds = minBounds + minToMax;
    level = owner ? owner->level + 1 : 0;
}

float Octree::Partition::GetSize() const
{
    return fabs(maxBounds.x - minBounds.x);
}

void Octree::RenderDiagnostics()
{
    if(m_engine->diagnostic()->AllowDiagnostics(Diagnostic::OCTREE))
    {
        for(const PartitionPtr& partition : m_octree)
        {
            RenderPartition(partition);
        }
    }
}

void Octree::RenderPartition(const PartitionPtr& partition)
{
    for(const PartitionPtr& child : partition->children)
    {
        RenderPartition(child);
    }

    auto colour = static_cast<Diagnostic::Colour>(min(MAX_LEVELS, partition->level));

    m_engine->diagnostic()->UpdateText(Diagnostic::OCTREE, partition->id,
        colour, StringCast(partition->nodes.size()));

    if(!partition->nodes.empty() || !partition->children.empty())
    {
        const float size = partition->GetSize();
        std::array<D3DXVECTOR3, CORNERS> corners =
        {
            // top four corners
            partition->minBounds,
            partition->minBounds + D3DXVECTOR3(size, 0, 0),
            partition->minBounds + D3DXVECTOR3(size, -size, 0),
            partition->minBounds + D3DXVECTOR3(0, -size, 0),
            
            // bottom four corners
            partition->minBounds + D3DXVECTOR3(0, 0, size),
            partition->minBounds + D3DXVECTOR3(size, 0, size),
            partition->minBounds + D3DXVECTOR3(size, -size, size),
            partition->minBounds + D3DXVECTOR3(0, -size, size)
        };
            
        for(unsigned int i = 0; i < CORNERS/2; ++i)
        {
            m_engine->diagnostic()->UpdateLine(Diagnostic::OCTREE,
                partition->id + StringCast(i) + "line1", colour,
                corners[i], corners[i+1 >= 4 ? 0 : i+1]);
            
            m_engine->diagnostic()->UpdateLine(Diagnostic::OCTREE,
                partition->id + StringCast(i) + "line2", colour,
                corners[i+4], corners[i+5 >= CORNERS ? 4 : i+5]);
            
            m_engine->diagnostic()->UpdateLine(Diagnostic::OCTREE,
                partition->id + StringCast(i) + "line3", colour,
                corners[i], corners[i+4]);
        }
    }
}

void Octree::BuildInitialTree()
{
    assert(m_octree.empty());

    const float size = PARITION_SIZE;
    const D3DXVECTOR3 offset(-size/2.0f, GROUND_HEIGHT, -size/2.0f);

    m_octree.push_back(PartitionPtr(new Partition(size, 
        D3DXVECTOR3(-size, size, -size) + offset)));

    m_octree.push_back(PartitionPtr(new Partition(size, 
        D3DXVECTOR3(0, size, -size) + offset)));

    m_octree.push_back(PartitionPtr(new Partition(size, 
        D3DXVECTOR3(-size, size, 0) + offset)));

    m_octree.push_back(PartitionPtr(new Partition(size, 
        D3DXVECTOR3(0, size, 0) + offset)));
    
    m_octree.push_back(PartitionPtr(new Partition(size, 
        D3DXVECTOR3(-size, size, size) + offset)));
    
    m_octree.push_back(PartitionPtr(new Partition(size, 
        D3DXVECTOR3(0, size, size) + offset)));
    
    m_octree.push_back(PartitionPtr(new Partition(size, 
        D3DXVECTOR3(size, size, size) + offset)));
    
    m_octree.push_back(PartitionPtr(new Partition(size, 
        D3DXVECTOR3(size, size, 0) + offset)));
    
    m_octree.push_back(PartitionPtr(new Partition(size, 
        D3DXVECTOR3(size, size, -size) + offset)));
}

void Octree::GenerateChildren(PartitionPtr& parent)
{
    assert(parent->children.empty());

    const float size = parent->GetSize() * 0.5f;
    const D3DXVECTOR3 offset((parent->minBounds + parent->maxBounds) * 0.5f);

    parent->children.push_back(PartitionPtr(new Partition(size,
        D3DXVECTOR3(-size, size, -size) + offset, parent)));

    parent->children.push_back(PartitionPtr(new Partition(size, 
        D3DXVECTOR3(0, size, -size) + offset, parent)));
    
    parent->children.push_back(PartitionPtr(new Partition(size, 
        D3DXVECTOR3(-size, 0, -size) + offset, parent)));
    
    parent->children.push_back(PartitionPtr(new Partition(size,
        D3DXVECTOR3(0, 0, -size) + offset, parent)));
    
    parent->children.push_back(PartitionPtr(new Partition(size, 
        D3DXVECTOR3(-size, size, 0) + offset, parent)));
    
    parent->children.push_back(PartitionPtr(new Partition(size, 
        D3DXVECTOR3(0, size, 0) + offset, parent)));
    
    parent->children.push_back(PartitionPtr(new Partition(size, 
        D3DXVECTOR3(-size, 0, 0) + offset, parent)));
    
    parent->children.push_back(PartitionPtr(new Partition(size, 
        D3DXVECTOR3(0, 0, 0) + offset, parent)));
}

void Octree::AddObject(CollisionMeshPtr object)
{
    std::find_if(m_octree.begin(), m_octree.end(), 
        std::bind(&Octree::AddToPartition, this, object, std::placeholders::_1));
}

bool Octree::AddToPartition(const CollisionMeshPtr& object, PartitionPtr& partition)
{
    if(IsInsidePartition(object, partition))
    {
        if(!partition->children.empty())
        {
            std::find_if(partition->children.begin(), partition->children.end(), 
                std::bind(&Octree::AddToPartition, this, object, std::placeholders::_1));
        }
        else if(partition->nodes.size() >= PARTITIONS && partition->level <= MAX_LEVELS)
        {
            GenerateChildren(partition);
            for(auto& node : partition->nodes)
            {
                std::find_if(partition->children.begin(), partition->children.end(), 
                    std::bind(&Octree::AddToPartition, this, node, std::placeholders::_1));
            }
            partition->nodes.clear();

            std::find_if(partition->children.begin(), partition->children.end(), 
                std::bind(&Octree::AddToPartition, this, object, std::placeholders::_1));
        }
        else
        {
            partition->nodes.push_back(object);


        }
        return true;
    }
    return false;
}

bool Octree::IsPointInsidePartition(const D3DXVECTOR3& point, const PartitionPtr& partition)
{
    return point.x > partition->minBounds.x && 
           point.x < partition->maxBounds.x &&
           point.y < partition->minBounds.y && 
           point.y > partition->maxBounds.y &&
           point.z > partition->minBounds.z && 
           point.z < partition->maxBounds.z;
}

bool Octree::IsInsidePartition(const CollisionMeshPtr& object, const PartitionPtr& partition)
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