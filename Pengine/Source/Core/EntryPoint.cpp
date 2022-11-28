#include "EntryPoint.h"

#include "Window.h"
#include "Input.h"
#include "Logger.h"
#include "Utils.h"
#include "Environment.h"
#include "MeshManager.h"
#include "MemoryManager.h"
#include "TextureManager.h"
#include "Viewport.h"
#include "Renderer.h"
#include "ThreadPool.h"
#include "Editor.h"
#include "Timer.h"
#include "../Audio/SoundManager.h"
#include "../EventSystem/EventSystem.h"
#include "../OpenGL/FrameBuffer.h"
#include "../OpenGL/Batch.h"
#include "Serializer.h"
#include "Animation2DManager.h"
#include "Visualizer.h"
#include "../UI/Gui.h"
#include "../Lua/LuaStateManager.h"

using namespace Pengine;

void EntryPoint::PrepareResources()
{
    ThreadPool::GetInstance().Initialize();

    std::vector<float> vertices =
    {
        -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 1.0f
    };

    std::vector<uint32_t> indices =
    {
        0, 1, 2, 2, 3, 0
    };

    const std::vector<uint32_t> layouts = { 2, 2 };

    MeshManager::GetInstance().Create("Quad", vertices, indices, layouts, "Quad");

    TextureManager::GetInstance().ResetTexParametersi();
    TextureManager::GetInstance().m_White = TextureManager::GetInstance().ColoredTexture("White", 0xFFFFFFFF);
    
    Renderer::GetInstance().Initialize();
    Viewport::GetInstance().Initialize();

    Utils::LoadShadersFromFolder("Source/Shaders");
    Utils::LoadTexturesFromFolder("Source/Images");
    Utils::LoadTexturesFromFolder("Source/UIimages/EditorIcons");
    Utils::LoadTexturesFromFolder("Source/UIimages");

    Environment::GetInstance().SetEditorCamera(std::make_shared<Camera>());
    Environment::GetInstance().UseEditorCameraAsMain();
}

void EntryPoint::OnStartState()
{
    if (m_StartState)
    {
        m_StartState = false;
        EventSystem::GetInstance().SendEvent(new IEvent(EventType::ONSTART));
        m_Application->OnPlay();
    }
}

void EntryPoint::SetApplication(Application* application, bool startState)
{
    assert(application);

    m_Application = application;
    m_StartState = startState;

    OnStart();

    OnUpdate();
}

void EntryPoint::OnStart()
{
    assert(Window::GetInstance().Initialize(m_Application->GetTitle()) == 0);

    PrepareResources();

#ifdef STANDALONE
    Serializer::DeserializeScene("Source/Sponza/Sponza.yaml");
#endif
}

EntryPoint::~EntryPoint()
{
    OnClose();
}

void EntryPoint::OnUpdate()
{
    while (Window::GetInstance().ShouldExit())
    {
        Window::GetInstance().NewFrame();
        {
#ifdef STANDALONE
            Editor::GetInstance().SetEnabled(false);
#else
            Environment::GetInstance().GetMainCamera()->Movement();
#endif
            EventSystem::GetInstance().ProcessEvents();
            Timer::UpdateCallbacks();

            if (m_Application->GetState() == Application::ApplicationState::Play)
            {
                m_Application->PostStartCall();
                m_Application->UpdatePhysics();
                m_Application->OnUpdate();
                m_Application->OnLuaUpdate();
            }

            SoundManager::GetInstance().UpdateSourceState();

            Renderer::GetInstance().Render(m_Application);

            Editor::GetInstance().Update(m_Application->GetScene());
        }

        Window::GetInstance().EndFrame();

        OnStartState();
    }
}

void EntryPoint::OnClose()
{
    m_Application->ShutDown();
    Animation2DManager::GetInstance().ShutDown();
    Viewport::GetInstance().ShutDown();
    MeshManager::GetInstance().ShutDown();
    MemoryManager::GetInstance().ShutDown();
    ThreadPool::GetInstance().Shutdown();
    MaterialManager::GetInstance().ShutDown();
    Logger::Success("Application has been closed!");
}
