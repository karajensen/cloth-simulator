#include "mesh.h"
#include "assimpmesh.h"
#include "collision.h"
#include "Shader.h"
#include "input.h"
#include "light.h"

namespace /*anon*/
{
    const D3DXVECTOR3 SELECTED_COLOR(0.7f, 0.7f, 1.0f);
}

Mesh::Mesh():
    m_selected(false),
    m_draw(true),
    m_pickable(true),
    m_index(NO_INDEX),
    m_color(1.0f, 1.0f, 1.0f),
    m_initialcolor(1.0f, 1.0f, 1.0f)
{
    m_data.reset(new MeshData());
    m_data->collision.reset(new Collision(*this));

    Transform::UpdateFn fullFn = std::bind(&Collision::FullUpdate, m_data->collision);
    Transform::UpdateFn positionalFn = std::bind(&Collision::PositionalUpdate, m_data->collision);
    SetObserver(fullFn, positionalFn);
}

Mesh::MeshData::MeshData() :
    mesh(nullptr),
    texture(nullptr),
    shader(nullptr),
    collision(nullptr)
{
}

Mesh::MeshData::~MeshData()
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

Mesh::Vertex::Vertex() :
    position(0.0f, 0.0f, 0.0f),
    normal(0.0f, 0.0f, 0.0f),
    uvs(0.0f, 0.0f)
{
}

void Mesh::DrawMesh(const D3DXVECTOR3& cameraPos, const Transform& projection, const Transform& view)
{
    if(m_data->mesh && m_draw)
    {
        LPD3DXEFFECT effect = Shader_Manager::UseWorldShader() 
            ? Shader_Manager::GetWorldEffect() : m_data->shader->GetEffect();

        effect->SetTechnique(DxConstant::DefaultTechnique);
        effect->SetFloatArray(DxConstant::CameraPosition, &(cameraPos.x), 3);
        effect->SetFloatArray(DxConstant::MeshColor, &(m_color.x), 3);

        if(m_data->texture != nullptr)
        {
            effect->SetTexture(DxConstant::DiffuseTexture, m_data->texture);
        }

        Light_Manager::SendLightingToShader(effect);

        D3DXMATRIX wit;
        D3DXMATRIX wvp = Matrix * view.Matrix * projection.Matrix;
        float det = D3DXMatrixDeterminant(&Matrix);
        D3DXMatrixInverse(&wit, &det, &Matrix);
        D3DXMatrixTranspose(&wit, &wit);

        effect->SetMatrix(DxConstant::WorldInverseTranspose, &wit);
        effect->SetMatrix(DxConstant::WordViewProjection, &wvp);
        effect->SetMatrix(DxConstant::World, &Matrix);

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

void Mesh::DrawCollision(const Transform& projection, const Transform& view)
{
    if(m_data->collision && m_draw)
    {
        m_data->collision->Draw(projection, view);
    }
}

Collision* Mesh::GetCollision()
{
    return m_data->collision.get();
}

void Mesh::MousePickingTest(Picking& input)
{
    if(m_pickable && m_draw && m_data->mesh)
    {
        LPD3DXMESH meshToTest = m_data->collision ? m_data->collision->GetMesh() : m_data->mesh;

        D3DXMATRIX worldInverse;
        D3DXMatrixInverse(&worldInverse, NULL, &Matrix);

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
            //if first mesh tested or if better than a previous test then save
            if(input.GetMesh() == nullptr || distanceToCollision < input.GetDistanceToMesh())
            {
                input.SetPickedMesh(this, distanceToCollision);
            }
        }
    }
}

void Mesh::SetCollisionVisibility(bool draw)
{
    if(m_data->collision)
    {
        m_data->collision->SetDraw(draw);
    }
}

void Mesh::SetVisible(bool visible)
{
    m_draw = visible;
}

void Mesh::ToggleSelected()
{
    m_selected = !m_selected;
}

bool Mesh::LoadTexture(LPDIRECT3DDEVICE9 d3ddev, const std::string& filename)
{
    if(FAILED(D3DXCreateTextureFromFile(d3ddev, filename.c_str(), &m_data->texture)))
    {
        Diagnostic::ShowMessage("Cannot create texture " + filename);
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
        Diagnostic::ShowMessage(errorBuffer);
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
        Diagnostic::ShowMessage("Mesh " + filename + " creation failed");
        return false;
    }

    //Fill in the vertex buffer
    Vertex* pVertexBuffer;
    if(FAILED( m_data->mesh->LockVertexBuffer(0, (void**)&pVertexBuffer)))
    {
        Diagnostic::ShowMessage(filename + " Vertex buffer lock failed");
        return false;
    }
    #pragma warning(disable: 4996)
    std::copy(vertexData.begin(), vertexData.end(), pVertexBuffer);
    m_data->mesh->UnlockVertexBuffer();

    //Fill in the index buffer
    DWORD* pm_indexBuffer;
    if(FAILED(m_data->mesh->LockIndexBuffer(0, (void**)&pm_indexBuffer)))
    {
        Diagnostic::ShowMessage(filename + " Index buffer lock failed");
        return false;
    }
    #pragma warning(disable: 4996)
    std::copy(indexData.begin(), indexData.end(), pm_indexBuffer);
    m_data->mesh->UnlockIndexBuffer();

    return true;
}

bool Mesh::LoadAsInstance(LPDIRECT3DDEVICE9 d3ddev, std::shared_ptr<MeshData> data, int index)
{
    m_index = index;
    m_data = data;
    Transform::UpdateFn fullFn = std::bind(&Collision::FullUpdate, m_data->collision);
    Transform::UpdateFn positionalFn = std::bind(&Collision::PositionalUpdate, m_data->collision);
    SetObserver(fullFn, positionalFn);
    return true;
}

void Mesh::CreateCollision(LPDIRECT3DDEVICE9 d3ddev, float radius, float length, int quality)
{
    m_data->collision->LoadCylinder(d3ddev, radius, length, quality);
}

void Mesh::CreateCollision(LPDIRECT3DDEVICE9 d3ddev, float width, float height, float depth)
{
    m_data->collision->LoadBox(d3ddev, width, height, depth);
}

void Mesh::CreateCollision(LPDIRECT3DDEVICE9 d3ddev, float radius, int quality)
{
    m_data->collision->LoadSphere(d3ddev, radius, quality);
}

bool Mesh::IsVisible() const
{
    return m_draw;
}

void Mesh::SetPickable(bool pickable)
{
    m_pickable = pickable;
}

std::shared_ptr<Mesh::MeshData> Mesh::GetData()
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
    m_color = selected ? SELECTED_COLOR : m_initialcolor;
}

void Mesh::SetColor(float r, float g, float b)
{
    m_initialcolor.x = r;
    m_initialcolor.y = g;
    m_initialcolor.z = b;
    m_color = m_initialcolor;
}