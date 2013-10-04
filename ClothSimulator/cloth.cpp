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

    /**
    * Initial values for the cloth
    */
    const int ROWS = 20;         
    const int ITERATIONS = 6;    
    const float TIMESTEP = 0.5f; 
    const float DAMPING = 0.9f;  
    const float SPACING = 0.75f; 
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
    m_gravity(0,-9.8f,0),
    m_diagnosticSelect(false),
    m_diagnosticParticle(0),
    m_engine(engine),
    m_vertexBuffer(nullptr),
    m_indexBuffer(nullptr)
{
    m_data.reset(new MeshData());
    m_data->shader = m_engine->getShader(ShaderManager::CLOTH_SHADER);
    m_template.reset(new CollisionMesh(*this, m_engine));
    m_template->LoadSphere(m_engine->device(), 1.0f, 8);

    if(FAILED(D3DXCreateTextureFromFile(m_engine->device(), 
        ".\\Resources\\Textures\\square.png", &m_data->texture)))
    {
        m_data->texture = nullptr;
        ShowMessageBox("Cannot create cloth texture");
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
    m_particleLength = rows;
    m_particleCount = rows*rows;

    //create particles
    m_particles.resize(m_particleCount);
    m_template->GetData().localWorld.SetScale(m_spacing/2.0f);

    const int mininum = -m_particleLength/2;
    const int maximum = m_particleLength/2;
    const D3DXVECTOR3 startingPos(1.0f, 8.0f, 0.0f);
    float UVu = 0;
    float UVv = 0;
    int index = 0;

    for(int x = mininum; x < maximum; ++x)
    {
        for(int z = mininum; z < maximum; ++z, ++index)
        {
            D3DXVECTOR2 uvs(UVu, UVv);
            D3DXVECTOR3 position = startingPos;
            position.x += x*m_spacing;
            position.z += z*m_spacing;

            if(!m_particles[index].get())
            {
                m_particles[index].reset(new Particle(m_engine));
            }
            m_particles[index]->Initialise(position, uvs,
                index, m_template->GetGeometry(), m_template->GetData());

            UVu += 0.5;
        }
        UVu = 0;
        UVv += 0.5;
    }

    //create vertices
    const int vertexCount = m_particleCount + ((m_particleLength-1)*(m_particleLength-1));
    m_vertexData.resize(vertexCount);
    m_quadVertices = vertexCount-m_particleCount;

    //create indices
    int triangleNumber = ((m_particleLength-1)*(m_particleLength-1))*4;
    m_indexData.resize(triangleNumber*3);

    index = 0;
    int quad = 0;

    for(int x = 0; x < m_particleLength-1; ++x)
    {
        for(int y = 0; y < m_particleLength-1; ++y)
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

            ++quad;
            index += 12;
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
    if(m_data->mesh)
    {
        m_data->mesh->Release();
        m_data->mesh = nullptr;
    }

    if(FAILED(D3DXCreateMesh(triangleNumber, m_vertexData.size(),
        D3DXMESH_VB_DYNAMIC | D3DXMESH_IB_MANAGED | D3DXMESH_32BIT,
        VertexDec, m_engine->device(), &m_data->mesh)))
    {
        m_data->mesh = nullptr;
        ShowMessageBox("Cloth Mesh creation failed");
    }
    SmoothCloth();

    //Vertex Buffer
    m_vertexBuffer = nullptr;
    if(FAILED(m_data->mesh->LockVertexBuffer(0,&m_vertexBuffer)))
    {
        ShowMessageBox("Cloth Vertex buffer lock failed");
    }
    #pragma warning(disable: 4996)
    std::copy(m_vertexData.begin(), m_vertexData.end(),(Vertex*)m_vertexBuffer);
    m_data->mesh->UnlockVertexBuffer();

    //Index Buffer
    m_indexBuffer = nullptr;
    if(FAILED(m_data->mesh->LockIndexBuffer(0,&m_indexBuffer)))
    {
        ShowMessageBox("Cloth Index buffer lock failed");
    }
    #pragma warning(disable: 4996)
    std::copy(m_indexData.begin(), m_indexData.end(), (DWORD*)m_indexBuffer);
    m_data->mesh->UnlockIndexBuffer();
}

void Cloth::Draw(const D3DXVECTOR3& cameraPos, const Matrix& projection, const Matrix& view)
{
    if(m_data->mesh)
    {
        LPD3DXEFFECT effect = m_data->shader->GetEffect();

        effect->SetTechnique(DxConstant::DefaultTechnique);
        effect->SetFloatArray(DxConstant::CameraPosition, &(cameraPos.x), 3);
        effect->SetTexture(DxConstant::DiffuseTexture, m_data->texture);
        m_engine->sendLightingToEffect(effect);

        D3DXMATRIX wit;
        D3DXMATRIX wvp = GetMatrix() * view.GetMatrix() * projection.GetMatrix();
        float det = D3DXMatrixDeterminant(&GetMatrix());
        D3DXMatrixInverse(&wit, &det, &GetMatrix());
        D3DXMatrixTranspose(&wit, &wit);

        effect->SetMatrix(DxConstant::WorldInverseTranspose, &wit);
        effect->SetMatrix(DxConstant::WordViewProjection, &wvp);
        effect->SetMatrix(DxConstant::World, &GetMatrix());

        UINT nPasses = 0;
        effect->Begin(&nPasses, 0);
        for(UINT iPass = 0; iPass<nPasses; ++iPass)
        {
            effect->BeginPass(iPass);
            m_data->mesh->DrawSubset(0);
            effect->EndPass();
        }
        effect->End();
    }
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

Cloth::ParticlePtr& Cloth::GetParticle(int row, int col)
{
    return m_particles[col*m_particleLength + row];
}

void Cloth::DrawCollisions(const Matrix& projection, const Matrix& view)
{
    if(m_engine->diagnostic()->AllowDiagnostics(Diagnostic::CLOTH))
    {
        const float radius = 0.4f;
        const auto& position = m_particles[m_diagnosticParticle]->GetPosition();

        m_engine->diagnostic()->UpdateSphere(Diagnostic::CLOTH, 
            "Particle", Diagnostic::YELLOW, position, radius);

        std::for_each(m_springs.begin(), m_springs.end(), [&](const SpringPtr& spring)
        { 
            spring->UpdateDiagnostic(m_engine->diagnostic()); 
        });
    }

    if(m_engine->diagnostic()->AllowText())
    {
        const auto& vertex = m_vertexData[m_diagnosticParticle].position;
        const auto& position = m_particles[m_diagnosticParticle]->GetPosition();
        const auto& collision = m_particles[m_diagnosticParticle]->GetCollisionMesh()->GetPosition();

        m_engine->diagnostic()->UpdateText("Particle",
            Diagnostic::YELLOW, StringCast(position.x) + ", " +
            StringCast(position.y) + ", " + StringCast(position.z));

        m_engine->diagnostic()->UpdateText("Collision", 
            Diagnostic::YELLOW, StringCast(collision.x) + ", " + 
            StringCast(collision.y) + ", " + StringCast(collision.z));

        m_engine->diagnostic()->UpdateText("Vertex",
            Diagnostic::YELLOW, StringCast(vertex.x) + ", " + 
            StringCast(vertex.y) + ", " + StringCast(vertex.z));
    }

    if(m_drawColParticles)
    {
        std::for_each(m_particles.begin(), m_particles.end(), 
            [&](const ParticlePtr& part){ part->DrawCollisionMeshMesh(projection, view); });
    }

    if(m_drawVisualParticles)
    {
        std::for_each(m_particles.begin(), m_particles.end(), 
            [&](const ParticlePtr& part){ part->DrawVisualMesh(projection, view); });
    }
}

bool Cloth::MousePickingTest(Picking& input)
{
    if(m_drawVisualParticles && !input.IsLocked())
    {
        int indexChosen = NO_INDEX;
        for(int i = 0; i < m_particleCount; ++i)
        {
            D3DXMATRIX worldInverse;
            D3DXMatrixInverse(&worldInverse, NULL,
                &m_particles[i]->GetCollisionMesh()->CollisionMatrix().GetMatrix());

            D3DXVECTOR3 rayObjOrigin;
            D3DXVECTOR3 rayObjDirection;
                
            D3DXVec3TransformCoord(&rayObjOrigin, &input.GetRayOrigin(), &worldInverse);
            D3DXVec3TransformNormal(&rayObjDirection, &input.GetRayDirection(), &worldInverse);
            D3DXVec3Normalize(&rayObjDirection, &rayObjDirection);
    
            BOOL hasHit; 
            if(FAILED(D3DXIntersect(m_particles[i]->GetCollisionMesh()->GetMesh(), &rayObjOrigin, 
                &rayObjDirection, &hasHit, NULL, NULL, NULL, NULL, NULL, NULL)))
            {
                hasHit = false; //Call failed for any reason continue to next mesh.
            }
    
            if(hasHit)
            {
                D3DXVECTOR3 cameraToMesh = input.GetRayOrigin()-m_particles[i]->GetPosition();
                float distanceToCollisionMesh = D3DXVec3Length(&cameraToMesh);
                if(distanceToCollisionMesh < input.GetDistanceToMesh())
                {
                    input.SetPickedMesh(this, distanceToCollisionMesh);
                    indexChosen = i;
                }
            }
        }
    
        //Update the mesh pick function with selected index
        if(indexChosen != NO_INDEX)
        {
            auto selectFn = m_diagnosticSelect ? &Cloth::SelectParticleForDiagnostics : &Cloth::SelectParticle;
            SetMeshPickFunction(std::bind(selectFn, this, indexChosen));
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
        auto addForce = [&](const ParticlePtr& part)
        { 
            if(part->IsSelected())
            { 
                part->AddForce(direction); 
            } 
        };
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

bool Cloth::UpdateVertexBuffer()
{
    SmoothCloth();

    //Lock the vertex buffer
    if(FAILED(m_data->mesh->LockVertexBuffer(0,&m_vertexBuffer)))
    {
        ShowMessageBox("Vertex buffer lock failed");
        return false;
    }

    //Copy over values 
    #pragma warning(disable: 4996)
    std::copy(m_vertexData.begin(), m_vertexData.end(),(Vertex*)m_vertexBuffer);

    //unlock vertex buffer
    m_data->mesh->UnlockVertexBuffer();
    return true;
}

D3DXVECTOR3 Cloth::CalculateNormal(const D3DXVECTOR3& p1, 
    const D3DXVECTOR3& p2, const D3DXVECTOR3& p3)
{
    D3DXVECTOR3 normal;
    D3DXVec3Cross(&normal, &(p2-p1), &(p3-p1));
    return normal;
}

void Cloth::SmoothCloth()
{
    // m_vertexData contains m_particleCount vertices that make up the main
    // grid of the cloth. It contains an extra set of vertices equal to
    // the number of quads in the cloth which sit in the quads center
    // smoothing involves determining the position of this center vertex

    int quad = 0;
    int index = NO_INDEX;
    D3DXVECTOR3 normal(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 halfp1, halfp2;
    D3DXVECTOR2 halfuv1, halfuv2;

    for(int i = 0; i < m_particleLength; ++i)
    {
        quad = i;
        for(int j = 0; j < m_particleLength; ++j)
        {
            index = (i*m_particleLength)+j;
            m_vertexData[index].position = m_particles[index]->GetPosition();
            m_vertexData[index].uvs = m_particles[index]->GetUVs();
            m_vertexData[index].normal = normal;

            if(j < m_particleLength-1 && i < m_particleLength-1)
            {
                // Determine position of quad vertex
                // based on surrounding four vertices
                int quadindex = m_particleCount + quad;
                quad += m_particleLength-1;

                halfp1 = (GetParticle(i, j)->GetPosition() 
                    + GetParticle(i+1, j+1)->GetPosition()) * 0.5f;

                halfp2 = (GetParticle(i+1, j)->GetPosition()
                    + GetParticle(i, j+1)->GetPosition()) * 0.5f;

                halfuv1 = (GetParticle(i, j)->GetUVs() 
                    + GetParticle(i+1, j+1)->GetUVs()) * 0.5f;

                halfuv2 = (GetParticle(i+1, j)->GetUVs()
                    + GetParticle(i, j+1)->GetUVs()) * 0.5f;

                m_vertexData[quadindex].position = (halfp1 + halfp2) * 0.5f;
                m_vertexData[quadindex].uvs = (halfuv1 + halfuv2) * 0.5f;
                m_vertexData[quadindex].normal = normal;
            }
        }
    }

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

    quad = 0;
    for(int x = 0; x < m_particleLength-1; ++x)
    {
        for(int y = 0; y < m_particleLength-1; ++y)
        {
            p1 = (x*m_particleLength)+y;
            p2 = ((x+1)*m_particleLength)+y;
            p3 = (x*m_particleLength)+y+1;
            p4 = ((x+1)*m_particleLength)+y+1;

            // Each quad vertex has averaged of surrounding normals
            m_vertexData[m_particleCount + quad].normal = (m_vertexData[p2].normal 
                + m_vertexData[p1].normal + m_vertexData[p3].normal 
                + m_vertexData[p4].normal) * 0.25f;

            quad += 1;
        }
    }
}