////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - octree.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "octree.h"
#include "collision.h"

namespace
{
    const int PARTITIONS = 8; ///< Number of partitions allowed within parent
    const float PARITION_SIZE = 40.0f; /// Initial dimensions of the parent
}

Octree::Octree()
{
}

void Octree::BuildInitialTree()
{
    //std::for_each(m_dynamic.begin(), m_dynamic.end(), [](const CollisionPtr&)
    //{
    //});
}

void Octree::AddObject(CollisionPtr object, bool dynamic)
{
    dynamic ? m_dynamic.push_back(object) : m_static.push_back(object);
}