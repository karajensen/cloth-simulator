////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - partition.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"
#include "diagnostic.h"

#include <deque>

class CollisionMesh;

/**
* Partition for holding collision mesh nodes
*/
class Partition
{
public:

    /**
    * Constructor
    */
    Partition();

    /**
    * Constructor
    * @param size The size of the partitions dimensions
    * @param minBounds the minimum point of the corners
    * @param parent The parent of the partition or null if none
    */
    Partition(float size, const D3DXVECTOR3& minBounds, Partition* parent);

    /**
    * Destructor
    */
    ~Partition();

    /**
    * @return the parent of the partition
    */
    Partition* GetParent();

    /**
    * @return the child-parent level of the partition 
    */
    int GetLevel() const;

    /**
    * @return the size dimensions of the partition
    */
    float GetSize() const;

    /**
    * @return the unique id for the partition
    */
    const std::string& GetID() const;

    /**
    * @return the children of the partition
    */
    const std::deque<std::unique_ptr<Partition>>& GetChildren() const;

    /**
    * @return the nodes of the partition
    */
    std::deque<CollisionMesh*>& GetNodes();

    /**
    * @return the minimum global coordinate of the partition
    */
    const D3DXVECTOR3& GetMinBounds() const;

    /**
    * @return the maximum global coordinate of the partition
    */
    const D3DXVECTOR3& GetMaxBounds() const;

    /**
    * @return whether the partition has any nodes or not
    */
    bool HasNodes() const;

    /**
    * Adds a child to the partition
    * @param size The size of the partitions dimensions
    * @param minBounds the minimum point of the corners
    */
    void AddChild(float size, const D3DXVECTOR3& minBounds);

    /**
    * Calls the given function on each child of the partition
    * @param fn The function to use for each child
    */
    void ModifyChildren(std::function<void(std::unique_ptr<Partition>&)> fn);

    /**
    * Adds a node to the partition
    * @param node The collision node to add
    */
    void AddNode(CollisionMesh& node);

    /**
    * Removes a node from the partition
    * @param node The collision node to remove
    */
    void RemoveNode(CollisionMesh& node);

    /**
    * @return the color for the partition
    */
    Diagnostic::Colour GetColor() const;

private:

    int m_level;                ///< Parent-child level for the partition
    std::string m_id;           ///< Unique ID for the partition
    D3DXVECTOR3 m_minBounds;    ///< Minimum point of the partition
    D3DXVECTOR3 m_maxBounds;    ///< Maximum point of the partition
    Partition* m_parent;        ///< Parent of the partition

    std::deque<CollisionMesh*> m_nodes;                 ///< collision mesh nodes
    std::deque<std::unique_ptr<Partition>> m_children;  ///< child partitions
};