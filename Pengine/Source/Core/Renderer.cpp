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
#include "Instancing.h"
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
    m_FrameBufferShadows = new FrameBuffer(params, TextureManager::GetInstance().GetTexParamertersi());
    m_FrameBufferUI = new FrameBuffer(params, TextureManager::GetInstance().GetTexParamertersi());
    m_FrameBufferBloom = new FrameBuffer(params, TextureManager::GetInstance().GetTexParamertersi());
    
    FrameBuffer::FrameBufferParams shadowsBlurParams = { Window::GetInstance().GetSize(), 1, GL_COLOR_ATTACHMENT0, GL_RGBA32F, GL_RGBA,
        GL_FLOAT, true, true, true };
    m_FrameBufferShadowsBlur.push_back(new FrameBuffer(shadowsBlurParams, TextureManager::GetInstance().GetTexParamertersi()));
    m_FrameBufferShadowsBlur.push_back(new FrameBuffer(shadowsBlurParams, TextureManager::GetInstance().GetTexParamertersi()));

    for (size_t i = 0; i < 12; i += 2)
    {
        params.m_Size /= 2;
        m_FrameBufferBlur.push_back(new FrameBuffer(params, TextureManager::GetInstance().GetTexParamertersi()));
        m_FrameBufferBlur.push_back(new FrameBuffer(params, TextureManager::GetInstance().GetTexParamertersi()));
    }

    FrameBuffer::FrameBufferParams depthParams = { Window::GetInstance().GetSize(), 1, GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, false, false, false };
    m_FrameBufferOutline = new FrameBuffer(depthParams, TextureManager::GetInstance().GetTexParamertersi());

    TextureManager::GetInstance().m_TexParameters[2] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER };
    TextureManager::GetInstance().m_TexParameters[3] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER };

    depthParams.m_Size = { 4096, 4096 };
    m_FrameBufferCSM.push_back(new FrameBuffer(depthParams, TextureManager::GetInstance().GetTexParamertersi()));
    depthParams.m_Size = { 2048, 2048 };
    m_FrameBufferCSM.push_back(new FrameBuffer(depthParams, TextureManager::GetInstance().GetTexParamertersi()));
    depthParams.m_Size = { 1024, 1024 };
    m_FrameBufferCSM.push_back(new FrameBuffer(depthParams, TextureManager::GetInstance().GetTexParamertersi()));
    //for (size_t i = 0; i < 3; i++)
    //{
    //}

    TextureManager::GetInstance().ResetTexParametersi();

    Logger::Success("Renderer has been initialized!");
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
    m_FrameBufferScene->UnBind();
    Viewport& viewport = Viewport::GetInstance();
    glViewport(0, 0, viewport.m_PreviousWindowSize.x, viewport.m_PreviousWindowSize.y);
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
    m_FrameBufferUI->UnBind();
    Viewport& viewport = Viewport::GetInstance();
    glViewport(0, 0, viewport.m_PreviousWindowSize.x, viewport.m_PreviousWindowSize.y);
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

    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, m_FrameBufferOutline->m_Textures[0]);
    shader->SetUniform1i("u_OutlineTexture", 3);

    shader->SetUniform1f("u_Gamma", Environment::GetInstance().m_BloomSettings.m_Gamma);
    shader->SetUniform1i("u_IsBloomEnabled", Environment::GetInstance().m_BloomSettings.m_IsEnabled);
    shader->SetUniform1i("u_OutlineThickness", Editor::GetInstance().m_OutlineParams.m_Thickness);
    shader->SetUniform3fv("u_OutlineColor", Editor::GetInstance().m_OutlineParams.m_Color);

    RenderFullScreenQuad();

    shader->UnBind();

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, TextureManager::GetInstance().White()->GetRendererID());
}

void Renderer::RenderCascadeShadowMaps(Scene* scene)
{
    const std::shared_ptr<Camera> camera = Environment::GetInstance().GetMainCamera();

    for (size_t i = 0; i < scene->m_DirectionalLights.size(); i++)
    {
        m_LightSpaceMatrices = GetLightSpaceMatrices(scene->m_DirectionalLights[i]);

        break;
    }

    Viewport& viewport = Viewport::GetInstance();

    Shader* shader = Shader::Get("Instancing3DDepth");
    shader->Bind();

    for (size_t i = 0; i < m_LightSpaceMatrices.size(); i++)
    {
        shader->SetUniformMat4f("u_ViewProjection", m_LightSpaceMatrices[i]);

        m_FrameBufferCSM[i]->Bind();
        Window::GetInstance().Clear();
        viewport.m_PreviousWindowSize = Window::GetInstance().GetSize();
        glViewport(0, 0, m_FrameBufferCSM[i]->m_Params.m_Size.x, m_FrameBufferCSM[i]->m_Params.m_Size.y);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_CLAMP);
        Instancing::GetInstance().RenderAllObjects(scene->m_InstancedObjects);
        glDisable(GL_DEPTH_CLAMP);
        glCullFace(GL_BACK);

        m_FrameBufferCSM[i]->UnBind();
    }

    shader->UnBind();
    glViewport(0, 0, viewport.m_PreviousWindowSize.x, viewport.m_PreviousWindowSize.y);
}

void Renderer::RenderCascadeShadowsToScene(class Scene* scene)
{
    Viewport& viewport = Viewport::GetInstance();
    Environment& environment = Environment::GetInstance();

    Shader* shader = Shader::Get("InstancingShadows");
    shader->Bind();

    shader->SetUniformMat4f("u_ViewProjection", environment.GetMainCamera()->GetViewProjectionMat4());
    shader->SetUniformMat4f("u_View", environment.GetMainCamera()->GetViewMat4());
    shader->SetUniformfv("u_CascadesDistance", environment.m_ShadowsSettings.m_CascadesDistance);
    shader->SetUniform1i("u_Pcf", environment.m_ShadowsSettings.m_Pcf);
    shader->SetUniform1f("u_Fog", environment.m_ShadowsSettings.m_Fog);
    shader->SetUniform1f("u_Bias", environment.m_ShadowsSettings.m_Bias);
    shader->SetUniform1f("u_FarPlane", environment.GetMainCamera()->GetZFar() * environment.m_ShadowsSettings.m_ZFarScale);
    shader->SetUniform1i("u_ShadowsVisualized", environment.m_ShadowsSettings.m_IsVisualized);
    shader->SetUniform1i("u_CascadesCount", (int)environment.m_ShadowsSettings.m_CascadesDistance.size());
    shader->SetUniform1f("u_Texels", (int)environment.m_ShadowsSettings.m_Texels);
    shader->SetUniformMat4fv("u_LightSpaceMatricies", Renderer::GetInstance().m_LightSpaceMatrices);
    shader->SetUniform3fv("u_CameraPosition", environment.GetMainCamera()->m_Transform.GetPosition());

    std::vector<int> csm(Renderer::GetInstance().m_FrameBufferCSM.size());
    for (size_t i = 0; i < csm.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i + 1);
        glBindTexture(GL_TEXTURE_2D, Renderer::GetInstance().m_FrameBufferCSM[i]->m_Textures[0]);
        csm[i] = i + 1;
        shader->SetUniform1iv("u_CSM", &csm[0], csm.size());
    }

    m_FrameBufferShadows->Bind();
    Window::GetInstance().Clear();
    viewport.m_PreviousWindowSize = Window::GetInstance().GetSize();
    glViewport(0, 0, m_FrameBufferShadows->m_Params.m_Size.x, m_FrameBufferShadows->m_Params.m_Size.y);

    Instancing::GetInstance().RenderAllObjects(scene->m_InstancedObjects);

    m_FrameBufferShadows->UnBind();

    shader->UnBind();
    glViewport(0, 0, viewport.m_PreviousWindowSize.x, viewport.m_PreviousWindowSize.y);
}

glm::mat4 Renderer::GetLightSpaceMatrix(DirectionalLight* light, const float nearPlane, const float farPlane)
{
    const std::shared_ptr<Camera> camera = Environment::GetInstance().GetMainCamera();
    glm::mat4 projection;
    float aspect = camera->GetAspect();
    if (camera->GetType() == Camera::CameraType::ORTHOGRAPHIC)
    {
        projection = glm::ortho(-aspect * camera->GetZoom(), aspect * camera->GetZoom(), -1.0f * camera->GetZoom(),
            1.0f * camera->GetZoom(), nearPlane, farPlane);
    }
    else if (camera->GetType() == Camera::CameraType::PERSPECTIVE)
    {
        projection = glm::perspective(camera->GetFov(), aspect, nearPlane, farPlane);
    }

    const glm::mat4 viewProjectionMat4 = glm::inverse(projection * camera->GetViewMat4());

    std::vector<glm::vec4> corners;
    for (unsigned int x = 0; x < 2; ++x)
    {   
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt =
                    viewProjectionMat4 * glm::vec4(
                        2.0f * x - 1.0f,
                        2.0f * y - 1.0f,
                        2.0f * z - 1.0f,
                        1.0f);
                corners.push_back(pt / pt.w);
            }
        }
    }

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();

    const auto lightView = glm::lookAt(center + light->GetDirection(), center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = (std::numeric_limits<float>::max)();
    float maxX = (std::numeric_limits<float>::min)();
    float minY = (std::numeric_limits<float>::max)();
    float maxY = (std::numeric_limits<float>::min)();
    float minZ = (std::numeric_limits<float>::max)();
    float maxZ = (std::numeric_limits<float>::min)();
    for (const auto& v : corners)
    {
        const auto trf = lightView * v;
        minX = (std::min)(minX, trf.x);
        maxX = (std::max)(maxX, trf.x);
        minY = (std::min)(minY, trf.y);
        maxY = (std::max)(maxY, trf.y);
        minZ = (std::min)(minZ, trf.z);
        maxZ = (std::max)(maxZ, trf.z);
    }

    // Tune this parameter according to the scene
    constexpr float zMult = 10.0f;
    if (minZ < 0)
    {
        minZ *= zMult;
    }
    else
    {
        minZ /= zMult;
    }
    if (maxZ < 0)
    {
        maxZ /= zMult;
    }
    else
    {
        maxZ *= zMult;
    }

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

    return lightProjection * lightView;
}

std::vector<glm::mat4> Renderer::GetLightSpaceMatrices(DirectionalLight* light)
{
    Environment& environment = Environment::GetInstance();
    const std::shared_ptr<Camera> camera = environment.GetMainCamera();
    
    std::vector<glm::mat4> matrcies;

    for (size_t i = 0; i < environment.m_ShadowsSettings.m_CascadesDistance.size() + 1; ++i)
    {
        if (i == 0)
        {
            matrcies.push_back(GetLightSpaceMatrix(light, camera->GetZNear(), environment.m_ShadowsSettings.m_CascadesDistance[i]));
        }
        else if (i < environment.m_ShadowsSettings.m_CascadesDistance.size())
        {
            matrcies.push_back(GetLightSpaceMatrix(light, environment.m_ShadowsSettings.m_CascadesDistance[i - 1], environment.m_ShadowsSettings.m_CascadesDistance[i]));
        }
        else
        {
            matrcies.push_back(GetLightSpaceMatrix(light, environment.m_ShadowsSettings.m_CascadesDistance[i - 1], camera->GetZFar() * environment.m_ShadowsSettings.m_ZFarScale));
        }
    }

    return matrcies;
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

void Renderer::RenderOutline()
{
    Shader* shader = Shader::Get("Depth");
    shader->Bind();
    shader->SetUniformMat4f("u_ViewProjection", Environment::GetInstance().GetMainCamera()->GetViewProjectionMat4());

    auto DrawOutlineObject = [shader](GameObject* gameObject)
    {
        Renderer3D* r3d = gameObject->m_ComponentManager.GetComponent<Renderer3D>();
        if (r3d != nullptr && r3d->m_Mesh != nullptr)
        {
            shader->SetUniformMat4f("u_Transform", gameObject->m_Transform.GetTransform());

            Editor::GetInstance().m_Stats.m_DrawCalls++;
            Editor::GetInstance().m_Stats.m_Indices = r3d->m_Mesh->m_Indices.size();

            r3d->m_Mesh->m_Va.Bind();
            r3d->m_Mesh->m_Ib.Bind();

            glDrawElements(GL_TRIANGLES, r3d->m_Mesh->m_Ib.GetCount(), GL_UNSIGNED_INT, nullptr);

            r3d->m_Mesh->m_Va.UnBind();
            r3d->m_Mesh->m_Ib.UnBind();
        }
    };

    Viewport& viewport = Viewport::GetInstance();

    m_FrameBufferOutline->Bind();
    viewport.m_PreviousWindowSize = Window::GetInstance().GetSize();
    glViewport(0, 0, viewport.m_Size.x, viewport.m_Size.y);

    Window::GetInstance().Clear();
   
    std::vector<GameObject*> gameObjects = Editor::GetInstance().GetSelectedGameObjects();
    for (auto& gameObject : gameObjects)
    {
        DrawOutlineObject(gameObject);
        for (auto& child : gameObject->GetChilds())
        {
            DrawOutlineObject(child);
        }
    }

    m_FrameBufferOutline->UnBind();
    glViewport(0, 0, viewport.m_Size.x, viewport.m_Size.y);
}

void Renderer::Blur(FrameBuffer* frameBufferSource, const std::vector<FrameBuffer*>& frameBuffers, int blurPasses,
    float brightnessThreshold, int pixelsBlured)
{
    Shader* shader = Shader::Get("Blur");
    shader->Bind();
    shader->SetUniform1i("u_Pixels", pixelsBlured);
    shader->SetUniform1f("u_Brightness", brightnessThreshold);

    Viewport& viewport = Viewport::GetInstance();
    viewport.m_PreviousWindowSize = Window::GetInstance().GetSize();

    bool firstIterationOfFirstFrameBuffer = true;

    for (size_t j = 0; j < frameBuffers.size() / 2; j++)
    {
        glm::ivec2 size = frameBuffers[j * 2]->m_Params.m_Size;

        uint32_t texture = 0;

        if (j == 0) texture = frameBufferSource->m_Textures[0];
        else texture = frameBuffers[j - 1]->m_Textures[0];

        bool horizontal = true, firstIteration = true;

        for (size_t i = 0; i < blurPasses * 2 + 1; i++)
        {
            uint32_t index = (int)!horizontal + (j * 2);

            frameBuffers[index]->Bind();
            glViewport(0, 0, size.x, size.y);
            Window::GetInstance().Clear();

            shader->SetUniform1i("u_Horizontal", horizontal);
            shader->SetUniform1i("u_FirstFrameBufferIteration", firstIterationOfFirstFrameBuffer);

            int id = firstIteration ? texture : frameBuffers[(int)horizontal + (j * 2)]->m_Textures[0];
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, id);
            shader->SetUniform1i("u_Image", 0);

            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, frameBufferSource->m_Textures[0]);
            shader->SetUniform1i("u_SourceImage", 1);

            RenderFullScreenQuad();

            horizontal = !horizontal;
            if (firstIteration) firstIteration = false;
            if (firstIterationOfFirstFrameBuffer) firstIterationOfFirstFrameBuffer = false;
        }
    }
}

void Renderer::Bloom()
{
    Viewport& viewport = Viewport::GetInstance();
    Environment::BloomSettings bloomSettings = Environment::GetInstance().m_BloomSettings;
    
    Blur(m_FrameBufferScene, m_FrameBufferBlur, bloomSettings.m_BlurPasses, bloomSettings.m_BrightnessThreshold, bloomSettings.m_PixelsBlured);

    int samplers[32];
    for (size_t i = 0; i < m_FrameBufferBlur.size(); i+=2)
    {
        glActiveTexture(GL_TEXTURE0 + i / 2);
        glBindTexture(GL_TEXTURE_2D, m_FrameBufferBlur[i]->m_Textures[0]);
    }
    
    for (unsigned int i = 0; i < 32; i++)
    {
        samplers[i] = i;
    }

    Shader* shader = Shader::Get("AddBlurImages");
    shader->Bind();
    
    shader->SetUniform1iv("u_BlurTextures", samplers);
    shader->SetUniform1i("u_BlurTexturesSize", m_FrameBufferBlur.size() / 2);
    shader->SetUniform1f("u_Exposure", bloomSettings.m_Exposure);
    shader->SetUniform1f("u_Brightness", Environment::GetInstance().m_BloomSettings.m_BrightnessThreshold);

    m_FrameBufferBloom->Bind();
    viewport = Viewport::GetInstance();
    viewport.m_PreviousWindowSize = Window::GetInstance().GetSize();
    glViewport(0, 0, viewport.m_Size.x, viewport.m_Size.y);
    Window::GetInstance().Clear();

    RenderFullScreenQuad();
    
    m_FrameBufferBloom->UnBind();
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, TextureManager::GetInstance().White()->GetRendererID());
    glViewport(0, 0, viewport.m_PreviousWindowSize.x, viewport.m_PreviousWindowSize.y);
}

void Renderer::Render(Application* application)
{
    Timer timer = Timer(false, &Editor::GetInstance().m_Stats.m_RenderTime);

    Environment& environment = Environment::GetInstance();

    if (environment.m_ShadowsSettings.m_IsEnabled)
    {
        RenderCascadeShadowMaps(application->m_Scene);

        if (environment.m_ShadowsSettings.m_Blur.m_IsEnabled)
        {
            RenderCascadeShadowsToScene(application->m_Scene);
            Blur(m_FrameBufferShadows, m_FrameBufferShadowsBlur, environment.m_ShadowsSettings.m_Blur.m_BlurPasses,
                0.0f, environment.m_ShadowsSettings.m_Blur.m_PixelsBlured);
        }
    }

    BeginScene();
    {
        Timer timer = Timer(false, &Editor::GetInstance().m_Stats.m_RenderSceneTime);
        Instancing::GetInstance().Render(application->m_Scene->m_InstancedObjects);
        Batch::GetInstance().SetGameOjbectsShader(Shader::Get("Default2D"));
        Batch::GetInstance().BeginGameObjects();
        {
            application->m_Scene->Render();
        }
        Batch::GetInstance().EndGameObjects();
    }
    EndScene();

    Instancing::GetInstance().PrepareVertexAtrrib(application->m_Scene->m_InstancedObjects);

    BeginUI();
    {
        Batch::GetInstance().SetGameOjbectsShader(Shader::Get("Visualizer"));
        Batch::GetInstance().BeginGameObjects();
        {
            Visualizer::RenderQuads();
            Visualizer::RenderCircles();
        }
        Batch::GetInstance().EndGameObjects(true);

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
        if (environment.m_BloomSettings.m_IsEnabled)
        {
            Bloom();
        }
    }

    {
        RenderOutline();
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
