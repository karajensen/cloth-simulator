
#include "cloth.h"
#include "input.h"
#include "collision.h"
#include "particle.h"
#include "spring.h"
#include <functional>
#include <algorithm>

namespace 
{
    /**
    * Viable colors for the particles
    */
    enum ParticleColors
    {
        NORMAL,
        PINNED,
        SELECTED,
        MAX_COLORS
    };
}

Cloth::Cloth(LPDIRECT3DDEVICE9 d3ddev, const std::string& texture, std::shared_ptr<Shader> shader, int dimensions, float scale) :
    m_selectedRow(1),
    m_timestep(0.5f),
    m_timestepSquared(m_timestep*m_timestep),
    m_damping(0.9f),
    m_springCount(0),
    m_springIterations(4),
    m_vertexLength(dimensions),
    m_vertexWidth(dimensions),
    m_vertexCount(dimensions*dimensions),
    m_simulation(false),
    m_manipulate(false),
    m_selfCollide(false),
    m_drawVisualParticles(true),
    m_drawColParticles(false),
    m_downwardPull(0,-4.5f,0),
    m_diagnosticParticle(0),
    m_diagnosticSelect(false)
{
    int numOfFaces = ((m_vertexWidth-1)*(m_vertexLength-1))*2;
    m_shader = shader;
    
    //Load texture
    if(FAILED(D3DXCreateTextureFromFile(d3ddev, texture.c_str(), &m_texture)))
    {
        m_texture = nullptr;
        Diagnostic::ShowMessage("Cannot create texture " + texture);
    }

    //Create colors
    m_colors.resize(MAX_COLORS);
    std::generate(m_colors.begin(), m_colors.end(), [&](){ return D3DXVECTOR3(0.0, 0.0, 0.0); });
    m_colors[NORMAL].z = 1.0f; ///< Blue for no selection/pinning
    m_colors[PINNED].x = 1.0f; ///< Red for pinned
    m_colors[SELECTED].y = 1.0f; ///< Green for selection

    //create verticies
    const int minW = -m_vertexWidth/2;
    const int maxW = minW + m_vertexWidth;
    const int minL = -m_vertexLength/2;
    const int maxL = minL + m_vertexLength;
    const float startingheight = 8.0f;
    float UVu = 0;
    float UVv = 0;
    for(int x = minW; x < maxW; ++x)
    {
        for(int z = minL; z < maxL; ++z)
        {
            Vertex vert;
            vert.position.x = x*scale;
            vert.position.z = z*scale;
            vert.position.y = startingheight;
            vert.uvs.x = UVu;
            vert.uvs.y = UVv;
            m_vertexData.push_back(vert);
            UVu += 0.5;
        }
        UVu = 0;
        UVv += 0.5;
    }

    //create indices
    m_indexData.resize(numOfFaces*3);
    m_indexData.assign(m_indexData.size(), 0);

    int k = 0;
    for(int i = 0; i < m_vertexWidth-1; ++i)
    {
        for(int j = 0; j < m_vertexLength-1; ++j)
        {
            m_indexData[k]   = i*m_vertexLength+j;
            m_indexData[k+1] = i*m_vertexLength+j+1;
            m_indexData[k+2] = (i+1)*m_vertexLength+j;
            m_indexData[k+3] = (i+1)*m_vertexLength+j;
            m_indexData[k+4] = i*m_vertexLength+j+1;
            m_indexData[k+5] = (i+1)*m_vertexLength+j+1;
            k += 6; // next quad
        }
    }

    //create particles
    float collisionSize = scale/2; 
    for(unsigned int i = 0; i < static_cast<unsigned int>(m_vertexCount); ++i)
    {
        m_particles.push_back(ParticlePtr(new Particle(d3ddev,collisionSize,m_vertexData[i].position, i)));
    }

    //Connect neighbouring m_particles with m_springs
    for(int x = 0; x < m_vertexWidth; ++x)
    {
        for(int y = 0; y < m_vertexLength; ++y)
        {
            if (x<m_vertexWidth-1)
            { 
                m_springs.push_back(SpringPtr(new Spring(*GetParticle(x,y),*GetParticle(x+1,y))));
            }
            if (y<m_vertexLength-1) 
            { 
                m_springs.push_back(SpringPtr(new Spring(*GetParticle(x,y),*GetParticle(x,y+1))));
            }
            if (x<m_vertexWidth-1 && y<m_vertexLength-1) 
            { 
                m_springs.push_back(SpringPtr(new Spring(*GetParticle(x,y),*GetParticle(x+1,y+1))));
            }
            if (x<m_vertexWidth-1 && y<m_vertexLength-1) 
            { 
                m_springs.push_back(SpringPtr(new Spring(*GetParticle(x+1,y),*GetParticle(x,y+1))));
            }
        }
    }

    //Connect bending m_springs
    for(int x = 0; x < m_vertexWidth; ++x)
    {
        for(int y = 0; y < m_vertexLength; ++y)
        {
            if (x<m_vertexWidth-2) 
            { 
                m_springs.push_back(SpringPtr(new Spring(*GetParticle(x,y),*GetParticle(x+2,y))));
            }
            if (y<m_vertexLength-2)
            { 
                m_springs.push_back(SpringPtr(new Spring(*GetParticle(x,y),*GetParticle(x,y+2))));
            }
            if (x<m_vertexWidth-2 && y<m_vertexLength-2) 
            { 
                m_springs.push_back(SpringPtr(new Spring(*GetParticle(x,y),*GetParticle(x+2,y+2))));
            }
            if (x<m_vertexWidth-2 && y<m_vertexLength-2) 
            { 
                m_springs.push_back(SpringPtr(new Spring(*GetParticle(x+2,y),*GetParticle(x,y+2))));
            }
        }
    }
    m_springCount = int(m_springs.size());

    //create normals
    UpdateNormals();

    //Mesh Vertex Declaration
    D3DVERTEXELEMENT9 VertexDec[] =
    {
        { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },     
        { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    };

    //Create the mesh
    if(!m_mesh)
    {
        if(FAILED(D3DXCreateMesh(numOfFaces, m_vertexData.size(),
                                 D3DXMESH_VB_DYNAMIC | D3DXMESH_IB_MANAGED | D3DXMESH_32BIT,
                                 VertexDec,d3ddev,&m_mesh)))
        {
            m_mesh = nullptr;
            Diagnostic::ShowMessage("Cloth Mesh creation failed");
        }
    }

    //Vertex Buffer
    if(FAILED(m_mesh->LockVertexBuffer(0,&m_vertexBuffer)))
    {
        Diagnostic::ShowMessage("Cloth Vertex buffer lock failed");
    }
    #pragma warning(disable: 4996)
    std::copy(m_vertexData.begin(), m_vertexData.end(),(Vertex*)m_vertexBuffer);
    m_mesh->UnlockVertexBuffer();

    //Index Buffer
    if(FAILED(m_mesh->LockIndexBuffer(0,&m_indexBuffer)))
    {
        Diagnostic::ShowMessage("Cloth Index buffer lock failed");
    }
    #pragma warning(disable: 4996)
    std::copy(m_indexData.begin(), m_indexData.end(), (DWORD*)m_indexBuffer);
    m_mesh->UnlockIndexBuffer();
}

void Cloth::ToggleSimulation()
{
    m_simulation = !m_simulation;
}

void Cloth::ToggleVisualParticles()
{
    m_drawVisualParticles = !m_drawVisualParticles;
}

void Cloth::SetCollisionVisibility(bool draw)
{
    m_drawColParticles = draw;
}

void Cloth::UnpinCloth()
{
    auto unpinParticle = [&](const ParticlePtr& part)
    { 
        part->PinParticle(false); 
        SetParticleColor(part);
    };
    std::for_each(m_particles.begin(), m_particles.end(), unpinParticle);
}

void Cloth::AddForce(const FLOAT3& force)
{
    std::for_each(m_particles.begin(), m_particles.end(), [&](const ParticlePtr& part){ part->AddForce(force); });
}

void Cloth::UpdateState(double deltatime)
{
    if(m_simulation || m_manipulate)
    {
        //Move cloth down slowly
        if(m_simulation)
        {
            AddForce(m_downwardPull*m_timestepSquared*static_cast<float>(deltatime));
        }
    
        //Solve Springs
        for(int j = 0; j < m_springIterations; ++j)
        {
            std::for_each(m_springs.begin(), m_springs.end(), [&](const SpringPtr& spring){ spring->SolveSpring(); });
        }

        //Updating particle positions
        std::for_each(m_particles.begin(), m_particles.end(), 
            [&](const ParticlePtr& part){ part->Update(m_damping, m_timestepSquared); });
    }
}

void Cloth::Reset()
{
    std::for_each(m_particles.begin(), m_particles.end(), 
        [&](const ParticlePtr& part){ part->ResetPosition(); });
    UpdateVertexBuffer();
}

bool Cloth::UpdateVertexBuffer()
{
    //update vertex containers
    for(int i = 0; i < m_vertexCount; ++i)
    {
        m_vertexData[i].position.x = m_particles[i]->GetPosition().x;
        m_vertexData[i].position.y = m_particles[i]->GetPosition().y;
        m_vertexData[i].position.z = m_particles[i]->GetPosition().z;
    }

    //Smooth over the normals
    UpdateNormals();

    //Lock the vertex buffer
    if(FAILED(m_mesh->LockVertexBuffer(0,&m_vertexBuffer)))
    {
        Diagnostic::ShowMessage("Vertex buffer lock failed");
        return false;
    }

    //Copy over values 
    #pragma warning(disable: 4996)
    std::copy(m_vertexData.begin(), m_vertexData.end(),(Vertex*)m_vertexBuffer);

    //unlock vertex buffer
    m_mesh->UnlockVertexBuffer();
    return true;
}

Cloth::ParticlePtr& Cloth::GetParticle(int row, int col)
{
    return m_particles[col*m_vertexWidth + row];
}

Mesh::Vertex& Cloth::GetVertex(int row, int col)
{
    return m_vertexData[col*m_vertexWidth + row];
}

void Cloth::UpdateNormals()
{
    //reset normals
    for(int i = 0; i < m_vertexCount; ++i)
    {
        m_vertexData[i].normal.x = 0.0;
        m_vertexData[i].normal.y = 0.0;
        m_vertexData[i].normal.z = 0.0;
    }

    //create smooth per particle normals
    D3DXVECTOR3 normal;
    for(int x = 0; x < (m_vertexWidth-1); ++x)
    {
        for(int y = 0; y < (m_vertexLength-1); ++y)
        {
            normal = CalculateTriNormal(GetParticle(x+1,y),GetParticle(x,y),GetParticle(x,y+1));
            GetVertex(x+1,y).normal += normal;
            GetVertex(x,y).normal = GetVertex(x,y).normal + normal;
            GetVertex(x,y+1).normal = GetVertex(x,y+1).normal + normal;

            normal = CalculateTriNormal(GetParticle(x+1,y+1),GetParticle(x+1,y),GetParticle(x,y+1));
            GetVertex(x+1,y+1).normal = GetVertex(x+1,y+1).normal + normal;
            GetVertex(x+1,y).normal = GetVertex(x+1,y).normal + normal;
            GetVertex(x,y+1).normal = GetVertex(x,y+1).normal + normal;
        }
    }
}

void Cloth::DrawCollision(const Transform& projection, const Transform& view)
{
    if(Diagnostic::AllowDiagnostics())
    {
        const float radius = 0.4f;
        const auto& position = m_particles[m_diagnosticParticle]->GetPosition();
        const auto& vertex = m_vertexData[m_diagnosticParticle].position;

        Diagnostic::Get().UpdateText("Particle", Diagnostic::YELLOW, StringCast(position.x) 
            + ", " + StringCast(position.y) + ", " + StringCast(position.z));

        Diagnostic::Get().UpdateText("Vertex", Diagnostic::YELLOW, StringCast(vertex.x) 
            + ", " + StringCast(vertex.y) + ", " + StringCast(vertex.z));

        Diagnostic::Get().UpdateSphere("Particle", Diagnostic::YELLOW, 
            D3DXVECTOR3(position.x, position.y, position.z), radius);
    }

    if(m_drawColParticles)
    {
        std::for_each(m_particles.begin(), m_particles.end(), [&](const ParticlePtr& part){ part->DrawCollisionMesh(projection, view); });
    }

    if(m_drawVisualParticles)
    {
        std::for_each(m_particles.begin(), m_particles.end(), [&](const ParticlePtr& part){ part->DrawVisualMesh(projection, view); });
    }
}

D3DXVECTOR3 Cloth::CalculateTriNormal(const ParticlePtr& p1, const ParticlePtr& p2, const ParticlePtr& p3)
{
    FLOAT3 normal((p2->GetPosition()-p1->GetPosition()).Cross(p3->GetPosition()-p1->GetPosition()));
    normal.Normalize();
    return D3DXVECTOR3(normal.x,normal.y,normal.z);
}

void Cloth::SolveCollision(const Collision* object)
{
    switch(object->GetShape())
    {
        case Collision::SPHERE:
        {
            const CollisionSphere* sphere = static_cast<const CollisionSphere*>(object);
            FLOAT3 sphereCenter(sphere->GetPosition().x, sphere->GetPosition().y, sphere->GetPosition().z);
            for(int i = 0; i < m_vertexCount; ++i)
            {
                FLOAT3 centerToParticle(m_particles[i]->GetPosition() - sphereCenter);
                float centerToParticle_l = centerToParticle.Length();
                if (centerToParticle_l < sphere->GetRadius())
                {
                    m_particles[i]->MovePosition(centerToParticle.GetNormalized()*(sphere->GetRadius()-centerToParticle_l)); 
                }
            }
            break;
        }
        case Collision::CUBE:
        {
            const CollisionCube* cube = static_cast<const CollisionCube*>(object);
            for(int i = 0; i < m_vertexCount; ++i)
            {
                FLOAT3 pos = m_particles[i]->GetPosition();
                if(pos.y <= cube->GetMaxBounds().y)
                {
                    float distance = cube->GetMaxBounds().y-pos.y;
                    distance *= (distance < 0) ? -1.0f : 1.0f;
                    m_particles[i]->MovePosition(FLOAT3(0,distance,0));
                }
            }
            break;
        }
    }
}

//bool Cloth::SelfCollisionTest()
//{
//        if(m_selfCollide && m_simulation)
//        {
//            for(int j = 0; j < m_vertexCount; ++j)
//            {
//                for(int i = (j+1); i < m_vertexCount; ++i) //don't test <j against i
//                {
//                    FLOAT3 centerTocenter = m_particles[i]->position - m_particles[j].position; //i <-- j
//                    float centerTocenter_l = centerTocenter.Length();
//                    float rad_l = m_particles[i].Radius + m_particles[j].Radius;
//                    if (centerTocenter_l < rad_l)
//                    {
//                        //don't wanna move j...only move i
//                        float difference = (rad_l-centerTocenter_l);
//                        if(difference < 0)
//                            difference*=-1;
//                        m_particles[i]->MovePosition(centerTocenter.Normalize()*difference);       
//                    }
//                }
//                //check for emergency stop
//                if(KEY_DOWN(VK_BACK))
//                {
//                    m_simulation = false;
//                    return false;
//                }
//            }
//        }
//    return true;
//}

void Cloth::MousePickingTest(Picking& input)
{
    if(m_draw && m_drawVisualParticles)
    {
        int indexChosen = NO_INDEX;
    
        Transform world;
        for(int i = 0; i < m_vertexCount; ++i)
        {
            //fill in world matrix for mesh
            world.Matrix._41 = m_particles[i]->GetCollision()->GetPosition().x;
            world.Matrix._42 = m_particles[i]->GetCollision()->GetPosition().y;
            world.Matrix._43 = m_particles[i]->GetCollision()->GetPosition().z;
    
            D3DXMATRIX worldInverse;
            D3DXMatrixInverse(&worldInverse, NULL, &world.Matrix);

            D3DXVECTOR3 rayObjOrigin;
            D3DXVECTOR3 rayObjDirection;
                
            D3DXVec3TransformCoord(&rayObjOrigin, &input.GetRayOrigin(), &worldInverse);
            D3DXVec3TransformNormal(&rayObjDirection, &input.GetRayDirection(), &worldInverse);
            D3DXVec3Normalize(&rayObjDirection, &rayObjDirection);
    
            BOOL hasHit; 
            float distanceToCollision;
            if(FAILED(D3DXIntersect(m_particles[i]->GetCollision()->GetMesh(), &rayObjOrigin, 
                &rayObjDirection, &hasHit, NULL, NULL, NULL, &distanceToCollision, NULL, NULL)))
            {
                hasHit = false; //Call failed for any reason continue to next mesh.
            }
    
            if(hasHit)
            {
                //if first mesh tested or if better than a previous test then save
                if(input.GetMesh() == nullptr || distanceToCollision < input.GetDistanceToMesh())
                {
                    input.SetPickedMesh(this, distanceToCollision);
                    indexChosen = i;
                }
            }
        }
    
        //Update the mesh pick function with selected index
        if(indexChosen != NO_INDEX)
        {
            auto selectFn = m_diagnosticSelect ? &Cloth::SelectParticleForDiagnostics : &Cloth::SelectParticle;
            SetMeshPickFunction(std::bind(selectFn, this, indexChosen));
        }
    }
}

void Cloth::SetParticleColor(const Cloth::ParticlePtr& particle)
{
    ParticleColors color = (particle->IsPinned() ? PINNED : 
        (particle->IsSelected() && m_manipulate ? SELECTED : NORMAL));
    particle->SetColor(m_colors[color]);
}

void Cloth::SelectParticleForDiagnostics(int index)
{
    m_diagnosticParticle = index;
}

void Cloth::SelectParticle(int index)
{
    m_particles[index]->PinParticle(!m_particles[index]->IsPinned());
    SetParticleColor(m_particles[index]);
}

void Cloth::MovePinnedRow(float right, float up, float forward)
{
    FLOAT3 direction(right,up,forward);
    auto addForce = [&](const ParticlePtr& part){ if(part->IsSelected()){ part->AddForce(direction); } };
    std::for_each(m_particles.begin(), m_particles.end(), addForce);
}

void Cloth::ChangeRow(int row)
{
    if(m_selectedRow != row)
    {
        ChangeRow(m_selectedRow, false);
        m_selectedRow = row;
        ChangeRow(m_selectedRow, true);
    }
}

void Cloth::SetTimeStep(float timestep)
{
    m_timestep = timestep;
    m_timestepSquared = m_timestep*m_timestep;
}

void Cloth::SetManipulate(bool manip)
{
    SetSimulation(false);
    SetSelfCollide(false);
    m_manipulate = manip;
    ChangeRow(m_selectedRow, m_manipulate);
}

void Cloth::ChangeRow(int row, bool select)
{
    int index = 0;
    int counter = 0;
    std::function<void(void)> getIndexFn = nullptr;

    switch(row)
    {
    case 1:
        getIndexFn = [&](){ index = counter; };
        break;
    case 2:
        getIndexFn = [&](){ index = (m_vertexWidth*counter) + (m_vertexWidth-1); };
        break;
    case 3:
        getIndexFn = [&](){ index = (m_vertexWidth*(m_vertexWidth-1))+counter; };
        break;
    case 4:
        getIndexFn = [&](){ index = m_vertexWidth*counter; };
        break;
    }

    for(; counter < m_vertexWidth; ++counter)
    {
        getIndexFn();
        m_particles[index]->SelectParticle(select);
        SetParticleColor(m_particles[index]);
    }
}
