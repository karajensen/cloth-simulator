////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - octree.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "octree.h"
#include "collision.h"

namespace
{
    const int PARTITIONS = 8; ///< Number of partitions allowed within a parent
    const float PARITION_SIZE = 30.0f; ///< Initial dimensions of the root
    const float MAX_CHILDREN = 3; ///< Number of children allowed from the root
    const int CORNERS = 8; ///< Number of corners in a cube
}

Octree::Octree(EnginePtr engine) :
    m_engine(engine)
{
}

void Octree::RenderDiagnostics()
{
    if(m_engine->diagnostic()->AllowDiagnostics(Diagnostic::OCTREE))
    {
        std::for_each(m_octree.begin(), m_octree.end(), [&](const Partition& partition)
        {
            const float size = PARITION_SIZE/2.0f;
            std::array<D3DXVECTOR3, CORNERS> corners =
            {
                // top four corners
                partition.minBounds,
                partition.minBounds + D3DXVECTOR3(size, 0, 0),
                partition.minBounds + D3DXVECTOR3(size, -size, 0),
                partition.minBounds + D3DXVECTOR3(0, -size, 0),
            
                // bottom four corners
                partition.minBounds + D3DXVECTOR3(0, 0, size),
                partition.minBounds + D3DXVECTOR3(size, 0, size),
                partition.minBounds + D3DXVECTOR3(size, -size, size),
                partition.minBounds + D3DXVECTOR3(0, -size, size)
            };
            
            for(unsigned int i = 0; i < CORNERS/2; ++i)
            {
                m_engine->diagnostic()->UpdateLine(Diagnostic::OCTREE,
                    partition.id + StringCast(i) + "line1", Diagnostic::BLUE, 
                    corners[i], corners[i+1 >= CORNERS/2 ? 0 : i+1]);
            
                m_engine->diagnostic()->UpdateLine(Diagnostic::OCTREE,
                    partition.id + StringCast(i) + "line2", Diagnostic::BLUE, 
                    corners[i+4], corners[i+5 >= CORNERS ? 4 : i+5]);
            
                m_engine->diagnostic()->UpdateLine(Diagnostic::OCTREE,
                    partition.id + StringCast(i) + "line3", Diagnostic::BLUE, 
                    corners[i], corners[i+4]);
            }
        });
    }
}

void Octree::BuildInitialTree()
{
    D3DXVECTOR3 origin = m_world.Position();
    const float size = PARITION_SIZE/2.0f;

    Partition one;
    one.id = "one";
    one.minBounds = D3DXVECTOR3(-size, size, -size) + origin;
    one.maxBounds = D3DXVECTOR3(0, 0, 0) + origin;
    m_octree.push_back(one);

    Partition two;
    two.id = "two";
    two.minBounds = D3DXVECTOR3(0, size, -size) + origin;
    two.maxBounds = D3DXVECTOR3(size, 0, 0) + origin;
    m_octree.push_back(two);

    Partition three;
    three.id = "three";
    three.minBounds = D3DXVECTOR3(-size, 0, -size) + origin;
    three.maxBounds = D3DXVECTOR3(0, -size, 0) + origin;
    m_octree.push_back(three);

    Partition four;
    four.id = "four";
    four.minBounds = D3DXVECTOR3(0, 0, -size) + origin;
    four.maxBounds = D3DXVECTOR3(size, -size, 0) + origin;
    m_octree.push_back(four);

    Partition five;
    five.id = "five";
    five.minBounds = D3DXVECTOR3(-size, size, 0) + origin;
    five.maxBounds = D3DXVECTOR3(0, 0, size) + origin;
    m_octree.push_back(five);

    Partition six;
    six.id = "six";
    six.minBounds = D3DXVECTOR3(0, size, 0) + origin;
    six.maxBounds = D3DXVECTOR3(size, 0, size) + origin;
    m_octree.push_back(six);

    Partition seven;
    seven.id = "seven";
    seven.minBounds = D3DXVECTOR3(-size, 0, 0) + origin;
    seven.maxBounds = D3DXVECTOR3(0, -size, -size) + origin;
    m_octree.push_back(seven);

    Partition eight;
    eight.id = "eight";
    eight.minBounds = D3DXVECTOR3(0, 0, 0);
    eight.maxBounds = D3DXVECTOR3(size, -size, size);
    m_octree.push_back(eight);
}

void Octree::UpdateTransform(const Transform& transform)
{
    m_world.SetScale(transform.GetScale());
    m_world.SetPosition(transform.Position());
}

void Octree::AddObject(CollisionPtr object, bool dynamic)
{
    

}