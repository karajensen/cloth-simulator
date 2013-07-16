#include "simulation.h"
#include "mesh.h"
#include "cloth.h"
#include "camera.h"
#include "light.h"
#include "input.h"
#include "shader.h"
#include "collision.h"
#include "timer.h"
#include "text.h"
#include <algorithm>
#include <sstream>

namespace
{
    const int GROUND_MESH = 0;
    const float CAMERA_MOVE_SPEED = 40.0f;  
    const float CAMERA_ROT_SPEED = 2.0f;    
    const float HANDLE_SPEED = 10.0f;

    const D3DCOLOR BACK_BUFFER_COLOR(D3DCOLOR_XRGB(190, 190, 195)); ///< CLear colour of the back buffer
    const D3DCOLOR RENDER_COLOR(D3DCOLOR_XRGB(0, 0, 255));          ///< Render event colour
    const D3DCOLOR UPDATE_COLOR(D3DCOLOR_XRGB(0, 255, 0));          ///< Update event colour
    const std::string ModelsFolder(".\\Resources\\Models\\");       ///< Folder for all meshes
}

bool Simulation::sm_drawCollisions = false;

Simulation::Simulation() :
    m_clothSize(0.5),
    m_clothDimensions(30),
    m_handleMode(false),
    m_d3ddev(nullptr)
{
}

Simulation::~Simulation()
{
    #ifdef _DEBUG
    OutputDebugString("RELEASING SIMULATION\n");
    #endif
}

void Simulation::Render()
{
    #ifdef _DEBUG
    D3DPERF_BeginEvent(RENDER_COLOR, L"Simulation::Render");
    #endif

    m_d3ddev->BeginScene();
    m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, BACK_BUFFER_COLOR, 1.0f, 0);
    m_d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

    D3DXVECTOR3 camPos(m_camera->World.Position());

    //Draw all meshes
    auto drawMesh = [&](MeshPtr& mesh) { mesh->DrawMesh(camPos, m_camera->Projection, m_camera->View); };
    std::for_each(std::begin(m_meshes), std::end(m_meshes), drawMesh);

    //Draw cloth
    m_cloth->DrawMesh(camPos, m_camera->Projection, m_camera->View);
    m_cloth->DrawCollision(m_camera->Projection, m_camera->View);

    //Draw all collision models
    auto drawCol = [&](MeshPtr& mesh) { mesh->DrawCollision(m_camera->Projection, m_camera->View); };
    std::for_each(std::begin(m_meshes), std::end(m_meshes), drawCol);

    //Draw all 3D Diagnostics
    Diagnostic::Get().DrawAll3D(m_camera->Projection, m_camera->View);

    //Draw all 2D Diagnostics
    Diagnostic::Get().DrawAll2D();

    m_d3ddev->EndScene();
    m_d3ddev->Present(NULL, NULL, NULL, NULL);
}

void Simulation::Update()
{
    #ifdef _DEBUG
    D3DPERF_BeginEvent(UPDATE_COLOR, L"Simulation::Update");
    #endif

    double deltaTime = m_timer->UpdateTimer();

    m_input->UpdateInput();
    m_camera->UpdateCamera();

    if (m_input->IsMouseClicked())
    {
        m_input->UpdatePicking(m_camera->Projection, m_camera->View); 

        m_cloth->MousePickingTest(m_input->GetMousePicking());

        auto meshPick = [&](MeshPtr& mesh) { mesh->MousePickingTest(m_input->GetMousePicking()); };
        std::for_each(std::begin(m_meshes), std::end(m_meshes), meshPick);

        m_input->SolvePicking();
    }

    m_cloth->UpdateState(deltaTime);

    std::for_each(std::begin(m_meshes), std::end(m_meshes), 
        [&](MeshPtr& mesh){ m_cloth->SolveCollision(mesh->GetCollision()); });

    m_cloth->UpdateVertexBuffer();
}

void Simulation::SetMouseCoord(int x, int y)
{
    m_input->SetMouseCoord(x, y);
}

void Simulation::LoadGuiCallbacks(GUI::GuiCallback* callback)
{
    // TO fix once GUI done
    const float CLICK_INCREASE = 0.1f;     
    const float MIN_TIMESTEP = 0.01f;      
    const float MIN_DAMPING = 0.01f;       
    const float MIN_CLOTHSIZE = 0.11f;     
    const unsigned int MIN_ITERATIONS = 2; 
    const unsigned int MIN_VERTS = 4;      

    //switch(index)
    //{
    //case GRAVITY:
    //{
    //    if(!m_handleMode)
    //    {
    //        m_sprites[GRAVITY]->ToggleVisibility();
    //        m_cloth->ToggleSimulation();
    //    }
    //    break;
    //}
    //case GRAB:
    //{   
    //    m_cloth->UnpinCloth();
    //    break;
    //}
    //case BOX:
    //{   
    //    m_meshes[BALL_MESH]->ToggleVisibility();
    //    m_sprites[BOX]->ToggleVisibility();
    //    break;
    //}
    //case CAMRESET:
    //{   
    //    m_camera->Reset();
    //    break;
    //}
    //case CLOTHRESET:
    //{
    //    m_sprites[GRAVITY]->SetVisibility(HALF_ALPHA);
    //    m_cloth->Reset();
    //    break;
    //}
    //case PLUSTIME:
    //{
    //    m_cloth->SetTimeStep(m_cloth->GetTimeStep() + CLICK_INCREASE);
    //    UpdateText();
    //    break;
    //}
    //case MINUSTIME:
    //{
    //    m_cloth->SetTimeStep(max(MIN_TIMESTEP, m_cloth->GetTimeStep()+CLICK_INCREASE));
    //    UpdateText();
    //    break;
    //}
    //case PLUSDAMP:
    //{
    //    m_cloth->SetDamping(m_cloth->GetDamping() + CLICK_INCREASE);
    //    UpdateText();
    //    break;
    //}
    //case MINUSDAMP:
    //{
    //    m_cloth->SetDamping(max(MIN_DAMPING, m_cloth->GetDamping()+CLICK_INCREASE));
    //    UpdateText();
    //    break;
    //}
    //case MINUSVERT: 
    //{
    //    if(m_clothDimensions > MIN_VERTS)
    //    {
    //      --m_clothDimensions;
    //      CreateCloth();
    //      UpdateText();
    //    }
    //    break;
    //}
    //case PLUSVERT:
    //{
    //    m_clothDimensions++;
    //    CreateCloth();
    //    UpdateText();
    //    break;
    //}
    //case PLUSSIZE:
    //{
    //    m_clothSize += CLICK_INCREASE;
    //    CreateCloth();
    //    UpdateText();
    //    break;
    //}
    //case MINUSSIZE:
    //{
    //    if(m_clothSize > MIN_CLOTHSIZE)
    //    {
    //        m_clothSize -= CLICK_INCREASE;
    //        CreateCloth();
    //        UpdateText();
    //    }
    //    break;
    //}
    //case MINUSIT:
    //{
    //    int itr = m_cloth->GetSpringIterations();
    //    if(m_cloth->GetSpringIterations() > MIN_ITERATIONS)
    //    {
    //        m_cloth->SetSpringIterations(--itr);
    //        UpdateText();
    //    }   
    //    break;
    //}
    //case PLUSIT:
    //{
    //    int itr = m_cloth->GetSpringIterations();
    //    m_cloth->SetSpringIterations(++itr);
    //    UpdateText();
    //    break;
    //}
    //case SHOWVERTS:
    //{
    //    m_cloth->ToggleVisualParticles();
    //    m_sprites[SHOWVERTS]->ToggleVisibility();
    //    break;
    //}
    //case HANDLEMODE:
    //{
    //    m_handleMode = !m_handleMode;
    //    m_sprites[HANDLEMODE]->SetVisibility(m_handleMode ? FULL_ALPHA : HALF_ALPHA);
    //    m_sprites[COLLIDE]->SetVisibility(HALF_ALPHA);
    //    m_sprites[GRAVITY]->SetVisibility(HALF_ALPHA);
    //    m_cloth->SetManipulate(m_handleMode);
    //    break;
    //}
    //case COLLIDE:
    //{
    //    if(!m_handleMode)
    //    {
    //        m_sprites[COLLIDE]->SetVisibility(m_cloth->IsSelfColliding() ? HALF_ALPHA : FULL_ALPHA);
    //        m_cloth->SetSelfCollide(!m_cloth->IsSelfColliding());
    //    }
    //    break;
    //}
    //case KEYS:
    //case KEYLIST:
    //{
    //    m_sprites[KEYLIST]->ToggleVisibility(0);
    //    m_sprites[KEYS]->ToggleVisibility();
    //    break;
    //}
    //case MOVE:
    //{
    //    m_moveBall = !m_moveBall;
    //    m_sprites[MOVE]->ToggleVisibility();
    //    break;
    //} 
    //}
}

bool Simulation::CreateSimulation(HINSTANCE hInstance, HWND hWnd, LPDIRECT3DDEVICE9 d3ddev) 
{   
    bool success = true;
    m_d3ddev = d3ddev;
    LoadInput(hInstance, hWnd);

    m_camera.reset(new Camera(D3DXVECTOR3(0.0f,0.0f,-30.0f), D3DXVECTOR3(0.0f,0.0f,0.0f)));
    m_camera->CreateProjMatrix();

    // Load all assets
    Light_Manager::Inititalise();
    success = Shader_Manager::Inititalise(d3ddev);
    success = (success ? LoadMeshes() : false);

    // Create the cloth
    if(success)
    {
        auto boundsShader = Shader_Manager::GetShader(Shader_Manager::BOUNDS_SHADER);
        Collision::Initialise(boundsShader);
        Diagnostic::Initialise(d3ddev, boundsShader);
        CreateCloth();
    }

    // Start the internal timer
    m_timer.reset(new Timer());
    m_timer->StartTimer();

    return success;
}

void Simulation::CreateCloth()
{
    auto clothShader = Shader_Manager::GetShader(Shader_Manager::CLOTH_SHADER);
    m_cloth.reset(new Cloth(m_d3ddev, ModelsFolder+"square.png", clothShader, m_clothDimensions, m_clothSize));
    m_handleMode = false;
}

bool Simulation::LoadMeshes()
{
    bool success = true;
    m_meshes.resize(1);
    std::generate(m_meshes.begin(), m_meshes.end(), [&](){ return Simulation::MeshPtr(new Mesh()); });

    auto meshShader = Shader_Manager::GetShader(Shader_Manager::MAIN_SHADER);
    success = (success ? m_meshes[GROUND_MESH]->Load(m_d3ddev,ModelsFolder+"ground.obj",meshShader) : false);

    if(success)
    {
        m_meshes[GROUND_MESH]->SetPosition(0,-20,0);
        m_meshes[GROUND_MESH]->CreateCollision(m_d3ddev,150.0f,1.0f,150.0f);
    }
    return success;
}

void Simulation::LoadInput(HINSTANCE hInstance, HWND hWnd)
{
    m_input.reset(new Input(hInstance, hWnd));

    // Camera forward movement
    auto cameraForward = [&]()
    {
        m_camera->ForwardMovement(m_input->GetMouseDirection(), 
            m_timer->GetDeltaTime()*CAMERA_MOVE_SPEED, 
            m_input->IsMousePressed());
    };
    m_input->SetKeyCallback(DIK_LSHIFT, true, cameraForward);
    m_input->AddClickPreventionKey(DIK_LSHIFT);

    // Camera side movement
    auto cameraSideways = [&]()
    {
        m_camera->SideMovement(m_input->GetMouseDirection(), 
            m_timer->GetDeltaTime()*CAMERA_MOVE_SPEED, 
            m_input->IsMousePressed());
    };
    m_input->SetKeyCallback(DIK_LCONTROL, true, cameraSideways);
    m_input->AddClickPreventionKey(DIK_LCONTROL);

    // Camera rotation
    auto cameraRotation = [&]()
    {
        m_camera->Rotation(m_input->GetMouseDirection(), 
            m_timer->GetDeltaTime()*CAMERA_ROT_SPEED, 
            m_input->IsMousePressed());
    };
    m_input->SetKeyCallback(DIK_LALT, true, cameraRotation);
    m_input->AddClickPreventionKey(DIK_LALT);

    // Controlling the cloth
    m_input->SetKeyCallback(DIK_W, true, 
        [&](){ if(m_handleMode){ m_cloth->MovePinnedRow(
        -m_timer->GetDeltaTime()*HANDLE_SPEED,0.0f,0.0f); }});

    m_input->SetKeyCallback(DIK_S, true, 
        [&](){ if(m_handleMode){ m_cloth->MovePinnedRow(
        m_timer->GetDeltaTime()*HANDLE_SPEED,0.0f,0.0f); }});

    m_input->SetKeyCallback(DIK_A, true, 
        [&](){ if(m_handleMode){ m_cloth->MovePinnedRow(
        0.0f,-m_timer->GetDeltaTime()*HANDLE_SPEED,0.0f); }});

    m_input->SetKeyCallback(DIK_D, true, 
        [&](){ if(m_handleMode){ m_cloth->MovePinnedRow(
        0.0f,m_timer->GetDeltaTime()*HANDLE_SPEED,0.0f); }});

    m_input->SetKeyCallback(DIK_Q, true, 
        [&](){ if(m_handleMode){ m_cloth->MovePinnedRow(
        0.0f,0.0f,-m_timer->GetDeltaTime()*HANDLE_SPEED); }});

    m_input->SetKeyCallback(DIK_E, true, 
        [&](){ if(m_handleMode){ m_cloth->MovePinnedRow(
        0.0f,0.0f,m_timer->GetDeltaTime()*HANDLE_SPEED); }});

    // Changing the cloth row selected
    m_input->SetKeyCallback(DIK_1, false, 
        [&](){ if(m_handleMode){ m_cloth->ChangeRow(1); } }); 

    m_input->SetKeyCallback(DIK_2, false, 
        [&](){ if(m_handleMode){ m_cloth->ChangeRow(2); } }); 

    m_input->SetKeyCallback(DIK_3, false, 
        [&](){ if(m_handleMode){ m_cloth->ChangeRow(3); } }); 

    m_input->SetKeyCallback(DIK_4, false, 
        [&](){ if(m_handleMode){ m_cloth->ChangeRow(4); } }); 

    // Toggling Diagnostic drawing
    m_input->SetKeyCallback(DIK_0, false, 
        [&](){ Diagnostic::ToggleDiagnostics(); });

    // Allow diagnostic selection of particles
    m_input->SetKeyCallback(DIK_RALT, true, 
        [&](){ m_cloth->SetDiagnosticSelect(true); },
        [&](){ m_cloth->SetDiagnosticSelect(false); });

    // Toggle mesh collision model diagnostics
    auto toggleCollisionDiagnostics = [&]()
    {
        sm_drawCollisions = !sm_drawCollisions;
        m_cloth->SetCollisionVisibility(sm_drawCollisions);
        std::for_each(std::begin(m_meshes), std::end(m_meshes), 
            [&](MeshPtr& mesh){ mesh->SetCollisionVisibility(sm_drawCollisions); });
    };
    m_input->SetKeyCallback(DIK_9, false, toggleCollisionDiagnostics);
}