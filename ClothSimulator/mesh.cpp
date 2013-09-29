////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - mesh.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "mesh.h"
#include "assimpmesh.h"
#include "collision.h"
#include "Shader.h"
#include "input.h"
#include "light.h"

Mesh::Mesh(EnginePtr engine):
    m_engine(engine),
    m_color(1.0f, 1.0f, 1.0f),
    m_initialcolor(1.0f, 1.0f, 1.0f),
    m_selectedcolor(0.7f, 0.7f, 1.0f),
    m_index(NO_INDEX),
    m_pickable(true),
    m_selected(false),
    m_draw(true),
    m_target(1),
    m_animating(false),
    m_reversing(false),
    m_speed(10.0)
{
    m_data.reset(new MeshData());
    m_collision.reset(new Collision(*this, m_engine));

    Transform::UpdateFn fullFn =
        std::bind(&Collision::FullUpdate, m_collision);

    Transform::UpdateFn positionalFn =
        std::bind(&Collision::PositionalUpdate, m_collision);

    SetObserver(fullFn, positionalFn);
}

MeshData::MeshData() :
    mesh(nullptr),
    texture(nullptr),
    shader(nullptr)
{
}

MeshData::~MeshData()
{
    if(texture != nullptr)
    { 
        texture->Release();
    }
    if(mesh != nullptr)
    { 
        mesh->Release();
    }
}

Mesh::~Mesh()
{
}

bool Mesh::LoadTexture(LPDIRECT3DDEVICE9 d3ddev, const std::string& filename, int dimensions, int miplevels)
{
    if(FAILED(D3DXCreateTextureFromFileEx(d3ddev, filename.c_str(), dimensions, 
        dimensions, miplevels, NULL, D3DFMT_FROM_FILE, D3DPOOL_DEFAULT, D3DX_DEFAULT, 
        D3DX_DEFAULT, NULL, NULL, NULL, &m_data->texture)))
    {
        ShowMessageBox("Cannot create texture " + filename);
        return false;
    }
    return true;
}

bool Mesh::Load(LPDIRECT3DDEVICE9 d3ddev, const std::string& filename, 
    std::shared_ptr<Shader> shader, int index)
{
    m_index = index;
    m_data->shader = shader;

    //Create a assimp mesh
    std::string errorBuffer;
    Assimpmesh mesh;
    if(!mesh.Initialise(filename, errorBuffer))
    {
        ShowMessageBox(errorBuffer);
        return false;
    }

    std::vector<unsigned long> indexData;
    std::vector<Vertex> vertexData;
    const std::vector<Assimpmesh::SubMesh>& subMeshes = mesh.GetMeshes();

    for(unsigned int i = 0; i < subMeshes.size(); ++i)
    {
        //Fill in vertex data
        for(unsigned int j = 0; j < subMeshes[i].vertices.size(); ++j)
        {
            Vertex v;
            v.normal.x = subMeshes[i].vertices[j].nx;
            v.normal.y = subMeshes[i].vertices[j].ny;
            v.normal.z = subMeshes[i].vertices[j].nz;
            v.position.x = subMeshes[i].vertices[j].x;
            v.position.y = subMeshes[i].vertices[j].y;
            v.position.z = subMeshes[i].vertices[j].z;
            v.uvs.x = subMeshes[i].vertices[j].u;
            v.uvs.y = subMeshes[i].vertices[j].v;
            vertexData.push_back(v);
        }

        for(unsigned int j = 0; j < subMeshes[i].indices.size(); ++j)
        {
            indexData.push_back(subMeshes[i].indices[j]);
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

    //Create the DirectX Mesh
    if(FAILED(D3DXCreateMesh(indexData.size()/3,
                             vertexData.size(),
                             D3DXMESH_MANAGED | D3DXMESH_32BIT,
                             VertexDec,
                             d3ddev,
                             &m_data->mesh)))
    {
        ShowMessageBox("Mesh " + filename + " creation failed");
        return false;
    }

    //Fill in the vertex buffer
    Vertex* pVertexBuffer;
    if(FAILED( m_data->mesh->LockVertexBuffer(0, (void**)&pVertexBuffer)))
    {
        ShowMessageBox(filename + " Vertex buffer lock failed");
        return false;
    }
    #pragma warning(disable: 4996)
    std::copy(vertexData.begin(), vertexData.end(), pVertexBuffer);
    m_data->mesh->UnlockVertexBuffer();

    //Fill in the index buffer
    DWORD* pm_indexBuffer;
    if(FAILED(m_data->mesh->LockIndexBuffer(0, (void**)&pm_indexBuffer)))
    {
        ShowMessageBox(filename + " Index buffer lock failed");
        return false;
    }
    #pragma warning(disable: 4996)
    std::copy(indexData.begin(), indexData.end(), pm_indexBuffer);
    m_data->mesh->UnlockIndexBuffer();

    return true;
}

bool Mesh::LoadAsInstance(LPDIRECT3DDEVICE9 d3ddev, const Collision* collision,
    std::shared_ptr<MeshData> data, int index)
{
    m_index = index;
    m_data = data;
    if(collision)
    {
        m_collision->LoadInstance(collision->GetData(), collision->GetGeometry());
    }
    return true;
}

void Mesh::DrawMesh(const D3DXVECTOR3& cameraPos,
    const Matrix& projection, const Matrix& view, float deltatime)
{
    if(m_data->mesh && m_draw)
    {
        Animate(deltatime);

        LPD3DXEFFECT effect = m_data->shader->GetEffect();

        effect->SetTechnique(DxConstant::DefaultTechnique);
        effect->SetFloatArray(DxConstant::CameraPosition, &(cameraPos.x), 3);
        effect->SetFloatArray(DxConstant::VertexColor, &(m_color.x), 3);
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

void Mesh::DrawCollision(const Matrix& projection, const Matrix& view)
{
    if(m_collision && m_draw)
    {
        m_collision->Draw(projection, view);
    }
}

Collision* Mesh::GetCollision()
{
    return m_collision.get();
}

bool Mesh::MousePickingTest(Picking& input)
{
    if(m_pickable && m_draw && m_data->mesh && !input.IsLocked())
    {
        LPD3DXMESH meshToTest = m_collision->HasGeometry()
            ? m_collision->GetMesh() : m_data->mesh;

        const Matrix& world = m_collision->HasGeometry()
            ? m_collision->CollisionMatrix() : *this;

        D3DXMATRIX worldInverse;
        D3DXMatrixInverse(&worldInverse, NULL, &world.GetMatrix());

        D3DXVECTOR3 rayOrigin;
        D3DXVECTOR3 rayDirection;
        D3DXVec3TransformCoord(&rayOrigin, &input.GetRayOrigin(), &worldInverse);
        D3DXVec3TransformNormal(&rayDirection, &input.GetRayDirection(), &worldInverse);
        D3DXVec3Normalize(&rayDirection, &rayDirection);

        BOOL hasHit; 
        float distanceToCollision;
        if(FAILED(D3DXIntersect(meshToTest, &rayOrigin, &rayDirection, 
            &hasHit, NULL, NULL, NULL, &distanceToCollision, NULL, NULL)))
        {
            hasHit = false; //Call failed for any reason continue to next mesh.
        }

        if(hasHit)
        {
            D3DXVECTOR3 cameraToMesh = input.GetRayOrigin()-world.Position();
            float distanceToCollision = D3DXVec3Length(&cameraToMesh);
            if(distanceToCollision < input.GetDistanceToMesh())
            {
                input.SetPickedMesh(this, distanceToCollision);
                return true;
            }
        }
    }
    return false;
}

void Mesh::SetCollisionVisibility(bool draw)
{
    if(m_collision)
    {
        m_collision->SetDraw(draw);
    }
}

void Mesh::SetVisible(bool visible)
{
    m_draw = visible;
    m_animation.clear();
}

void Mesh::ToggleSelected()
{
    m_selected = !m_selected;
}

bool Mesh::HasCollision() const
{
    return m_collision->HasGeometry();
}

void Mesh::CreateCollision(LPDIRECT3DDEVICE9 d3ddev, float radius, float length, int quality)
{
    m_collision->LoadCylinder(d3ddev, radius, length, quality);
}

void Mesh::CreateCollision(LPDIRECT3DDEVICE9 d3ddev, float width, float height, float depth)
{
    m_collision->LoadBox(d3ddev, width, height, depth);
}

void Mesh::CreateCollision(LPDIRECT3DDEVICE9 d3ddev, float radius, int quality)
{
    m_collision->LoadSphere(d3ddev, radius, quality);
}

bool Mesh::IsVisible() const
{
    return m_draw;
}

void Mesh::SetPickable(bool pickable)
{
    m_pickable = pickable;
}

std::shared_ptr<MeshData> Mesh::GetData()
{
    return m_data;
}

int Mesh::GetIndex() const
{
    return m_index;
}

void Mesh::SetSelected(bool selected)
{
    m_selected = selected;
    m_color = selected ? m_selectedcolor : m_initialcolor;
    m_animating = !selected;

    if(selected && !m_animation.empty())
    {
        // snap mesh to end of animation
        SetPosition(m_animation[m_animation.size()-1]);
        m_target = m_animation.size()-1;
        m_reversing = false;
    }
}

void Mesh::SetColor(float r, float g, float b)
{
    m_initialcolor.x = r;
    m_initialcolor.y = g;
    m_initialcolor.z = b;
    m_color = m_initialcolor;
}

void Mesh::SetSelectedColor(float r, float g, float b)
{
    m_selectedcolor.x = r;
    m_selectedcolor.y = g;
    m_selectedcolor.z = b;
}

const std::vector<D3DXVECTOR3>& Mesh::GetAnimationPoints() const
{
    return m_animation;
}

void Mesh::ResetAnimation()
{
    m_animation.clear();
    m_target = 1;
}

void Mesh::SavePosition()
{
    D3DXVECTOR3 position = Position();
    if(m_animation.empty() || m_animation[m_animation.size()-1] != position)
    {
        m_animation.push_back(position);
        m_target = m_animation.size()-1;
    }
}

void Mesh::Animate(float deltatime)
{
    if(m_animating && !m_animation.empty())
    {
        const float threshold = 1.0f;
        const float length = D3DXVec3Length(&(m_animation[m_target]-Position()));

        if(length < threshold)
        {
            m_target += m_reversing ? -1 : 1;
            if(m_target < 0)
            {
                m_reversing = !m_reversing;
                m_target = 1;
            }
            else if(m_target >= static_cast<int>(m_animation.size()))
            {
                m_reversing = !m_reversing;
                m_target = static_cast<int>(m_animation.size()-2);
            }
        }

        D3DXVECTOR3 path = m_animation[m_target] - 
            m_animation[m_target+(m_reversing ? 1 : -1)];

        D3DXVec3Normalize(&path, &path);
        Translate(path * m_speed * deltatime);
    }
}