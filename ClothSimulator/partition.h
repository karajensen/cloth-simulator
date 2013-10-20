////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - partition.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include <deque>

class CollisionMesh;

/**
* Partition for holding collision mesh nodes
*/
class Partition
{
public:

    Partition();
    Partition(float size, const D3DXVECTOR3& minBounds, const Partition* parent);
    ~Partition();

    int GetLevel() const;
    float GetSize() const;
    const std::string& GetID() const;
    const std::deque<std::unique_ptr<Partition>>& GetChildren() const;
    std::deque<CollisionMesh*>& GetNodes();
    const D3DXVECTOR3& GetMinBounds() const;
    const D3DXVECTOR3& GetMaxBounds() const;
    bool HasNodes() const;

    void AddChild(float size, const D3DXVECTOR3& minBounds, const Partition* parent);
    void ModifyChildren(std::function<void(std::unique_ptr<Partition>&)> fn);
    void AddNode(CollisionMesh* node);
    void RemoveNode(CollisionMesh* node);

private:

    int m_level;
    std::string m_id;
    D3DXVECTOR3 m_minBounds;
    D3DXVECTOR3 m_maxBounds;
    const Partition* m_parent;
    std::deque<std::unique_ptr<Partition>> m_children;
    std::deque<CollisionMesh*> m_nodes;
};