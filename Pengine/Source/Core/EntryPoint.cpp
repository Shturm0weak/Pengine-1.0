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
#include "ThreadPool.h"
#include "Editor.h"
#include "../EventSystem/EventSystem.h"
#include "../OpenGL/FrameBuffer.h"
#include "../OpenGL/Batch.h"
#include "Serializer.h"
#include "Animation2DManager.h"
#include "Visualizer.h"
#include "../UI/Gui.h"

using namespace Pengine;

void EntryPoint::PrepareResources()
{
    std::vector<float> vertices =
    {
        -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 1.0f
    };

    std::vector<uint32_t> indices =
    {
        0, 1, 2 , 2, 3, 0
    };

    const std::vector<uint32_t> layouts = { 2, 2 };

    MeshManager::GetInstance().Create("Quad", vertices, indices, layouts);

    TextureManager::GetInstance().ResetTexParametersi();
    TextureManager::GetInstance().ColoredTexture("White", 0xFFFFFFFF);

    Viewport::GetInstance().Initialize();
    ThreadPool::GetInstance().Initialize();

    Utils::LoadShadersFromFolder("Source/Shaders");
    Utils::LoadTexturesFromFolder("Source/Images");
    Utils::LoadTexturesFromFolder("Source/Images/Icons");
    Utils::LoadTexturesFromFolder("Source/UIimages");

    Environment::GetInstance().SetEditorCamera(std::make_shared<Camera>());
    Environment::GetInstance().UseEditorCameraAsMain();
}

void EntryPoint::OnStart()
{
    assert(Window::GetInstance().Initialize() == 0);

    PrepareResources();
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
            Environment::GetInstance().GetMainCamera()->Movement();
            EventSystem::GetInstance().ProcessEvents();
            if (m_Application->GetState() == Application::ApplicationState::Play)
            {
                m_Application->UpdatePhysics();
                m_Application->OnUpdate();
            }

            Viewport::GetInstance().Begin();
            {
                Batch::GetInstance().BeginGameObjects();
                {
                    m_Application->Render();
                    Visualizer::RenderQuads();
                }
                Batch::GetInstance().EndGameObjects();

                Visualizer::RenderLines();

                Gui::GetInstance().Begin();
                {
                    m_Application->OnGuiRender();
                }
                Gui::GetInstance().End();
            }
            Viewport::GetInstance().End();

            Editor::GetInstance().Update(m_Application->GetScene());
        }
        Window::GetInstance().EndFrame();
    }
}

void EntryPoint::OnClose()
{
    m_Application->ShutDown();
    Animation2DManager::GetInstance().ShutDown();
    MeshManager::GetInstance().ShutDown();
    MemoryManager::GetInstance().ShutDown();
    ThreadPool::GetInstance().Shutdown();
    Logger::Success("Application has been closed!");
}
