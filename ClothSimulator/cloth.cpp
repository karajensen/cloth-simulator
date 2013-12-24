////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - cloth.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "cloth.h"
#include "input.h"
#include "particle.h"
#include "collisionmesh.h"
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

    const int ROWS = 20;                   ///< Initial rows for the cloth 
    const int ITERATIONS = 2;              ///< Initial iterations for the cloth
    const float TIMESTEP = 0.45f;          ///< Initial timestep for the cloth
    const float DAMPING = 0.9f;            ///< Initial damping for the cloth
    const float SPACING = 0.75f;           ///< Initial particle spacing for the cloth
    const int PARTICLE_SUBDIVISIONS = 8;   ///< Subdivisions for cloth particles
    const float SMOOTH_INCREASE = 0.01f;   ///< Increase amount when changing smoothing

    const D3DXVECTOR3 STARTING_POSITION(0.5f, 8.0f, 0.0f); ///< Initial position for the cloth
}

Cloth::Cloth(EnginePtr engine) :
    m_selectedRow(1),
    m_timestep(TIMESTEP),
    m_timestepSquared(TIMESTEP*TIMESTEP),
    m_damping(DAMPING),
    m_springCount(0),
    m_springIterations(ITERATIONS),
    m_particleLength(0),
    m_particleCount(0),
    m_quadVertices(0),
    m_simulation(false),
    m_drawVisualParticles(false),
    m_drawColParticles(false),
    m_spacing(0.0f),
    m_handleMode(false),
    m_subdivideCloth(false),
    m_gravity(0,-9.8f,0),
    m_generalSmoothing(0.85f),
    m_engine(engine),
    m_template(nullptr),
    m_mesh(nullptr),
    m_texture(nullptr),
    m_shader(nullptr)
{
    m_shader = m_engine->getShader(ShaderManager::CLOTH_SHADER);
    m_template.reset(new CollisionMesh(*this, m_engine));
    m_template->LoadSphere(true, 1.0f, PARTICLE_SUBDIVISIONS);

    const std::string path(".\\Resources\\Textures\\square.png");
    if(FAILED(D3DXCreateTextureFromFile(m_engine->device(), path.c_str(), &m_texture)))
    {
        m_texture = nullptr;
        ShowMessageBox("Cannot create cloth texture");
    }

    m_colors.resize(MAX_COLORS);
    m_colors[NORMAL] = engine->diagnostic()->GetColor(Diagnostic::BLUE);
    m_colors[PINNED] = engine->diagnostic()->GetColor(Diagnostic::RED);
    m_colors[SELECTED] = engine->diagnostic()->GetColor(Diagnostic::CYAN);

    CreateCloth(ROWS, SPACING);
}

Cloth::~Cloth()
{
}

void Cloth::CreateCloth(int rows, float spacing)
{
    m_spacing = spacing;
    m_particleLength = rows;
    m_particleCount = rows*rows;

    // Remove any particles from octree no longer needed
    int current = static_cast<int>(m_particles.size());
    int difference = current - m_particleCount;
    if(difference > 0)
    {
        for(int i = current-difference; i < current; ++i)
        {
            m_engine->octree()->RemoveObject(m_particles[i]->GetCollisionMesh());
        }
    }

    // Create the particles
    m_particles.resize(m_particleCount);
    m_template->SetLocalScale(m_spacing/2.0f);
    const int mininum = -m_particleLength/2;
    const int maximum = m_particleLength/2;

    float UVu = 0;
    float UVv = 0;
    int index = 0;

    for(int x = mininum; x < maximum; ++x)
    {
        for(int z = mininum; z < maximum; ++z, ++index)
        {
            bool firstInitialisation = !m_particles[index].get();
            D3DXVECTOR2 uvs(UVu, UVv);
            D3DXVECTOR3 position = STARTING_POSITION;
            position.x += x*m_spacing;
            position.z += z*m_spacing;

            if(firstInitialisation)
            {
                m_particles[index].reset(new Particle(m_engine));
            }

            m_particles[index]->Initialise(position, uvs,
                index, m_template->GetGeometry(), m_template->GetLocalScale());

            if(firstInitialisation)
            {
                m_engine->octree()->AddObject(m_particles[index]->GetCollisionMesh());
            }

            UVu += 0.5;
        }
        UVu = 0;
        UVv += 0.5;
    }

    // Set a centered particle as the one to draw any collision solve diagnostics
    const int middle = ((m_particleLength/2) * m_particleLength) + (m_particleLength/2);
    auto& collision = m_particles[middle]->GetCollisionMesh();
    collision.SetRenderCollisionDiagnostics(true);

    // Create the vertices
    m_quadVertices = m_subdivideCloth ? ((m_particleLength-1)*(m_particleLength-1)) : 0;
    const int vertexCount = m_particleCount + m_quadVertices;
    m_vertexData.resize(vertexCount);

    // Create the indices
    const int trianglesPerQuad = m_subdivideCloth ? 4 : 2;
    int triangleNumber = ((m_particleLength-1)*(m_particleLength-1)) * trianglesPerQuad;
    m_indexData.resize(triangleNumber * POINTS_IN_FACE);

    index = 0;
    int quad = 0;

    for(int x = 0; x < m_particleLength-1; ++x)
    {
        for(int y = 0; y < m_particleLength-1; ++y)
        {
            if(m_subdivideCloth)
            {
                m_indexData[index]   = (x*m_particleLength)+y;
                m_indexData[index+1] = (x*m_particleLength)+y+1;
                m_indexData[index+2] = m_particleCount + quad;

                m_indexData[index+3] = m_particleCount + quad;
                m_indexData[index+4] = (x*m_particleLength)+y+1;
                m_indexData[index+5] = ((x+1)*m_particleLength)+y+1;
            
                m_indexData[index+6] = ((x+1)*m_particleLength)+y;
                m_indexData[index+7] = ((x+1)*m_particleLength)+y+1;
                m_indexData[index+8] = m_particleCount + quad;
            
                m_indexData[index+9] = (x*m_particleLength)+y;
                m_indexData[index+10] = m_particleCount + quad;
                m_indexData[index+11] = ((x+1)*m_particleLength)+y;
            }
            else
            {
                m_indexData[index] = (x*m_particleLength)+y;
                m_indexData[index+1] = (x*m_particleLength)+y+1;
                m_indexData[index+2] = ((x+1)*m_particleLength)+y;

                m_indexData[index+3] = ((x+1)*m_particleLength)+y;
                m_indexData[index+4] = (x*m_particleLength)+y+1;
                m_indexData[index+5] = ((x+1)*m_particleLength)+y+1;
            }

            ++quad;
            index += m_subdivideCloth ? 12 : 6;
        }
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

    m_springCount = ((m_particleLength-1)*(((m_particleLength-2)*2)+2)) 
        + (m_particleLength*(m_particleLength-1)) 
        + (m_particleLength*(m_particleLength-2))
        + ((m_particleLength-2)*m_particleLength)
        + ((m_particleLength-1)*m_particleLength);

    m_springs.resize(m_springCount);
    index = NO_INDEX;
    for(int x = 0; x < m_particleLength; ++x)
    {
        for(int y = 0; y < m_particleLength; ++y)
        {
            //Last y doesn't have cross springs
            if(y < m_particleLength-1)
            {
                if(x < m_particleLength-1) //Don't create right cross if last x
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
            if(x < m_particleLength-2)
            {
                index = createSpring(index);
                m_springs[index]->Initialise(*GetParticle(x,y),
                    *GetParticle(x+2,y), index, Spring::BEND);
            }

            //Last x doesn't have horizontal springs
            if(x < m_particleLength-1)
            {
                index = createSpring(index);
                m_springs[index]->Initialise(*GetParticle(x,y),
                    *GetParticle(x+1,y), index, Spring::STRETCH);
            }

            //Last 2ys doesn't have bending vertical springs
            if(y < m_particleLength-2)
            {
                index = createSpring(index);
                m_springs[index]->Initialise(*GetParticle(x,y),
                    *GetParticle(x,y+2), index, Spring::BEND);
            }
            
            //Last y doesn't have vertical springs
            if(y < m_particleLength-1)
            {
                index = createSpring(index);
                m_springs[index]->Initialise(*GetParticle(x,y),
                    *GetParticle(x,y+1), index, Spring::STRETCH);
            }
        }
    }

    //Mesh Vertex Declaration
    D3DVERTEXELEMENT9 VertexDec[] =
    {
        { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },     
        { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    };

    //Create the mesh
    if(m_mesh)
    {
        m_mesh->Release();
        m_mesh = nullptr;
    }

    if(FAILED(D3DXCreateMesh(triangleNumber, m_vertexData.size(),
        D3DXMESH_VB_DYNAMIC | D3DXMESH_IB_MANAGED | D3DXMESH_32BIT,
        VertexDec, m_engine->device(), &m_mesh)))
    {
        m_mesh = nullptr;
        ShowMessageBox("Cloth Mesh creation failed");
    }

    UpdateVertices();

    #pragma warning(disable: 4996)

    //Vertex Buffer
    void* vertexBuffer = nullptr;
    if(FAILED(m_mesh->LockVertexBuffer(0, &vertexBuffer)))
    {
        ShowMessageBox("Cloth Vertex buffer lock failed");
    }
    std::copy(m_vertexData.begin(), m_vertexData.end(), (MeshVertex*)vertexBuffer);
    m_mesh->UnlockVertexBuffer();

    //Index Buffer
    void* indexdata = nullptr;
    if(FAILED(m_mesh->LockIndexBuffer(0, &indexdata)))
    {
        ShowMessageBox("Cloth Index buffer lock failed");
    }
    std::copy(m_indexData.begin(), m_indexData.end(), (DWORD*)indexdata);
    m_mesh->UnlockIndexBuffer();
}

void Cloth::Draw(const D3DXVECTOR3& cameraPos, const Matrix& projection, const Matrix& view)
{
    m_shader->SetTechnique(DxConstant::DefaultTechnique);
    m_shader->SetFloatArray(DxConstant::CameraPosition, &(cameraPos.x), 3);
    m_shader->SetTexture(DxConstant::DiffuseTexture, m_texture);
    m_engine->sendLightsToShader(m_shader);

    D3DXMATRIX worldInvTrans;
    D3DXMATRIX worldViewProj = GetMatrix() * view.GetMatrix() * projection.GetMatrix();
    D3DXMatrixInverse(&worldInvTrans, 0, &GetMatrix());
    D3DXMatrixTranspose(&worldInvTrans, &worldInvTrans);

    m_shader->SetMatrix(DxConstant::WorldInverseTranspose, &worldInvTrans);
    m_shader->SetMatrix(DxConstant::WordViewProjection, &worldViewProj);
    m_shader->SetMatrix(DxConstant::World, &GetMatrix());

    UINT nPasses = 0;
    m_shader->Begin(&nPasses, 0);
    for(UINT iPass = 0; iPass < nPasses; ++iPass)
    {
        m_shader->BeginPass(iPass);
        m_mesh->DrawSubset(0);
        m_shader->EndPass();
    }
    m_shader->End();
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
    for(const ParticlePtr& particle : m_particles)
    {
        particle->AddForce(force);
    }
}

void Cloth::PreCollisionUpdate(float deltatime)
{
    UpdateDiagnostics();

    if(m_simulation || m_handleMode)
    {
        // Move cloth down slowly
        if(m_simulation)
        {
            AddForce(m_gravity*m_timestepSquared*deltatime);
        }
    
        // Solve Springs
        for(int j = 0; j < m_springIterations; ++j)
        {
            for(const SpringPtr& spring : m_springs)
            {
                spring->SolveSpring();
            }
        }

        // Updating particle positions
        for(const ParticlePtr& particle : m_particles)
        {
            particle->PreCollisionUpdate(m_damping, m_timestepSquared);
        }
    }
}

void Cloth::UpdateDiagnostics()
{
    if(m_engine->diagnostic()->AllowDiagnostics(Diagnostic::CLOTH))
    {
        std::for_each(m_springs.begin(), m_springs.end(), [&](const SpringPtr& spring)
        { 
            spring->UpdateDiagnostic(m_engine->diagnostic()); 
        });

        m_engine->diagnostic()->UpdateText(Diagnostic::CLOTH, 
            "ParticleCount", Diagnostic::WHITE, StringCast(m_particleCount));

        m_engine->diagnostic()->UpdateText(Diagnostic::CLOTH, 
            "Smoothing", Diagnostic::WHITE, StringCast(m_generalSmoothing));
    }
}

void Cloth::Reset()
{
    for(const ParticlePtr& particle : m_particles)
    {
        particle->ResetPosition();
    }
    UpdateVertexBuffer();
}

Cloth::ParticlePtr& Cloth::GetParticle(int row, int column)
{
    return m_particles[(column * m_particleLength) + row];
}

void Cloth::DrawCollisions(const Matrix& projection, const Matrix& view)
{
    if(m_drawColParticles)
    {
        for(const ParticlePtr& particle : m_particles)
        {
            particle->DrawCollisionMesh(projection, view);
        }
    }

    if(m_drawVisualParticles)
    {
        for(unsigned int i = 0; i < m_particles.size(); ++i)
        {
            // Draw visual particles at smoothed position
            m_particles[i]->DrawVisualMesh(projection, 
                view, m_vertexData[i].position);
        }
    }
}

bool Cloth::MousePickingTest(Picking& input)
{
    if(m_drawVisualParticles && !input.IsLocked())
    {
        int indexChosen = NO_INDEX;
        for(int index = 0; index < m_particleCount; ++index)
        {
            const CollisionMesh& mesh = m_particles[index]->GetCollisionMesh();
            const Geometry& geometry = *mesh.GetGeometry();

            if(input.RayCastSphere(mesh.GetPosition(), mesh.GetRadius()))
            {
                if(input.RayCastMesh(this, mesh.CollisionMatrix().GetMatrix(), geometry))
                {
                    indexChosen = index;
                }
            }
        }
    
        //Update the mesh pick function with selected index
        if(indexChosen != NO_INDEX)
        {
            SetMeshPickFunction(std::bind(&Cloth::SelectParticle, this, indexChosen));
            return true;
        }
    }
    return false;
}

void Cloth::SetParticleColor(const Cloth::ParticlePtr& particle)
{
    ParticleColors color = (particle->IsPinned() ? PINNED : 
        (particle->IsSelected() && m_handleMode ? SELECTED : NORMAL));
    particle->SetColor(m_colors[color]);
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
        D3DXVECTOR3 direction(right, up, forward);
        for(const ParticlePtr& particle : m_particles)
        {
            if(particle->IsSelected())
            { 
                particle->AddForce(direction); 
            } 
        }
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
        getIndexFn = [&](){ index = (m_particleLength*counter) + (m_particleLength-1); };
        break;
    case 3:
        getIndexFn = [&](){ index = (m_particleLength*(m_particleLength-1))+counter; };
        break;
    case 4:
        getIndexFn = [&](){ index = m_particleLength*counter; };
        break;
    }

    for(; counter < m_particleLength; ++counter)
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
        CreateCloth(m_particleLength, static_cast<float>(size));
    }
}

void Cloth::SetVertexRows(double number)
{
    if(number != m_particleLength)
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
    return m_particleLength;
}

double Cloth::GetSpacing() const
{
    return m_spacing;
}

std::vector<Cloth::ParticlePtr>& Cloth::GetParticles()
{
    return m_particles;
}

void Cloth::PostCollisionUpdate()
{
    // Update the collision mesh last after all movement has been decided
    for(const ParticlePtr& particle : m_particles)
    {
        particle->PostCollisionUpdate();
    }

    UpdateVertexBuffer();
}

bool Cloth::UpdateVertexBuffer()
{
    UpdateVertices();
    SmoothCloth();
    UpdateNormals();
    UpdateSubdividedVertices();

    //Lock the vertex buffer
    void* vertexBuffer = nullptr;
    if(FAILED(m_mesh->LockVertexBuffer(0, &vertexBuffer)))
    {
        ShowMessageBox("Vertex buffer lock failed");
        return false;
    }

    //Copy over values 
    #pragma warning(disable: 4996)
    std::copy(m_vertexData.begin(), m_vertexData.end(), (MeshVertex*)vertexBuffer);

    //unlock vertex buffer
    m_mesh->UnlockVertexBuffer();
    return true;
}

D3DXVECTOR3 Cloth::CalculateNormal(const D3DXVECTOR3& p1, 
    const D3DXVECTOR3& p2, const D3DXVECTOR3& p3)
{
    D3DXVECTOR3 normal;
    D3DXVec3Cross(&normal, &(p2-p1), &(p3-p1));
    return normal;
}

void Cloth::ChangeSmoothing(bool increase)
{
    m_generalSmoothing += increase ? SMOOTH_INCREASE : -SMOOTH_INCREASE;
    m_generalSmoothing = min(m_generalSmoothing, 1.0f);
    m_generalSmoothing = max(m_generalSmoothing, 0.0f);
}

void Cloth::UpdateVertices()
{
    int index = NO_INDEX;
    D3DXVECTOR3 normal(0.0f, 0.0f, 0.0f);

    for(int x = 0; x < m_particleLength; ++x)
    {
        for(int y = 0; y < m_particleLength; ++y)
        {
            index = (x*m_particleLength)+y;
            m_vertexData[index].normal = normal;
            m_vertexData[index].uvs = m_particles[index]->GetUVs();
            m_vertexData[index].position = m_particles[index]->GetPosition();
        }
    }
}

void Cloth::UpdateNormals()
{
    D3DXVECTOR3 normal;
    int p1, p2, p3, p4;

    for(int x = 0; x < m_particleLength-1; ++x)
    {
        for(int y = 0; y < m_particleLength-1; ++y)
        {
            p1 = (x*m_particleLength)+y;
            p2 = ((x+1)*m_particleLength)+y;
            p3 = (x*m_particleLength)+y+1;
            p4 = ((x+1)*m_particleLength)+y+1;

            normal = CalculateNormal(m_vertexData[p2].position,
                m_vertexData[p1].position, m_vertexData[p3].position);

            m_vertexData[p2].normal += normal;
            m_vertexData[p1].normal += normal;
            m_vertexData[p3].normal += normal;

            normal = CalculateNormal(m_vertexData[p4].position,
                m_vertexData[p2].position, m_vertexData[p3].position);

            m_vertexData[p4].normal += normal;
            m_vertexData[p2].normal += normal;
            m_vertexData[p3].normal += normal;
        }
    }
}

void Cloth::SmoothCloth()
{
    if(m_generalSmoothing > 0.0f)
    {
        int index = NO_INDEX;
        D3DXVECTOR3 halfp1, halfp2;
        D3DXVECTOR3 positionDifference;
        D3DXVECTOR3 smoothedPosition;
        int p1, p2, p3, p4;

        for(int x = 1; x < m_particleLength-1; ++x)
        {
            for(int y = 1; y < m_particleLength-1; ++y)
            {
                index = (x*m_particleLength)+y;
                if(m_particles[index]->RequiresSmoothing())
                {
                    p1 = ((x+1)*m_particleLength)+y+1;
                    p2 = ((x+1)*m_particleLength)+y-1;
                    p3 = ((x-1)*m_particleLength)+y+1;
                    p4 = ((x-1)*m_particleLength)+y-1;

                    halfp1 = (m_vertexData[p1].position
                        + m_vertexData[p4].position) * 0.5f;

                    halfp2 = (m_vertexData[p2].position
                        + m_vertexData[p3].position) * 0.5f;

                    smoothedPosition = (halfp1 + halfp2) * 0.5f;
                    positionDifference = smoothedPosition - m_vertexData[index].position;
                    m_vertexData[index].position += positionDifference * m_generalSmoothing;
                }
            }
        }
    }
}

void Cloth::UpdateSubdividedVertices()
{
    if(m_subdivideCloth)
    {
        int quad = 0;
        int quadindex = 0;
        D3DXVECTOR2 halfuv1, halfuv2;
        D3DXVECTOR3 halfp1, halfp2;
        int p1, p2, p3, p4;

        for(int x = 0; x < m_particleLength-1; ++x)
        {
            for(int y = 0; y < m_particleLength-1; ++y)
            {
                p1 = (x*m_particleLength)+y;
                p2 = ((x+1)*m_particleLength)+y;
                p3 = (x*m_particleLength)+y+1;
                p4 = ((x+1)*m_particleLength)+y+1;
                quadindex = m_particleCount + quad;

                halfp1 = (m_vertexData[p1].position
                    + m_vertexData[p4].position) * 0.5f;

                halfp2 = (m_vertexData[p2].position
                    + m_vertexData[p3].position) * 0.5f;

                halfuv1 = (m_vertexData[p1].uvs
                    + m_vertexData[p4].uvs) * 0.5f;

                halfuv2 = (m_vertexData[p2].uvs
                    + m_vertexData[p3].uvs) * 0.5f;

                m_vertexData[quadindex].position = (halfp1 + halfp2) * 0.5f;
                m_vertexData[quadindex].uvs = (halfuv1 + halfuv2) * 0.5f;
                m_vertexData[quadindex].normal = (m_vertexData[p2].normal 
                    + m_vertexData[p1].normal + m_vertexData[p3].normal 
                    + m_vertexData[p4].normal) * 0.25f;

                quad += 1;
            }
        }
    }
}