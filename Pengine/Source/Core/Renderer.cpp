#include "Renderer.h"

#include "Window.h"
#include "Viewport.h"
#include "Logger.h"
#include "MeshManager.h"
#include "Editor.h"
#include "Environment.h"
#include "Timer.h"
#include "Visualizer.h"
#include "Application.h"
#include "../UI/Gui.h"
#include "../OpenGL/Batch.h"
#include "../OpenGL/Shader.h"
#include "../OpenGL/FrameBuffer.h"

using namespace Pengine;

Renderer& Renderer::GetInstance()
{
	static Renderer renderer;
	return renderer;
}

void Renderer::Initialize()
{
    FrameBuffer::FrameBufferParams params = { Window::GetInstance().GetSize(), 1, GL_COLOR_ATTACHMENT0, GL_RGBA32F, GL_RGBA,
        GL_FLOAT, true, true, true };

    TextureManager::GetInstance().m_TexParameters[0] = { GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR };
    TextureManager::GetInstance().m_TexParameters[1] = { GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR };

    m_FrameBufferScene = new FrameBuffer(params, TextureManager::GetInstance().GetTexParamertersi());
    m_FrameBufferUI = new FrameBuffer(params, TextureManager::GetInstance().GetTexParamertersi());
    m_FrameBufferBloom = new FrameBuffer(params, TextureManager::GetInstance().GetTexParamertersi());
    
    for (size_t i = 0; i < 12; i++)
    {
        m_FrameBufferBlur.push_back(new FrameBuffer(params, TextureManager::GetInstance().GetTexParamertersi()));
    }

    TextureManager::GetInstance().ResetTexParametersi();

    Logger::Success("Viewport has been initialized!");
}

void Renderer::BeginScene()
{
    m_FrameBufferScene->Bind();
    Viewport& viewport = Viewport::GetInstance();
    viewport.m_PreviousWindowSize = Window::GetInstance().GetSize();
    glViewport(0, 0, viewport.m_Size.x, viewport.m_Size.y);
    Window::GetInstance().Clear();
}

void Renderer::EndScene()
{
    Viewport& viewport = Viewport::GetInstance();
    glViewport(0, 0, viewport.m_PreviousWindowSize.x, viewport.m_PreviousWindowSize.y);
    m_FrameBufferScene->UnBind();
}

void Renderer::BeginUI()
{
    m_FrameBufferUI->Bind();
    Viewport& viewport = Viewport::GetInstance();
    viewport.m_PreviousWindowSize = Window::GetInstance().GetSize();
    glViewport(0, 0, viewport.m_Size.x, viewport.m_Size.y);
    Window::GetInstance().Clear({ 0.0f, 0.0f, 0.0f, 0.0f });
}

void Renderer::EndUI()
{
    Viewport& viewport = Viewport::GetInstance();
    glViewport(0, 0, viewport.m_PreviousWindowSize.x, viewport.m_PreviousWindowSize.y);
    m_FrameBufferUI->UnBind();
}

void Renderer::ComposeFinalImage()
{
    Shader* shader = Shader::Get("ComposeFinalImage");
    shader->Bind();
    
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, m_FrameBufferScene->m_Textures[0]);
    shader->SetUniform1i("u_SceneTexture", 0);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, m_FrameBufferUI->m_Textures[0]);
    shader->SetUniform1i("u_UITexture", 1);

    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, m_FrameBufferBloom->m_Textures[0]);
    shader->SetUniform1i("u_BloomTexture", 2);

    shader->SetUniform1f("u_Gamma", Environment::GetInstance().m_BloomSettings.m_Gamma);
    shader->SetUniform1i("u_IsBloomEnabled", Environment::GetInstance().m_BloomSettings.m_IsEnabled);

    RenderFullScreenQuad();

    shader->UnBind();

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, TextureManager::GetInstance().Get("White")->GetRendererID());
}

void Renderer::RenderFullScreenQuad()
{
    Mesh* quad = MeshManager::GetInstance().Get("Quad");

    quad->m_Va.Bind();
    quad->m_Ib.Bind();

    Editor::GetInstance().m_Stats.m_Vertices += (float)quad->GetVertexAttributes().size() / (float)quad->m_Layout.GetElements().size();
    Editor::GetInstance().m_Stats.m_Indices += quad->m_Ib.GetCount();
    Editor::GetInstance().m_Stats.m_DrawCalls++;

    glDrawElements(GL_TRIANGLES, quad->m_Ib.GetCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::Bloom()
{
    Shader* shader = Shader::Get("Blur");
    Viewport& viewport = Viewport::GetInstance();
    viewport.m_PreviousWindowSize = Window::GetInstance().GetSize();
    glm::vec2 size = viewport.GetSize();
    //size *= 2.0f;

    bool firstIterationOfFirstFrameBuffer = true;

    for (size_t j = 0; j < m_FrameBufferBlur.size() / 2; j++)
    {
        size *= 0.5f;

        uint32_t texture = 0;

        if (j == 0) texture = m_FrameBufferScene->m_Textures[0];
        else texture = m_FrameBufferBlur[j - 1]->m_Textures[0];

        bool horizontal = true, firstIteration = true;

        for (size_t i = 0; i < Environment::GetInstance().m_BloomSettings.m_BlurPasses; i++)
        {
            uint32_t index = (int)horizontal + (j * 2);
          
            m_FrameBufferBlur[index]->Bind();
            glViewport(0, 0, size.x, size.y);
            Window::GetInstance().Clear();
            
            shader->Bind();
            shader->SetUniform1i("u_Pixels", Environment::GetInstance().m_BloomSettings.m_PixelsBlured);
            shader->SetUniform1i("u_Horizontal", horizontal);
            shader->SetUniform1i("u_DownSampling", Environment::GetInstance().m_BloomSettings.m_DownSampling);
            shader->SetUniform1i("u_FirstFrameBufferIteration", firstIterationOfFirstFrameBuffer);
            shader->SetUniform1f("u_Brightness", Environment::GetInstance().m_BloomSettings.m_BrightnessThreshold);

            int id = firstIteration ? texture : m_FrameBufferBlur[(int)!horizontal + (j * 2)]->m_Textures[0];
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, id);
            shader->SetUniform1i("u_Image", 0);
            
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, m_FrameBufferScene->m_Textures[0]);
            shader->SetUniform1i("u_SourceImage", 1);

            RenderFullScreenQuad();

            horizontal = !horizontal;
            if (firstIteration) firstIteration = false;
            if (firstIterationOfFirstFrameBuffer) firstIterationOfFirstFrameBuffer = false;

            shader->UnBind();

            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, TextureManager::GetInstance().Get("White")->GetRendererID());
            m_FrameBufferBlur[index]->UnBind();
        }
    }

    glViewport(0, 0, viewport.m_PreviousWindowSize.x, viewport.m_PreviousWindowSize.y);  
    Window::GetInstance().Clear();

    int samplers[32];
    for (size_t i = 0; i < m_FrameBufferBlur.size() / 2; i++)
    {
        uint32_t index = (i * 2) + 1;
        glActiveTexture(GL_TEXTURE0 + i + 1);
        glBindTexture(GL_TEXTURE_2D, m_FrameBufferBlur[index]->m_Textures[0]);
    }
    
    for (unsigned int i = 0; i < 32; i++)
    {
        samplers[i] = i;
    }
    
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, m_FrameBufferScene->m_Textures[0]);

    shader = Shader::Get("AddBlurImages");
    shader->Bind();
    
    shader->SetUniform1iv("u_BlurTextures", samplers);
    shader->SetUniform1i("u_BlurTexturesSize", m_FrameBufferBlur.size() / 2 + 1);
    shader->SetUniform1f("u_Exposure", Environment::GetInstance().m_BloomSettings.m_Exposure);
    shader->SetUniform1f("u_Brightness", Environment::GetInstance().m_BloomSettings.m_BrightnessThreshold);
    shader->SetUniform1i("u_UpSampling", Environment::GetInstance().m_BloomSettings.m_UpSampling);
    shader->SetUniform1f("u_UpSamplingScale", Environment::GetInstance().m_BloomSettings.m_UpScalingScale);

    m_FrameBufferBloom->Bind();
    viewport.m_PreviousWindowSize = Window::GetInstance().GetSize();
    glViewport(0, 0, viewport.m_Size.x, viewport.m_Size.y);
    Window::GetInstance().Clear();

    RenderFullScreenQuad();
    
    m_FrameBufferBloom->UnBind();
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, TextureManager::GetInstance().Get("White")->GetRendererID());
    glViewport(0, 0, viewport.m_PreviousWindowSize.x, viewport.m_PreviousWindowSize.y);
}

void Renderer::Render(class Application* application)
{
    Timer timer = Timer(false, &Editor::GetInstance().m_Stats.m_RenderTime);

    BeginScene();
    {
        Timer timer = Timer(false, &Editor::GetInstance().m_Stats.m_RenderSceneTime);
        Batch::GetInstance().BeginGameObjects();
        {
            application->m_Scene->Render();
            Visualizer::RenderQuads();
            Visualizer::RenderCircles();
        }
        Batch::GetInstance().EndGameObjects();
    }
    EndScene();

    BeginUI();
    {
        {
            Timer timer = Timer(false, &Editor::GetInstance().m_Stats.m_RenderLinesTime);
            Visualizer::RenderLines();
        }

        Timer timer = Timer(false, &Editor::GetInstance().m_Stats.m_RenderUITime);
        //Gui::GetInstance().Begin(); It is called in Window::NewFrame for allowing OnGuiRender calls from components.
        {
            application->OnGuiRender();
        }
        Gui::GetInstance().End();
    }
    EndUI();

    {
        Timer timer = Timer(false, &Editor::GetInstance().m_Stats.m_RenderBloomTime);
        if (Environment::GetInstance().m_BloomSettings.m_IsEnabled)
        {
            Bloom();
        }
    }

    Viewport::GetInstance().Begin();
    {
        Timer timer = Timer(false, &Editor::GetInstance().m_Stats.m_RenderComposeTime);
        ComposeFinalImage();
    }
    Viewport::GetInstance().End();
}

void Renderer::ShutDown()
{
    delete m_FrameBufferScene;
    delete m_FrameBufferUI;
    delete m_FrameBufferBloom;

    for (size_t i = 0; i < m_FrameBufferBlur.size(); i++)
    {
        delete m_FrameBufferBlur[i];
    }
}
