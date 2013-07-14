#include "simulation.h"
#include "mesh.h"
#include "cloth.h"
#include "camera.h"
#include "sprite.h"
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
    ///< All meshes in the scene
    enum SceneMesh
    { 
        GROUND_MESH, 
        BALL_MESH, 
        MAX_MESH 
    };

    ///< All text in the scene
    enum SceneText
    { 
        STAT_TXT, 
        MAX_TEXT 
    };

    ///< All sprites in the scene
    enum SceneSprite
    { 
        GRAVITY, GRAB, BOX, CAMRESET, CLOTHRESET, 
        PLUSTIME, MINUSTIME, PLUSDAMP, MINUSDAMP, 
        PLUSVERT, MINUSVERT, PLUSSIZE, MINUSSIZE, 
        SHOWVERTS, PLUSIT, MINUSIT, HANDLEMODE,
        KEYS, KEYLIST, COLLIDE, MOVE, MAX_SPRITE
    };

    const float CAMERA_MOVE_SPEED = 40.0f;  
    const float CAMERA_ROT_SPEED = 2.0f;    
    const float HANDLE_SPEED = 10.0f;

    const D3DCOLOR BACK_BUFFER_COLOR(D3DCOLOR_XRGB(190, 190, 195)); ///< CLear colour of the back buffer
    const D3DCOLOR RENDER_COLOR(D3DCOLOR_XRGB(0, 0, 255));          ///< Render event colour
    const D3DCOLOR UPDATE_COLOR(D3DCOLOR_XRGB(0, 255, 0));          ///< Update event colour

    const std::string ModelsFolder(".\\Resources\\Models\\");       ///< Folder for all meshes
    const std::string SpritesFolder(".\\Resources\\Sprites\\");     ///< Folder for all sprites
}

bool Simulation::sm_drawCollisions = false;

Simulation::Simulation() :
    m_clothSize(0.5),
    m_clothDimensions(30),
    m_handleMode(false),
    m_d3ddev(nullptr),
    m_ballPosition(0.0f),
    m_ballMoveAmount(0.2f),
    m_moveBall(false)
{
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

    //Draw all sprites
    Sprite::StartSpriteRender();
    auto drawSprite = [](SpritePtr& sprite) { sprite->OnDraw(); };
    std::for_each(std::begin(m_sprites), std::end(m_sprites), drawSprite);
    Sprite::EndSpriteRender();
    
    //Draw text
    auto drawText = [](TextPtr& m_text) { m_text->Draw(); };
    std::for_each(std::begin(m_text), std::end(m_text), drawText);

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
        if(!OnClickSprite())
        {
            m_input->UpdatePicking(m_camera->Projection, m_camera->View); 

            m_cloth->MousePickingTest(m_input->GetMousePicking());

            auto meshPick = [&](MeshPtr& mesh) { mesh->MousePickingTest(m_input->GetMousePicking()); };
            std::for_each(std::begin(m_meshes), std::end(m_meshes), meshPick);

            m_input->SolvePicking();
        }
    }

    //To remove once GUI finished
    if(m_moveBall)
    {
        m_ballPosition += m_ballMoveAmount;
        m_meshes[BALL_MESH]->SetPosition(0,-2.0f,m_ballPosition);
        m_ballMoveAmount *= ((m_ballPosition >= 20.0f) || (m_ballPosition <= -20.0f)) ? -1.0f : 1.0f;
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

void Simulation::Release()
{
    Sprite::ReleaseSpriteObject();
}

bool Simulation::OnClickSprite()
{
    for(int i = 0; i < MAX_SPRITE; ++i)
    {
        if(m_sprites[i]->OnClick(m_input->GetMouseX(), m_input->GetMouseY()))
        {
            SolveSpriteClick(i);
            return true;
        }
    }
    return false;
}

void Simulation::SolveSpriteClick(int index)
{
    // TO fix once GUI done
    const float CLICK_INCREASE = 0.1f;     
    const float MIN_TIMESTEP = 0.01f;      
    const float MIN_DAMPING = 0.01f;       
    const float MIN_CLOTHSIZE = 0.11f;     
    const unsigned int MIN_ITERATIONS = 2; 
    const unsigned int MIN_VERTS = 4;      

    switch(index)
    {
    case GRAVITY:
    {
        if(!m_handleMode)
        {
            m_sprites[GRAVITY]->ToggleVisibility();
            m_cloth->ToggleSimulation();
        }
        break;
    }
    case GRAB:
    {   
        m_cloth->UnpinCloth();
        break;
    }
    case BOX:
    {   
        m_meshes[BALL_MESH]->ToggleVisibility();
        m_sprites[BOX]->ToggleVisibility();
        break;
    }
    case CAMRESET:
    {   
        m_camera->Reset();
        break;
    }
    case CLOTHRESET:
    {
        m_sprites[GRAVITY]->SetVisibility(HALF_ALPHA);
        m_cloth->Reset();
        break;
    }
    case PLUSTIME:
    {
        m_cloth->SetTimeStep(m_cloth->GetTimeStep() + CLICK_INCREASE);
        UpdateText();
        break;
    }
    case MINUSTIME:
    {
        m_cloth->SetTimeStep(max(MIN_TIMESTEP, m_cloth->GetTimeStep()+CLICK_INCREASE));
        UpdateText();
        break;
    }
    case PLUSDAMP:
    {
        m_cloth->SetDamping(m_cloth->GetDamping() + CLICK_INCREASE);
        UpdateText();
        break;
    }
    case MINUSDAMP:
    {
        m_cloth->SetDamping(max(MIN_DAMPING, m_cloth->GetDamping()+CLICK_INCREASE));
        UpdateText();
        break;
    }
    case MINUSVERT: 
    {
        if(m_clothDimensions > MIN_VERTS)
        {
          --m_clothDimensions;
          CreateCloth();
          UpdateText();
        }
        break;
    }
    case PLUSVERT:
    {
        m_clothDimensions++;
        CreateCloth();
        UpdateText();
        break;
    }
    case PLUSSIZE:
    {
        m_clothSize += CLICK_INCREASE;
        CreateCloth();
        UpdateText();
        break;
    }
    case MINUSSIZE:
    {
        if(m_clothSize > MIN_CLOTHSIZE)
        {
            m_clothSize -= CLICK_INCREASE;
            CreateCloth();
            UpdateText();
        }
        break;
    }
    case MINUSIT:
    {
        int itr = m_cloth->GetSpringIterations();
        if(m_cloth->GetSpringIterations() > MIN_ITERATIONS)
        {
            m_cloth->SetSpringIterations(--itr);
            UpdateText();
        }   
        break;
    }
    case PLUSIT:
    {
        int itr = m_cloth->GetSpringIterations();
        m_cloth->SetSpringIterations(++itr);
        UpdateText();
        break;
    }
    case SHOWVERTS:
    {
        m_cloth->ToggleVisualParticles();
        m_sprites[SHOWVERTS]->ToggleVisibility();
        break;
    }
    case HANDLEMODE:
    {
        m_handleMode = !m_handleMode;
        m_sprites[HANDLEMODE]->SetVisibility(m_handleMode ? FULL_ALPHA : HALF_ALPHA);
        m_sprites[COLLIDE]->SetVisibility(HALF_ALPHA);
        m_sprites[GRAVITY]->SetVisibility(HALF_ALPHA);
        m_cloth->SetManipulate(m_handleMode);
        break;
    }
    case COLLIDE:
    {
        if(!m_handleMode)
        {
            m_sprites[COLLIDE]->SetVisibility(m_cloth->IsSelfColliding() ? HALF_ALPHA : FULL_ALPHA);
            m_cloth->SetSelfCollide(!m_cloth->IsSelfColliding());
        }
        break;
    }
    case KEYS:
    case KEYLIST:
    {
        m_sprites[KEYLIST]->ToggleVisibility(0);
        m_sprites[KEYS]->ToggleVisibility();
        break;
    }
    case MOVE:
    {
        m_moveBall = !m_moveBall;
        m_sprites[MOVE]->ToggleVisibility();
        break;
    } 
    }
}

void Simulation::UpdateText()
{
    std::stringstream ss;
    ss.precision(1);
    ss.setf(std::ios_base::floatfield, std::ios_base::fixed);
    ss.setf(std::ios_base::showpoint);
    ss << m_cloth->GetTimeStep() << " [TIME] \n";
    ss << m_cloth->GetVertexCount() << " [VERT] \n";
    ss << m_cloth->GetDamping() << " [DAMP] \n";
    ss << m_clothSize << " [SIZE] \n";
    ss << m_cloth->GetSpringIterations() << " [ITRN] \n";
    m_text[STAT_TXT]->SetText(ss.str());
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
    success = (success ? LoadSprites() : false);
    if(success)
    {
        auto boundsShader = Shader_Manager::GetShader(Shader_Manager::BOUNDS_SHADER);
        Collision::Initialise(boundsShader);
        Diagnostic::Initialise(d3ddev, boundsShader);
        CreateCloth();
    }
    success = (success ? LoadText() : false);

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
    m_sprites[SHOWVERTS]->SetVisibility(FULL_ALPHA);
    m_sprites[GRAVITY]->SetVisibility(HALF_ALPHA);
    m_sprites[HANDLEMODE]->SetVisibility(HALF_ALPHA);
    m_sprites[COLLIDE]->SetVisibility(HALF_ALPHA);
}

bool Simulation::LoadMeshes()
{
    bool success = true;
    m_meshes.resize(MAX_MESH);
    std::generate(m_meshes.begin(), m_meshes.end(), [&](){ return Simulation::MeshPtr(new Mesh()); });

    auto meshShader = Shader_Manager::GetShader(Shader_Manager::MAIN_SHADER);
    success = (success ? m_meshes[BALL_MESH]->Load(m_d3ddev,ModelsFolder+"ball.obj",meshShader) : false);
    success = (success ? m_meshes[GROUND_MESH]->Load(m_d3ddev,ModelsFolder+"ground.obj",meshShader) : false);

    if(success)
    {
        m_meshes[BALL_MESH]->SetScale(0.4f);
        m_meshes[BALL_MESH]->SetPosition(0,-2,0);
        m_meshes[BALL_MESH]->CreateCollision(m_d3ddev,2.5f,12);

        m_meshes[GROUND_MESH]->SetPosition(0,-20,0);
        m_meshes[GROUND_MESH]->CreateCollision(m_d3ddev,150.0f,1.0f,150.0f);
    }
    return success;
}

bool Simulation::LoadSprites()
{
    Sprite::CreateSpriteObject(m_d3ddev);
    m_sprites.resize(MAX_SPRITE);
    std::generate(m_sprites.begin(), m_sprites.end(), [&](){ return Simulation::SpritePtr(new Sprite()); });
    int xposLeft = WINDOW_WIDTH-200;
    int xposRight = WINDOW_WIDTH-10;

    float i = 29.0f; 
    float h = 15.0f;
    m_sprites[PLUSTIME]->Load(m_d3ddev,SpritesFolder+"plus.png",FULL_ALPHA,16,16,float(xposRight)-33,(i),16,16);
    m_sprites[MINUSTIME]->Load(m_d3ddev,SpritesFolder+"minus.png",FULL_ALPHA,16,16,float(xposRight)-16,(i),16,16);
    m_sprites[PLUSDAMP]->Load(m_d3ddev,SpritesFolder+"plus.png",FULL_ALPHA,16,16,float(xposRight)-33,(i+h),16,16);
    m_sprites[MINUSDAMP]->Load(m_d3ddev,SpritesFolder+"minus.png",FULL_ALPHA,16,16,float(xposRight)-16,(i+h),16,16);
    m_sprites[PLUSVERT]->Load(m_d3ddev,SpritesFolder+"plus.png",FULL_ALPHA,16,16,float(xposRight)-33,(i+h+h),16,16);
    m_sprites[MINUSVERT]->Load(m_d3ddev,SpritesFolder+"minus.png",FULL_ALPHA,16,16,float(xposRight)-16,(i+h+h),16,16);
    m_sprites[PLUSSIZE]->Load(m_d3ddev,SpritesFolder+"plus.png",FULL_ALPHA,16,16,float(xposRight)-33,(i+h+h+h),16,16);
    m_sprites[MINUSSIZE]->Load(m_d3ddev,SpritesFolder+"minus.png",FULL_ALPHA,16,16,float(xposRight)-16,(i+h+h+h),16,16);
    m_sprites[PLUSIT]->Load(m_d3ddev,SpritesFolder+"plus.png",FULL_ALPHA,16,16,float(xposRight)-33,(i+h+h+h+h),16,16);
    m_sprites[MINUSIT]->Load(m_d3ddev,SpritesFolder+"minus.png",FULL_ALPHA,16,16,float(xposRight)-16,(i+h+h+h+h),16,16);

    float Y = 20;
    float X = 20;
    m_sprites[GRAVITY]->Load(m_d3ddev,SpritesFolder+"gravity.png",HALF_ALPHA,32,32,X,Y,32,32);
    m_sprites[SHOWVERTS]->Load(m_d3ddev,SpritesFolder+"showVerts.png",FULL_ALPHA,32,32,X,(Y+(32*1)-1),32,32);
    m_sprites[BOX]->Load(m_d3ddev,SpritesFolder+"box.png",FULL_ALPHA,32,32,X,(Y+(32*2)-2),32,32);
    m_sprites[MOVE]->Load(m_d3ddev,SpritesFolder+"move.png",HALF_ALPHA,32,32,X,(Y+(32*3)-3),32,32);
    m_sprites[CAMRESET]->Load(m_d3ddev,SpritesFolder+"resetCam.png",FULL_ALPHA,32,32,X,(Y+(32*4)-4),32,32);
    m_sprites[CLOTHRESET]->Load(m_d3ddev,SpritesFolder+"resetCloth.png",FULL_ALPHA,32,32,X,(Y+(32*5)-5),32,32);
    m_sprites[GRAB]->Load(m_d3ddev,SpritesFolder+"grab.png",FULL_ALPHA,32,32,X,(Y+(32*6)-6),32,32);
    m_sprites[HANDLEMODE]->Load(m_d3ddev,SpritesFolder+"handle.png",HALF_ALPHA,32,32,X,(Y+(32*7)-7),32,32);
    m_sprites[COLLIDE]->Load(m_d3ddev,SpritesFolder+"collide.png",HALF_ALPHA,32,32,X,(Y+(32*8)-8),32,32);
    m_sprites[KEYS]->Load(m_d3ddev,SpritesFolder+"keys.png",HALF_ALPHA,32,32,X,(Y+(32*9)-9),32,32);

    float xl = float(WINDOW_WIDTH-266);
    float yl = float(WINDOW_HEIGHT-266);
    m_sprites[KEYLIST]->Load(m_d3ddev,SpritesFolder+"keylist.png",0,256,256,xl,yl,256,256);

    return true;
}

bool Simulation::LoadText()
{
    m_text.push_back(Simulation::TextPtr(new Text()));
    m_text[STAT_TXT]->Load(m_d3ddev, false, 400, 15, DT_RIGHT, WINDOW_WIDTH-200, 30, WINDOW_WIDTH-46, 60);
    UpdateText();
    return true;
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
    m_input->SetKeyCallback(DIK_G, false, 
        [&](){ if(!m_handleMode){ m_cloth->ToggleSimulation(); }});

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