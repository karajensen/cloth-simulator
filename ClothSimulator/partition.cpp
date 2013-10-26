////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - octree.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "partition.h"
#include <algorithm>

Partition::Partition() :
    m_parent(nullptr),
    m_minBounds(0, 0, 0),
    m_maxBounds(0, 0, 0),
    m_level(0),
    m_id("0")
{
}

Partition::~Partition()
{
}

Partition::Partition(float size, const D3DXVECTOR3& minBounds, Partition* parent) :
    m_parent(parent),
    m_minBounds(minBounds)
{
    const D3DXVECTOR3 minToMax(size, -size, size);
    m_maxBounds = minBounds + minToMax;
    m_level = parent->m_level + 1;
    m_id = parent->m_id + "|" + StringCast(m_level)
        + "-" + StringCast(parent->GetChildren().size());
}

float Partition::GetSize() const
{
    return fabs(m_maxBounds.x - m_minBounds.x);
}

const std::string& Partition::GetID() const
{
    return m_id;
}

int Partition::GetLevel() const
{
    return m_level;
}

const std::deque<std::unique_ptr<Partition>>& Partition::GetChildren() const
{
    return m_children;
}

std::deque<CollisionMesh*>& Partition::GetNodes()
{
    return m_nodes;
}

const D3DXVECTOR3& Partition::GetMinBounds() const
{
    return m_minBounds;
}

const D3DXVECTOR3& Partition::GetMaxBounds() const
{
    return m_maxBounds;
}

bool Partition::HasNodes() const
{
    return !m_nodes.empty();
}

void Partition::RemoveNode(CollisionMesh* node)
{
    m_nodes.erase(std::remove(m_nodes.begin(), m_nodes.end(), node), m_nodes.end());
}

void Partition::AddChild(float size, const D3DXVECTOR3& minBounds, Partition* parent)
{
    m_children.push_back(std::unique_ptr<Partition>(
        new Partition(size, minBounds, parent)));
}

void Partition::ModifyChildren(std::function<void(std::unique_ptr<Partition>&)> fn)
{
    for(std::unique_ptr<Partition>& child : m_children)
    {
        fn(child);
    }
}

void Partition::AddNode(CollisionMesh* node)
{
    m_nodes.push_back(node);
}

Partition* Partition::GetParent()
{
    return m_parent;
}