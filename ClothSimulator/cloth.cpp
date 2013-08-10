////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com
////////////////////////////////////////////////////////////////////////////////////////

#include "cloth.h"
#include "input.h"
#include "particle.h"
#include "collision.h"
#include "spring.h"
#include "shader.h"
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

    /**
    * Initial values for the cloth
    */
    const int ROWS = 20;         
    const int ITERATIONS = 6;    
    const float TIMESTEP = 0.5f; 
    const float DAMPING = 0.9f;  
    const float SPACING = 0.75f; 
}

Cloth::Cloth(LPDIRECT3DDEVICE9 d3ddev, const RenderCallbacks& callbacks) :
    Mesh(callbacks),
    m_selectedRow(1),
    m_timestep(TIMESTEP),
    m_timestepSquared(TIMESTEP*TIMESTEP),
    m_damping(DAMPING),
    m_springCount(0),
    m_springIterations(ITERATIONS),
    m_vertexLength(0),
    m_vertexWidth(0),
    m_vertexCount(0),
    m_simulation(false),
    m_drawVisualParticles(false),
    m_drawColParticles(false),
    m_spacing(0.0f),
    m_handleMode(false),
    m_gravity(0,-9.8f,0),
    m_diagnosticSelect(false),
    m_diagnosticParticle(0),
    m_d3ddev(d3ddev),
    m_vertexBuffer(nullptr),
    m_indexBuffer(nullptr),
    m_callbacks(callbacks)
{
    m_data->shader = callbacks.getShader(ShaderManager::CLOTH_SHADER);;
    m_template.reset(new Collision(*this, callbacks.getShader(ShaderManager::BOUNDS_SHADER)));
    m_template->LoadSphere(d3ddev, 1.0f, 8);

    if(FAILED(D3DXCreateTextureFromFile(d3ddev, ".\\Resources\\Textures\\square.png", &m_data->texture)))
    {
        m_data->texture = nullptr;
        Diagnostic::ShowMessage("Cannot create cloth texture");
    }

    m_colors.resize(MAX_COLORS);
    std::generate(m_colors.begin(), m_colors.end(), [&](){ return D3DXVECTOR3(0.0, 0.0, 0.0); });
    m_colors[NORMAL].z = 1.0f; ///< Blue for no selection/pinning
    m_colors[PINNED].x = 1.0f; ///< Red for pinned
    m_colors[SELECTED].y = 1.0f; ///< Green for selection

    CreateCloth(ROWS, SPACING);
}

void Cloth::CreateCloth(int rows, float spacing)
{
    m_spacing = spacing;
    m_vertexLength = rows;
    m_vertexWidth = rows;
    m_vertexCount = rows*rows;

    //create verticies
    m_vertexData.resize(m_vertexCount);
    const int minW = -m_vertexWidth/2;
    const int maxW = minW + m_vertexWidth;
    const int minL = -m_vertexLength/2;
    const int maxL = minL + m_vertexLength;
    const D3DXVECTOR3 startingPos(1.0f, 8.0f, 0.0f);
    float UVu = 0;
    float UVv = 0;
    int index = 0;
    for(int x = minW; x < maxW; ++x)
    {
        for(int z = minL; z < maxL; ++z, ++index)
        {
            m_vertexData[index].position = startingPos;
            m_vertexData[index].position.x += x*m_spacing;
            m_vertexData[index].position.z += z*m_spacing;
            m_vertexData[index].uvs.x = UVu;
            m_vertexData[index].uvs.y = UVv;
            UVu += 0.5;
        }
        UVu = 0;
        UVv += 0.5;
    }

    //create indices
    int numOfFaces = ((m_vertexWidth-1)*(m_vertexLength-1))*2;
    m_indexData.resize(numOfFaces*3);

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
    m_particles.resize(m_vertexCount);
    m_template->GetData().localWorld.SetScale(m_spacing/2.0f);
    for(unsigned int i = 0; i < m_particles.size(); ++i)
    {
        if(!m_particles[i].get())
        {
            m_particles[i].reset(new Particle(m_d3ddev, m_callbacks));
        }
        m_particles[i]->Initialise(m_vertexData[i].position, i,
            m_template->GetGeometry(), m_template->GetData());
    }

    /* Connect neighbouring particles with springs
    ------ x
    |  |  |    Stretch/Compression: Horizontal/Vertical springs
    -------    Shear: Cross springs
    |  |  |    Bending: Link every second horizontal/vertical vertex
    ------              
    y */

    auto createSpring = [&](int i) -> int
    {
        if(!m_springs[i+1].get())
        {
            m_springs[i+1].reset(new Spring());
        }
        return i+1;
    };

    m_springCount = ((m_vertexLength-1)*(((m_vertexWidth-2)*2)+2)) 
        + (m_vertexLength*(m_vertexWidth-1)) 
        + (m_vertexLength*(m_vertexWidth-2))
        + ((m_vertexLength-2)*m_vertexWidth)
        + ((m_vertexLength-1)*m_vertexWidth);

    m_springs.resize(m_springCount);
    index = NO_INDEX;
    for(int x = 0; x < m_vertexWidth; ++x)
    {
        for(int y = 0; y < m_vertexLength; ++y)
        {
            //Last y doesn't have cross springs
            if(y < m_vertexLength-1)
            {
                if(x < m_vertexWidth-1) //Don't create right cross if last x
                {
                    index = createSpring(index);
                    m_springs[index]->Initialise(*GetParticle(x,y),
                        *GetParticle(x+1,y+1), index, Spring::SHEAR);
                }

                if(x > 0) //Don't create left cross if first x
                {
                    index = createSpring(index);
                    m_springs[index]->Initialise(*GetParticle(x,y),
                        *GetParticle(x-1,y+1), index, Spring::SHEAR);
                }
            }

            //Last 2 xs doesn't have bending horizontal springs
            if(x < m_vertexWidth-2)
            {
                index = createSpring(index);
                m_springs[index]->Initialise(*GetParticle(x,y),
                    *GetParticle(x+2,y), index, Spring::BEND);
            }

            //Last x doesn't have horizontal springs
            if(x < m_vertexWidth-1)
            {
                index = createSpring(index);
                m_springs[index]->Initialise(*GetParticle(x,y),
                    *GetParticle(x+1,y), index, Spring::STRETCH);
            }

            //Last 2ys doesn't have bending vertical springs
            if(y < m_vertexLength-2)
            {
                index = createSpring(index);
                m_springs[index]->Initialise(*GetParticle(x,y),
                    *GetParticle(x,y+2), index, Spring::BEND);
            }
            
            //Last y doesn't have vertical springs
            if(y < m_vertexLength-1)
            {
                index = createSpring(index);
                m_springs[index]->Initialise(*GetParticle(x,y),
                    *GetParticle(x,y+1), index, Spring::STRETCH);
            }
        }
    }

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
    if(m_data->mesh)
    {
        m_data->mesh->Release();
        m_data->mesh = nullptr;
    }

    if(FAILED(D3DXCreateMesh(numOfFaces, m_vertexData.size(),
                             D3DXMESH_VB_DYNAMIC | D3DXMESH_IB_MANAGED | D3DXMESH_32BIT,
                             VertexDec, m_d3ddev, &m_data->mesh)))
    {
        m_data->mesh = nullptr;
        Diagnostic::ShowMessage("Cloth Mesh creation failed");
    }

    //Vertex Buffer
    m_vertexBuffer = nullptr;
    if(FAILED(m_data->mesh->LockVertexBuffer(0,&m_vertexBuffer)))
    {
        Diagnostic::ShowMessage("Cloth Vertex buffer lock failed");
    }
    #pragma warning(disable: 4996)
    std::copy(m_vertexData.begin(), m_vertexData.end(),(Vertex*)m_vertexBuffer);
    m_data->mesh->UnlockVertexBuffer();

    //Index Buffer
    m_indexBuffer = nullptr;
    if(FAILED(m_data->mesh->LockIndexBuffer(0,&m_indexBuffer)))
    {
        Diagnostic::ShowMessage("Cloth Index buffer lock failed");
    }
    #pragma warning(disable: 4996)
    std::copy(m_indexData.begin(), m_indexData.end(), (DWORD*)m_indexBuffer);
    m_data->mesh->UnlockIndexBuffer();
}

void Cloth::SetHandleMode(bool set)
{
    m_handleMode = set;
    ChangeRow(m_selectedRow, m_handleMode);
}

void Cloth::SetVertexVisibility(bool draw)
{
    m_drawVisualParticles = draw;
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

void Cloth::AddForce(const D3DXVECTOR3& force)
{
    std::for_each(m_particles.begin(), m_particles.end(), 
        [&](const ParticlePtr& part){ part->AddForce(force); });
}

void Cloth::UpdateState(double deltatime)
{
    if(m_simulation || m_handleMode)
    {
        //Move cloth down slowly
        if(m_simulation)
        {
            AddForce(m_gravity*m_timestepSquared*static_cast<float>(deltatime));
        }
    
        //Solve Springs
        for(int j = 0; j < m_springIterations; ++j)
        {
            std::for_each(m_springs.begin(), m_springs.end(), 
                [&](const SpringPtr& spring){ spring->SolveSpring(); });
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
    if(FAILED(m_data->mesh->LockVertexBuffer(0,&m_vertexBuffer)))
    {
        Diagnostic::ShowMessage("Vertex buffer lock failed");
        return false;
    }

    //Copy over values 
    #pragma warning(disable: 4996)
    std::copy(m_vertexData.begin(), m_vertexData.end(),(Vertex*)m_vertexBuffer);

    //unlock vertex buffer
    m_data->mesh->UnlockVertexBuffer();
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

        Diagnostic::UpdateSphere("Particle", 
            Diagnostic::YELLOW, position, radius);

        std::for_each(m_springs.begin(), m_springs.end(), 
            [&](const SpringPtr& spring){ spring->UpdateDiagnostic(); });
    }

    if(Diagnostic::AllowText())
    {
        const auto& vertex = m_vertexData[m_diagnosticParticle].position;
        const auto& position = m_particles[m_diagnosticParticle]->GetPosition();
        const auto& collision = m_particles[m_diagnosticParticle]->GetCollision()->GetPosition();

        Diagnostic::UpdateText("Particle", Diagnostic::YELLOW, StringCast(position.x) 
            + ", " + StringCast(position.y) + ", " + StringCast(position.z));

        Diagnostic::UpdateText("Collision", Diagnostic::YELLOW, StringCast(collision.x) 
            + ", " + StringCast(collision.y) + ", " + StringCast(collision.z));

        Diagnostic::UpdateText("Vertex", Diagnostic::YELLOW, StringCast(vertex.x) 
            + ", " + StringCast(vertex.y) + ", " + StringCast(vertex.z));
    }

    if(m_drawColParticles)
    {
        std::for_each(m_particles.begin(), m_particles.end(), 
            [&](const ParticlePtr& part){ part->DrawCollisionMesh(projection, view); });
    }

    if(m_drawVisualParticles)
    {
        std::for_each(m_particles.begin(), m_particles.end(), 
            [&](const ParticlePtr& part){ part->DrawVisualMesh(projection, view); });
    }
}

D3DXVECTOR3 Cloth::CalculateTriNormal(const ParticlePtr& p1, const ParticlePtr& p2, const ParticlePtr& p3)
{
    D3DXVECTOR3 normal;
    D3DXVECTOR3 p2p1(p2->GetPosition()-p1->GetPosition());
    D3DXVECTOR3 p3p1(p3->GetPosition()-p1->GetPosition());
    D3DXVec3Cross(&normal, &p2p1, &p3p1);
    D3DXVec3Normalize(&normal, &normal);
    return normal;
}

void Cloth::MousePickingTest(Picking& input)
{
    if(m_draw && m_drawVisualParticles)
    {
        int indexChosen = NO_INDEX;
        for(int i = 0; i < m_vertexCount; ++i)
        {
            D3DXMATRIX worldInverse;
            D3DXMatrixInverse(&worldInverse, NULL, &m_particles[i]->GetCollision()->GetTransform().Matrix());

            D3DXVECTOR3 rayObjOrigin;
            D3DXVECTOR3 rayObjDirection;
                
            D3DXVec3TransformCoord(&rayObjOrigin, &input.GetRayOrigin(), &worldInverse);
            D3DXVec3TransformNormal(&rayObjDirection, &input.GetRayDirection(), &worldInverse);
            D3DXVec3Normalize(&rayObjDirection, &rayObjDirection);
    
            BOOL hasHit; 
            if(FAILED(D3DXIntersect(m_particles[i]->GetCollision()->GetMesh(), &rayObjOrigin, 
                &rayObjDirection, &hasHit, NULL, NULL, NULL, NULL, NULL, NULL)))
            {
                hasHit = false; //Call failed for any reason continue to next mesh.
            }
    
            if(hasHit)
            {
                D3DXVECTOR3 cameraToMesh = input.GetRayOrigin()-m_particles[i]->GetPosition();
                float distanceToCollision = D3DXVec3Length(&cameraToMesh);
                if(distanceToCollision < input.GetDistanceToMesh())
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
        (particle->IsSelected() && m_handleMode ? SELECTED : NORMAL));
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
    if(m_handleMode)
    {
        D3DXVECTOR3 direction(right,up,forward);
        auto addForce = [&](const ParticlePtr& part){ if(part->IsSelected()){ part->AddForce(direction); } };
        std::for_each(m_particles.begin(), m_particles.end(), addForce);
    }
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

void Cloth::SetSpacing(double size)
{
    if(size != m_spacing)
    {
        CreateCloth(m_vertexLength, static_cast<float>(size));
    }
}

void Cloth::SetVertexRows(double number)
{
    if(number != m_vertexLength)
    {
        CreateCloth(static_cast<int>(number), m_spacing);
    }
}

void Cloth::SetIterations(double iterations)
{
    m_springIterations = static_cast<int>(iterations);
}

void Cloth::SetTimeStep(double timestep)
{
    m_timestep = static_cast<float>(timestep);
    m_timestepSquared = m_timestep*m_timestep;
}

double Cloth::GetTimeStep() const
{
    return m_timestep;
}

double Cloth::GetIterations() const
{
    return m_springIterations;
}

double Cloth::GetVertexRows() const
{
    return m_vertexLength;
}

double Cloth::GetSpacing() const
{
    return m_spacing;
}

std::vector<Cloth::ParticlePtr>& Cloth::GetParticles()
{
    return m_particles;
}