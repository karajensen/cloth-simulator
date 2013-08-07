
#include "collision.h"
#include "shader.h"

std::shared_ptr<Shader> Collision::sm_shader = nullptr;

Collision::Collision(const Transform* parent) :
    m_shape(NONE),
    m_draw(false),
    m_colour(0.0f, 0.0f, 1.0f),
    m_position(0.0f, 0.0f, 0.0f),
    m_parent(parent)
{
}

Collision::Geometry::Geometry() :
    mesh(nullptr)
{
}

Collision::Box::Box(LPDIRECT3DDEVICE9 d3ddev, float width, float height, float depth) :
    localMinBounds(-width/2.0f, -height/2.0f, -depth/2.0f),
    localMaxBounds(width/2.0f, height/2.0f, depth/2.0f),
    minBounds(0.0f, 0.0f, 0.0f),
    maxBounds(0.0f, 0.0f, 0.0f),
    dimensions(width, height, depth)
{
    D3DXCreateBox(d3ddev, width, height, depth, &mesh, NULL);
}

void Collision::LoadBox(LPDIRECT3DDEVICE9 d3ddev, float width, float height, float depth)
{
    Box* box = new Box(d3ddev, width, height, depth);
    m_data.reset(box);
    MakeBox(box);
}

void Collision::MakeBox(Box* boxdata)
{
    m_shape = BOX;
    m_localWorld.SetScale(boxdata->dimensions.x, boxdata->dimensions.y, boxdata->dimensions.z);
    m_world.Matrix = m_localWorld.Matrix * m_parent->Matrix;
    m_position = m_world.Position();
    D3DXVec3TransformCoord(&boxdata->minBounds, &boxdata->localMinBounds, &m_world.Matrix);
    D3DXVec3TransformCoord(&boxdata->maxBounds, &boxdata->localMaxBounds, &m_world.Matrix);
}

Collision::Sphere::Sphere(LPDIRECT3DDEVICE9 d3ddev, float radius, int divisions) :
    localRadius(radius),
    radius(0.0f)
{
    D3DXCreateSphere(d3ddev, radius, divisions, divisions, &mesh, NULL);
}

void Collision::LoadSphere(LPDIRECT3DDEVICE9 d3ddev, float radius, int divisions)
{
    Sphere* sphere = new Sphere(d3ddev, radius, divisions);
    m_data.reset(sphere);
    MakeSphere(sphere);
}

void Collision::MakeSphere(Sphere* spheredata)
{
    m_shape = SPHERE;
    m_localWorld.SetScale(spheredata->localRadius);
    m_world.Matrix = m_localWorld.Matrix * m_parent->Matrix;
    m_position = m_world.Position();
    spheredata->radius = spheredata->localRadius * m_world.GetScaleFactor().x;
}

Collision::Cylinder::Cylinder(LPDIRECT3DDEVICE9 d3ddev, float radius, float length, int divisions)
{
    D3DXCreateCylinder(d3ddev, radius, radius, length, divisions, 1, &mesh, NULL);
}

void Collision::LoadCylinder(LPDIRECT3DDEVICE9 d3ddev, float radius, float length, int divisions)
{
    Cylinder* cylinder = new Cylinder(d3ddev, radius, length, divisions);
    m_data.reset(cylinder);
    MakeCylinder(cylinder);
}

void Collision::MakeCylinder(Cylinder* cylinderdata)
{
    m_shape = CYLINDER;
    m_world.Matrix = m_localWorld.Matrix * m_parent->Matrix;
    m_position = m_world.Position();
}

void Collision::SetParent(const Transform* parent)
{
    m_parent = parent;
}

void Collision::LoadInstance(Shape shape, std::shared_ptr<Collision::Geometry> data)
{
    m_data = data;
    if(shape == SPHERE)
    {
        MakeSphere(&GetSphereData());
    }
    else if(shape == BOX)
    {
        MakeBox(&GetBoxData());
    }
    else if(shape == CYLINDER)
    {
        MakeCylinder(&GetCylinderData());
    }
}

Collision::Geometry::~Geometry()
{ 
    if(mesh != nullptr)
    { 
        mesh->Release(); 
    } 
}

void Collision::Initialise(std::shared_ptr<Shader> boundsShader)
{
    sm_shader = boundsShader;
}

LPD3DXMESH Collision::GetMesh()
{
    return m_data->mesh;
}

void Collision::SetDraw(bool draw) 
{ 
    m_draw = draw;
}

const D3DXVECTOR3& Collision::GetPosition() const 
{ 
    return m_position; 
}

const Transform& Collision::GetTransform() const
{
    return m_world;
}

std::shared_ptr<Collision::Geometry> Collision::GetData()
{
    return m_data;
}

Collision::Shape Collision::GetShape() const
{ 
    return m_shape;
}

void Collision::SetColor(const D3DXVECTOR3& color)
{ 
    m_colour = color;
}

void Collision::Draw(const Transform& projection, const Transform& view)
{
    if(m_draw && m_data && m_data->mesh)
    {
        LPD3DXEFFECT pEffect(sm_shader->GetEffect());
        pEffect->SetTechnique(DxConstant::DefaultTechnique);

        D3DXMATRIX wvp = m_world.Matrix * view.Matrix * projection.Matrix;
        pEffect->SetMatrix(DxConstant::WordViewProjection, &wvp);

        pEffect->SetFloatArray(DxConstant::VertexColor, &(m_colour.x), 3);

        UINT nPasses = 0;
        pEffect->Begin(&nPasses, 0);
        for( UINT iPass = 0; iPass<nPasses; iPass++)
        {
            pEffect->BeginPass(iPass);
            m_data->mesh->DrawSubset(0);
            pEffect->EndPass();
        }
        pEffect->End();
    }
}

Collision::Sphere& Collision::GetSphereData()
{
    return static_cast<Sphere&>(*m_data.get());
}

Collision::Box& Collision::GetBoxData()
{
    return static_cast<Box&>(*m_data.get());
}

Collision::Cylinder& Collision::GetCylinderData()
{
    return static_cast<Cylinder&>(*m_data.get());
}

const Collision::Sphere& Collision::GetSphereData() const
{
    return static_cast<Sphere&>(*m_data.get());
}

const Collision::Box& Collision::GetBoxData() const
{
    return static_cast<Box&>(*m_data.get());
}

const Collision::Cylinder& Collision::GetCylinderData() const
{
    return static_cast<Cylinder&>(*m_data.get());
}

void Collision::PositionalUpdate()
{
    if(m_data)
    {
        if(m_shape == BOX)
        {
            Box& box = GetBoxData();
            D3DXVECTOR3 difference(m_position-m_parent->Position());
            m_position += difference;
            m_world.Translate(difference);
            box.minBounds += difference;
            box.maxBounds += difference;
        }
        else if(m_shape == SPHERE)
        {
            m_position = m_parent->Position();
            m_world.SetPosition(m_position);
        }
    }
}

void Collision::FullUpdate()
{
    if(m_data)
    {
        //DirectX:  World = LocalWorld * ParentWorld
        m_world.Matrix = m_localWorld.Matrix * m_parent->Matrix;
        m_position = m_world.Position();

        if(m_shape == BOX)
        {
            Box& box = GetBoxData();
            D3DXVec3TransformCoord(&box.minBounds, &box.localMinBounds, &m_world.Matrix);
            D3DXVec3TransformCoord(&box.maxBounds, &box.localMaxBounds, &m_world.Matrix);
        }
        else if(m_shape == SPHERE)
        {
            //assumes uniform scaling
            Sphere& sphere = GetSphereData();
            sphere.radius = sphere.localRadius * m_world.GetScaleFactor().x;
        }
    }
}

bool Collision::TestCollision(const Collision* obj) const
{
    if(m_data)
    {
        if(m_shape == BOX && obj->GetShape() == BOX)
        {
            return CollisionTester::TestBoxBoxCollision(this, obj);
        }
        else if(m_shape == BOX && obj->GetShape() == SPHERE)
        {
            return CollisionTester::TestBoxSphereCollision(this, obj);
        }
        else if(m_shape == SPHERE && obj->GetShape() == BOX)
        {
            return CollisionTester::TestBoxSphereCollision(obj, this);
        }
        else if(m_shape == SPHERE && obj->GetShape() == SPHERE)
        {
            return CollisionTester::TestSphereSphereCollision(this, obj);
        }
    }
    return false;
}

bool CollisionTester::TestSphereSphereCollision(const Collision* sphere1, const Collision* sphere2)
{
    const Collision::Sphere& sphere1data = sphere1->GetSphereData();
    const Collision::Sphere& sphere2data = sphere2->GetSphereData();

    D3DVECTOR relPos = sphere1->GetPosition() - sphere2->GetPosition();
    float dist = (relPos.x * relPos.x) + (relPos.y * relPos.y) + (relPos.z * relPos.z);
    float minDist = sphere1data.radius + sphere2data.radius;
    return (dist <= (pow(minDist,2)));
}

bool CollisionTester::TestBoxBoxCollision(const Collision* box1, const Collision* box2)
{
    const Collision::Box& box1data = box1->GetBoxData();
    const Collision::Box& box2data = box2->GetBoxData();

    return (!((box1data.maxBounds.x < box2data.minBounds.x  || 
               box1data.minBounds.x > box2data.maxBounds.x) ||
              (box1data.maxBounds.y < box2data.minBounds.y  || 
               box1data.minBounds.y > box2data.maxBounds.y) ||
              (box1data.maxBounds.z < box2data.minBounds.z  ||
               box1data.minBounds.z > box2data.maxBounds.z)));
}

bool CollisionTester::TestBoxSphereCollision(const Collision* box, const Collision* sphere)
{
    const Collision::Box& boxData = box->GetBoxData();
    const Collision::Sphere& sphereData = sphere->GetSphereData();

    //get normalized vector from sphere center to box center
    //times by radius to get vector from center sphere to sphere pointing at cube
    D3DXVECTOR3 sphereToBox = box->GetPosition() - sphere->GetPosition();
    D3DXVec3Normalize(&sphereToBox,&sphereToBox);
    D3DXVECTOR3 sphereDir = sphereData.radius * sphereToBox; //center of sphere pointing at cube

    //convert vector to world space
    sphereDir += sphere->GetPosition();

    //check for collision
    if((sphereDir.x < boxData.maxBounds.x) && 
        (sphereDir.x > boxData.minBounds.x)) //test if in x portion
    {
        if((sphereDir.y < boxData.maxBounds.y) && 
            (sphereDir.y > boxData.minBounds.y)) //test if in y portion
        {
            return ((sphereDir.z < boxData.maxBounds.z) &&
                (sphereDir.z > boxData.minBounds.z)); //test if in z portion
        }
    }
    return false;
}

void Collision::DrawWithRadius(const Transform& projection, const Transform& view, float radius)
{
    Sphere& spheredata = GetSphereData();
    m_world.Matrix._11 = m_world.Matrix._22 = m_world.Matrix._33 = radius;
    Draw(projection, view);
    m_world.Matrix._11 = m_world.Matrix._22 = m_world.Matrix._33 = spheredata.radius;
}