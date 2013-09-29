////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - octree.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "octree.h"
#include "collision.h"

namespace
{
    const int ROOT_PARTITIONS = 16; ///< Number of partitions for the root
    const int PARTITIONS = 8; ///< Number of partitions allowed within a parent
    const float PARITION_SIZE = 45.0f; ///< Initial dimensions of the root partitions
    const float GROUND_HEIGHT = -23.0f; ///< Height of the ground plane
    const int CORNERS = 8; ///< Number of corners in a cube
    const int MAX_CHILDREN = 3; ///< Number of children allowed from the root
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
            //RenderPartition(0, partition);

            std::for_each(partition.children.begin(), partition.children.end(), 
                std::bind(&Octree::RenderPartition, this, 1, std::placeholders::_1));
        });
    }
}

void Octree::RenderPartition(int recursionLevel, const Partition& partition)
{
    std::for_each(partition.children.begin(), partition.children.end(), 
        std::bind(&Octree::RenderPartition, this, recursionLevel+1, std::placeholders::_1));

    const float size = partition.size;
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
            
    auto colour = static_cast<Diagnostic::Colour>(min(MAX_CHILDREN, recursionLevel));
    for(unsigned int i = 0; i < CORNERS/2; ++i)
    {
        m_engine->diagnostic()->UpdateLine(Diagnostic::OCTREE,
            partition.id + StringCast(i) + "line1", colour,
            corners[i], corners[i+1 >= 4 ? 0 : i+1]);
            
        m_engine->diagnostic()->UpdateLine(Diagnostic::OCTREE,
            partition.id + StringCast(i) + "line2", colour,
            corners[i+4], corners[i+5 >= CORNERS ? 4 : i+5]);
            
        m_engine->diagnostic()->UpdateLine(Diagnostic::OCTREE,
            partition.id + StringCast(i) + "line3", colour,
            corners[i], corners[i+4]);
    }
}

void Octree::BuildInitialTree()
{
    const float size = PARITION_SIZE;
    const D3DXVECTOR3 offset(-size/2.0f, GROUND_HEIGHT, -size/2.0f);
    const D3DXVECTOR3 boundsOffset(size, -size, size);
         
    Partition one;
    one.id = "one";
    one.size = size;
    one.minBounds = D3DXVECTOR3(-size, size, -size) + offset;
    one.maxBounds = one.minBounds + boundsOffset;
    m_octree.push_back(one);

    Partition two;
    two.id = "two";
    two.size = size;
    two.minBounds = D3DXVECTOR3(0, size, -size) + offset;
    two.maxBounds = two.minBounds + boundsOffset;
    m_octree.push_back(two);

    Partition three;
    three.id = "three";
    three.size = size;
    three.minBounds = D3DXVECTOR3(-size, size, 0) + offset;
    three.maxBounds = three.minBounds + boundsOffset;
    m_octree.push_back(three);
    
    Partition four;
    four.id = "four";
    four.size = size;
    four.minBounds = D3DXVECTOR3(0, size, 0) + offset;
    four.maxBounds = four.minBounds + boundsOffset;
    
    // Testing partitioning
    GenerateChildren(four);
    GenerateChildren(*four.children.begin());
    m_octree.push_back(four);

    Partition five;
    five.id = "five";
    five.size = size;
    five.minBounds = D3DXVECTOR3(-size, size, size) + offset;
    five.maxBounds = five.minBounds + boundsOffset;
    m_octree.push_back(five);

    Partition six;
    six.id = "six";
    six.size = size;
    six.minBounds = D3DXVECTOR3(0, size, size) + offset;
    six.maxBounds = six.minBounds + boundsOffset;
    m_octree.push_back(six);

    Partition seven;
    seven.id = "seven";
    seven.size = size;
    seven.minBounds = D3DXVECTOR3(size, size, size) + offset;
    seven.maxBounds = seven.minBounds + boundsOffset;
    m_octree.push_back(seven);

    Partition eight;
    eight.id = "eight";
    eight.size = size;
    eight.minBounds = D3DXVECTOR3(size, size, 0) + offset;
    eight.maxBounds = eight.minBounds + boundsOffset;
    m_octree.push_back(eight);

    Partition nine;
    nine.size = size;
    nine.id = "nine";
    nine.minBounds = D3DXVECTOR3(size, size, -size) + offset;
    nine.maxBounds = nine.minBounds + boundsOffset;
    m_octree.push_back(nine);
}

void Octree::GenerateChildren(Octree::Partition& parent)
{
    const float size = parent.size * 0.5f;
    const D3DXVECTOR3 offset((parent.minBounds + parent.maxBounds) * 0.5f);
    const D3DXVECTOR3 boundsOffset(size, -size, size);

    Partition one;
    one.id = parent.id + "one";
    one.size = size;
    one.minBounds = D3DXVECTOR3(-size, size, -size) + offset;
    one.maxBounds = one.minBounds + boundsOffset;
    parent.children.push_back(one);

    Partition two;
    two.id = parent.id + "two";
    two.size = size;
    two.minBounds = D3DXVECTOR3(0, size, -size) + offset;
    two.maxBounds = two.minBounds + boundsOffset;
    parent.children.push_back(two);
    
    Partition three;
    three.id = parent.id + "three";
    three.size = size;
    three.minBounds = D3DXVECTOR3(-size, 0, -size) + offset;
    three.maxBounds = three.minBounds + boundsOffset;
    parent.children.push_back(three);
    
    Partition four;
    four.id = parent.id + "four";
    four.size = size;
    four.minBounds = D3DXVECTOR3(0, 0, -size) + offset;
    four.maxBounds = four.minBounds + boundsOffset;
    parent.children.push_back(four);
    
    Partition five;
    five.id = parent.id + "five";
    five.size = size;
    five.minBounds = D3DXVECTOR3(-size, size, 0) + offset;
    five.maxBounds = five.minBounds + boundsOffset;
    parent.children.push_back(five);
    
    Partition six;
    six.id = parent.id + "six";
    six.size = size;
    six.minBounds = D3DXVECTOR3(0, size, 0) + offset;
    six.maxBounds = six.minBounds + boundsOffset;
    parent.children.push_back(six);
    
    Partition seven;
    seven.id = parent.id + "seven";
    seven.size = size;
    seven.minBounds = D3DXVECTOR3(-size, 0, 0) + offset;
    seven.maxBounds = seven.minBounds + boundsOffset;
    parent.children.push_back(seven);
    
    Partition eight;
    eight.id = parent.id + "eight";
    eight.size = size;
    eight.minBounds = D3DXVECTOR3(0, 0, 0) + offset;
    eight.maxBounds = eight.minBounds + boundsOffset;
    parent.children.push_back(eight);
}

void Octree::AddObject(CollisionPtr object, bool dynamic)
{
    

}