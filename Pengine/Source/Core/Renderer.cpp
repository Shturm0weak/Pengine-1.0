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
    glm::ivec2 size = { 100, 100 };

    TextureManager::GetInstance().m_TexParameters[0] = { GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST };
    TextureManager::GetInstance().m_TexParameters[1] = { GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST };

    FrameBuffer::FrameBufferParams SSAOParams = { size, GL_COLOR_ATTACHMENT0, GL_RED, GL_RED, GL_FLOAT };
    m_FrameBufferSSAO = new FrameBuffer({ SSAOParams }, TextureManager::GetInstance().GetTexParamertersi());

    TextureManager::GetInstance().m_TexParameters[0] = { GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR };
    TextureManager::GetInstance().m_TexParameters[1] = { GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR };

    FrameBuffer::FrameBufferParams sceneParams = { size, GL_COLOR_ATTACHMENT0, GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT };
    m_FrameBufferScene = new FrameBuffer({ sceneParams }, TextureManager::GetInstance().GetTexParamertersi());
    m_FrameBufferScene->GenerateRbo();

    m_FrameBufferG = new FrameBuffer(
        {
            { size, GL_COLOR_ATTACHMENT0, GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT }, // Albedo.
            { size, GL_COLOR_ATTACHMENT1, GL_RGB32F, GL_RGB, GL_FLOAT }, // WorldPosition.
            { size, GL_COLOR_ATTACHMENT2, GL_RGB16F, GL_RGB, GL_FLOAT }, // Normal.
        },
        TextureManager::GetInstance().GetTexParamertersi());
    m_FrameBufferG->GenerateRbo();

    m_FrameBufferGDownSampled = new FrameBuffer(
        {
            { size / 2, GL_COLOR_ATTACHMENT0, GL_RGB32F, GL_RGB, GL_FLOAT }, // WorldPosition.
            { size / 2, GL_COLOR_ATTACHMENT1, GL_RGB16F, GL_RGB, GL_FLOAT }, // Normal.
        },
        TextureManager::GetInstance().GetTexParamertersi());

    m_FrameBufferShadows = new FrameBuffer({ { size, GL_COLOR_ATTACHMENT0, GL_RGB, GL_RGB, GL_FLOAT } }, TextureManager::GetInstance().GetTexParamertersi());
    m_FrameBufferShadows->GenerateRbo();
    
    FrameBuffer::FrameBufferParams uiParams = { size, GL_COLOR_ATTACHMENT0, GL_RGBA, GL_RGBA, GL_UNSIGNED_INT };
    m_FrameBufferUI = new FrameBuffer({ uiParams }, TextureManager::GetInstance().GetTexParamertersi());

    FrameBuffer::FrameBufferParams bloomParams = { size, GL_COLOR_ATTACHMENT0, GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT};
    m_FrameBufferBloom = new FrameBuffer({ bloomParams }, TextureManager::GetInstance().GetTexParamertersi());

    FrameBuffer::FrameBufferParams SSAOBlurParams = { size, GL_COLOR_ATTACHMENT0, GL_RED, GL_RED,
       GL_UNSIGNED_INT };
    m_FrameBufferSSAOBlur.push_back(new FrameBuffer({ SSAOBlurParams }, TextureManager::GetInstance().GetTexParamertersi()));
    m_FrameBufferSSAOBlur.push_back(new FrameBuffer({ SSAOBlurParams }, TextureManager::GetInstance().GetTexParamertersi()));

    FrameBuffer::FrameBufferParams blurParams = { size, GL_COLOR_ATTACHMENT0, GL_RGB, GL_RGB,
      GL_UNSIGNED_INT };
    m_FrameBufferShadowsBlur.push_back(new FrameBuffer({ blurParams }, TextureManager::GetInstance().GetTexParamertersi()));
    m_FrameBufferShadowsBlur.push_back(new FrameBuffer({ blurParams }, TextureManager::GetInstance().GetTexParamertersi()));

    for (size_t i = 0; i < 12; i += 2)
    {
        size /= 2;
        m_FrameBufferBlur.push_back(new FrameBuffer({ bloomParams }, TextureManager::GetInstance().GetTexParamertersi()));
        m_FrameBufferBlur.push_back(new FrameBuffer({ bloomParams }, TextureManager::GetInstance().GetTexParamertersi()));
    }

    FrameBuffer::FrameBufferParams depthParams = { size, GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT };
    m_FrameBufferOutline = new FrameBuffer({ depthParams }, TextureManager::GetInstance().GetTexParamertersi());

    TextureManager::GetInstance().m_TexParameters[0] = { GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST };
    TextureManager::GetInstance().m_TexParameters[1] = { GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST };
    TextureManager::GetInstance().m_TexParameters[2] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER };
    TextureManager::GetInstance().m_TexParameters[3] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER };

    depthParams.m_Size = { 4096, 4096 };
    m_FrameBufferCSM.push_back(new FrameBuffer({ depthParams }, TextureManager::GetInstance().GetTexParamertersi()));
    depthParams.m_Size = { 2048, 2048 };
    m_FrameBufferCSM.push_back(new FrameBuffer({ depthParams }, TextureManager::GetInstance().GetTexParamertersi()));
    depthParams.m_Size = { 1024, 1024 };
    m_FrameBufferCSM.push_back(new FrameBuffer({ depthParams }, TextureManager::GetInstance().GetTexParamertersi()));
    //for (size_t i = 0; i < 3; i++)
    //{
    //}

    TextureManager::GetInstance().ResetTexParametersi();

    GenerateSSAOKernel();
    GenerateSSAONoiseTexture();

    Logger::Success("Renderer has been initialized!");
}

void Renderer::Begin(FrameBuffer* frameBuffer, const glm::vec4& clearColor, float depth)
{
    frameBuffer->Bind();
    Viewport& viewport = Viewport::GetInstance();
    viewport.m_PreviousWindowSize = Window::GetInstance().GetSize();
    glViewport(0, 0, viewport.m_Size.x, viewport.m_Size.y);
    Window::GetInstance().Clear(clearColor, depth);
}

void Renderer::End(FrameBuffer* frameBuffer)
{
    frameBuffer->UnBind();
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

void Renderer::GenerateSSAOKernel()
{
    m_SSAOKernel.clear();

    auto ourLerp = [](float a, float b, float f)
    {
        return a + f * (b - a);
    };

    // generate sample kernel
    // ----------------------
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    for (unsigned int i = 0; i < Environment::GetInstance().m_SSAO.m_KernelSize; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / (float)Environment::GetInstance().m_SSAO.m_KernelSize;

        // scale samples s.t. they're more aligned to center of kernel
        scale = ourLerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        m_SSAOKernel.push_back(sample);
    }
}

void Renderer::GenerateSSAONoiseTexture()
{
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;

    // generate noise texture
    // ----------------------
    int dimension = Environment::GetInstance().m_SSAO.m_NoiseTextureDimension;
    std::vector<glm::vec3> ssaoNoise;
    for (int i = 0; i < dimension * dimension; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    if (m_SSAO != 0)
    {
        glDeleteTextures(1, &m_SSAO);
        m_SSAO = 0;
    }

    glGenTextures(1, &m_SSAO);
    glBindTexture(GL_TEXTURE_2D, m_SSAO);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, dimension, dimension, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
        glViewport(0, 0, m_FrameBufferCSM[i]->m_Params[0].m_Size.x, m_FrameBufferCSM[i]->m_Params[0].m_Size.y);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_CLAMP);
        Instancing::GetInstance().RenderShadowsObjects(scene->m_ShadowsByMesh, Instancing::GetInstance().m_ShadowsBuffersByMesh);
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

    shader->SetUniform3fv("u_CameraPosition", environment.GetMainCamera()->m_Transform.GetPosition());
    shader->SetUniformMat4f("u_Shadows.view", environment.GetMainCamera()->GetViewMat4());
    shader->SetUniformfv("u_Shadows.cascadesDistance", environment.m_ShadowsSettings.m_CascadesDistance);
    shader->SetUniform1i("u_Shadows.pcf", environment.m_ShadowsSettings.m_Pcf);
    shader->SetUniform1f("u_Shadows.fog", environment.m_ShadowsSettings.m_Fog);
    shader->SetUniform1f("u_Shadows.bias", environment.m_ShadowsSettings.m_Bias);
    shader->SetUniform1f("u_Shadows.farPlane", environment.GetMainCamera()->GetZFar() * environment.m_ShadowsSettings.m_ZFarScale);
    shader->SetUniform1i("u_Shadows.isVisualized", environment.m_ShadowsSettings.m_IsVisualized);
    shader->SetUniform1i("u_Shadows.cascadesCount", (int)environment.m_ShadowsSettings.m_CascadesDistance.size());
    shader->SetUniform1f("u_Shadows.texels", (int)environment.m_ShadowsSettings.m_Texels);
    shader->SetUniformMat4fv("u_Shadows.lightSpaceMatricies", Renderer::GetInstance().m_LightSpaceMatrices);

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, m_FrameBufferG->m_Textures[1]);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, m_FrameBufferG->m_Textures[2]);

    shader->SetUniform1i("u_WorldPosition", 0);
    shader->SetUniform1i("u_Normal", 1);

    std::vector<int> csm(Renderer::GetInstance().m_FrameBufferCSM.size());
    for (size_t i = 0; i < csm.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i + 2);
        glBindTexture(GL_TEXTURE_2D, Renderer::GetInstance().m_FrameBufferCSM[i]->m_Textures[0]);
        csm[i] = i + 2;
        shader->SetUniform1iv("u_Shadows.CSM", &csm[0], csm.size());
    }

    m_FrameBufferShadows->Bind();
    viewport.m_PreviousWindowSize = Window::GetInstance().GetSize();
    glViewport(0, 0, m_FrameBufferShadows->m_Params[0].m_Size.x, m_FrameBufferShadows->m_Params[0].m_Size.y);
    Window::GetInstance().Clear();

    RenderFullScreenQuad();

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

void Renderer::RenderDeferred(Scene* scene)
{
    Shader* shader = Shader::Get("Deferred");
    shader->Bind();

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, m_FrameBufferG->m_Textures[0]);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, m_FrameBufferG->m_Textures[1]);

    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, m_FrameBufferG->m_Textures[2]);

    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, Renderer::GetInstance().m_FrameBufferShadowsBlur[0]->m_Textures[0]);

    glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_2D, m_FrameBufferSSAOBlur[0]->m_Textures[0]);

    Environment& environment = Environment::GetInstance();

    shader->SetUniform1i("u_Albedo", 0);
    shader->SetUniform1i("u_WorldPosition", 1);
    shader->SetUniform1i("u_Normal", 2);
    shader->SetUniform1i("u_Shadows.color", 3);
    shader->SetUniform1i("u_SSAO.color", 4);
    shader->SetUniform1i("u_SSAO.isEnabled", (int)environment.m_SSAO.m_IsEnabled);
    shader->SetUniform3fv("u_CameraPosition", environment.GetMainCamera()->m_Transform.GetPosition());
    shader->SetUniform2fv("u_ViewportSize", (glm::vec2)Viewport::GetInstance().GetSize());
    shader->SetUniform1i("u_Shadows.isEnabled", environment.m_ShadowsSettings.m_IsEnabled);

    if (scene->m_DirectionalLights.empty())
    {
        shader->SetUniform1f("u_DirectionalLight.intensity", 0.0f);
    }
    else
    {
        DirectionalLight* directionalLight = scene->m_DirectionalLights[0];

        shader->SetUniform3fv("u_DirectionalLight.direction", directionalLight->GetDirection());
        shader->SetUniform3fv("u_DirectionalLight.color", directionalLight->m_Color);
        shader->SetUniform1f("u_DirectionalLight.intensity", directionalLight->m_Intensity);
    }

    int pointLightsSize = scene->m_PointLights.size();
    shader->SetUniform1i("pointLightsSize", pointLightsSize);
    char uniformBuffer[64];
    for (int i = 0; i < pointLightsSize; i++)
    {
        PointLight* pointLight = scene->m_PointLights[i];
        sprintf(uniformBuffer, "pointLights[%i].position", i);
        shader->SetUniform3fv(uniformBuffer, pointLight->GetOwner()->m_Transform.GetPosition());
        sprintf(uniformBuffer, "pointLights[%i].color", i);
        shader->SetUniform3fv(uniformBuffer, pointLight->m_Color);
        sprintf(uniformBuffer, "pointLights[%i].constant", i);
        shader->SetUniform1f(uniformBuffer, pointLight->m_Constant);
        sprintf(uniformBuffer, "pointLights[%i]._linear", i);
        shader->SetUniform1f(uniformBuffer, pointLight->m_Linear);
        sprintf(uniformBuffer, "pointLights[%i].quadratic", i);
        shader->SetUniform1f(uniformBuffer, pointLight->m_Quadratic);
    }

    int spotLightsSize = scene->m_SpotLights.size();
    shader->SetUniform1i("spotLightsSize", spotLightsSize);
    for (int i = 0; i < spotLightsSize; i++)
    {
        SpotLight* spotLight = scene->m_SpotLights[i];
        sprintf(uniformBuffer, "spotLights[%i].position", i);
        shader->SetUniform3fv(uniformBuffer, spotLight->GetOwner()->m_Transform.GetPosition());
        sprintf(uniformBuffer, "spotLights[%i].direction", i);
        shader->SetUniform3fv(uniformBuffer, spotLight->GetDirection());
        sprintf(uniformBuffer, "spotLights[%i].color", i);
        shader->SetUniform3fv(uniformBuffer, spotLight->m_Color);
        sprintf(uniformBuffer, "spotLights[%i].constant", i);
        shader->SetUniform1f(uniformBuffer, spotLight->m_Constant);
        sprintf(uniformBuffer, "spotLights[%i]._linear", i);
        shader->SetUniform1f(uniformBuffer, spotLight->m_Linear);
        sprintf(uniformBuffer, "spotLights[%i].quadratic", i);
        shader->SetUniform1f(uniformBuffer, spotLight->m_Quadratic);
        sprintf(uniformBuffer, "spotLights[%i].innerCutOff", i);
        shader->SetUniform1f(uniformBuffer, (spotLight->m_CosInnerCutOff));
        sprintf(uniformBuffer, "spotLights[%i].outerCutOff", i);
        shader->SetUniform1f(uniformBuffer, (spotLight->m_CosOuterCutOff));
    }

    RenderFullScreenQuad();
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

    auto DrawOutlineObject = [shader](GameObject* gameObject)
    {
        Renderer3D* r3d = gameObject->m_ComponentManager.GetComponent<Renderer3D>();
        if (r3d != nullptr && r3d->m_Mesh != nullptr)
        {
            shader->SetUniformMat4f("u_TransformViewProjected", Environment::GetInstance().GetMainCamera()->GetViewProjectionMat4() * gameObject->m_Transform.GetTransform());

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
   
    std::vector<GameObject*> gameObjects = Editor::GetInstance().GetSelectedGameObjects();
    for (auto& gameObject : gameObjects)
    {
        DrawOutlineObject(gameObject);
        for (auto& child : gameObject->GetChilds())
        {
            DrawOutlineObject(child);
        }
    }
}

void Renderer::RenderSSAO()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FrameBufferG->m_Fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FrameBufferGDownSampled->m_Fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(
        0, 0, m_FrameBufferG->m_Params[0].m_Size.x, m_FrameBufferG->m_Params[0].m_Size.y, 0, 0,
        m_FrameBufferGDownSampled->m_Params[0].m_Size.x, m_FrameBufferGDownSampled->m_Params[0].m_Size.y, GL_COLOR_BUFFER_BIT, GL_LINEAR
    );

    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FrameBufferG->m_Fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT2);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FrameBufferGDownSampled->m_Fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT1);
    glBlitFramebuffer(
        0, 0, m_FrameBufferG->m_Params[0].m_Size.x, m_FrameBufferG->m_Params[0].m_Size.y, 0, 0,
        m_FrameBufferGDownSampled->m_Params[0].m_Size.x, m_FrameBufferGDownSampled->m_Params[0].m_Size.y, GL_COLOR_BUFFER_BIT, GL_LINEAR
    );

    Viewport& viewport = Viewport::GetInstance();
    Environment& environment = Environment::GetInstance();
    std::shared_ptr<Camera> camera = environment.GetMainCamera();

    Shader* shader = Shader::Get("SSAO");
    shader->Bind();

    // Send kernel + rotation 
    for (unsigned int i = 0; i < Environment::GetInstance().m_SSAO.m_KernelSize; ++i)
    {
        shader->SetUniform3fv("u_Samples[" + std::to_string(i) + "]", m_SSAOKernel[i]);
    }
    shader->SetUniformMat4f("u_ViewProjection", camera->GetViewProjectionMat4());
    shader->SetUniform3fv("u_CameraPosition", camera->m_Transform.GetPosition());
    shader->SetUniform3fv("u_CameraDirection", glm::normalize(camera->m_Transform.GetForward()));
    shader->SetUniform2fv("u_Resolution", glm::vec2(viewport.GetSize()));
    shader->SetUniform1f("u_Radius", environment.m_SSAO.m_Radius);
    shader->SetUniform1f("u_Bias", environment.m_SSAO.m_Bias);
    shader->SetUniform1i("u_KernelSize", environment.m_SSAO.m_KernelSize);
    shader->SetUniform1f("u_NoiseSize", (float)environment.m_SSAO.m_NoiseTextureDimension);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_FrameBufferGDownSampled->m_Textures[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_FrameBufferGDownSampled->m_Textures[1]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_SSAO);

    shader->SetUniform1i("u_Position", 0);
    shader->SetUniform1i("u_Normal", 1);
    shader->SetUniform1i("u_Noise", 2);

    Begin(m_FrameBufferSSAO);

    RenderFullScreenQuad();

    End(m_FrameBufferSSAO);
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
        glm::ivec2 size = frameBuffers[j * 2]->m_Params[0].m_Size;

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

    Begin(m_FrameBufferBloom);
    {
        RenderFullScreenQuad();
    }
    End(m_FrameBufferBloom);
}

void Renderer::Render(Application* application)
{
    Timer timer = Timer(false, &Editor::GetInstance().m_Stats.m_RenderTime);

    SortLods(application->m_Scene);

    GeometryPass(application->m_Scene);
    ShadowPass(application->m_Scene);

    Instancing::GetInstance().PrepareVertexAtrrib(application->m_Scene->m_OpaqueByMesh);
    application->m_Scene->PrepareVisualizer();
    application->m_Scene->SortTransparent();
    
    SSAOPass(application->m_Scene);
    LightingPass(application->m_Scene);
    PostProcessingPass(application);
}

void Renderer::SortLods(Scene* scene)
{
    std::shared_ptr<Camera> camera = Environment::GetInstance().GetMainCamera();
    glm::vec3 cameraPosition = camera->m_Transform.GetPosition();

    for (auto& r3d : scene->m_Renderers3D)
    {
        std::vector<float> lodsDistance = r3d->m_LodsDistance;
        glm::vec3 position = r3d->GetOwner()->m_Transform.GetPosition();
        float distance2 = glm::distance2(cameraPosition, position);
            
        if (distance2 < lodsDistance[0] * lodsDistance[0])
        {
            r3d->SetCurrentLod(0);
        }
        else if (distance2 < lodsDistance[1] * lodsDistance[1])
        {
            r3d->SetCurrentLod(1);
        }
        else
        {
            r3d->SetCurrentLod(2);
        }
    }
}

void Renderer::GeometryPass(Scene* scene)
{
    Begin(m_FrameBufferG);
    {
        Timer timer = Timer(false, &Editor::GetInstance().m_Stats.m_RenderGBufferTime);
        Instancing::GetInstance().BindBuffers(scene->m_OpaqueByMesh);
        Instancing::GetInstance().RenderGBuffer(scene->m_OpaqueByMesh, Instancing::GetInstance().m_OpaqueBuffersByMesh);
    }
    End(m_FrameBufferG);
}

void Renderer::SSAOPass(Scene* scene)
{
    Environment& environment = Environment::GetInstance();

    if (environment.m_SSAO.m_IsEnabled)
    {
        Timer timer = Timer(false, &Editor::GetInstance().m_Stats.m_RenderSSAOTime);
        RenderSSAO();
        Blur(m_FrameBufferSSAO, m_FrameBufferSSAOBlur, environment.m_SSAO.m_Blur.m_BlurPasses,
            0.0f, environment.m_SSAO.m_Blur.m_PixelsBlured);
    }
}

void Renderer::LightingPass(Scene* scene)
{
    Begin(m_FrameBufferScene);
    {
        Timer timer = Timer(false, &Editor::GetInstance().m_Stats.m_RenderSceneTime);

        RenderDeferred(scene);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FrameBufferG->m_Fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FrameBufferScene->m_Fbo);
        glBlitFramebuffer(
            0, 0, m_FrameBufferG->m_Params[0].m_Size.x, m_FrameBufferG->m_Params[0].m_Size.y, 0, 0,
            m_FrameBufferScene->m_Params[0].m_Size.x, m_FrameBufferScene->m_Params[0].m_Size.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST
        );

        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferScene->m_Fbo);

        Batch::GetInstance().SetGameOjbectsShader(Shader::Get("Default2D"));
        Batch::GetInstance().BeginGameObjects();
        {
            scene->Render();
        }
        Batch::GetInstance().EndGameObjects();
    }
    End(m_FrameBufferScene);
}

void Renderer::ShadowPass(Scene* scene)
{
    Environment& environment = Environment::GetInstance();

    if (environment.m_ShadowsSettings.m_IsEnabled)
    {
        Timer timer = Timer(false, &Editor::GetInstance().m_Stats.m_RenderShadowsTime);

        Instancing::GetInstance().BindShadowsBuffers(scene->m_ShadowsByMesh);
        RenderCascadeShadowMaps(scene);
        RenderCascadeShadowsToScene(scene);
        Blur(m_FrameBufferShadows, m_FrameBufferShadowsBlur, environment.m_ShadowsSettings.m_Blur.m_BlurPasses,
            0.0f, environment.m_ShadowsSettings.m_Blur.m_PixelsBlured);
    }
}

void Renderer::PostProcessingPass(Application* application)
{
    Environment& environment = Environment::GetInstance();
    Batch& batch = Batch::GetInstance();
    Editor& editor = Editor::GetInstance();
    Viewport& viewport = Viewport::GetInstance();

    Begin(m_FrameBufferUI, { 0.0f, 0.0f, 0.0f, 0.0f });
    {
        batch.SetGameOjbectsShader(Shader::Get("Visualizer"));
        batch.BeginGameObjects();
        {
            Visualizer::RenderQuads();
            Visualizer::RenderCircles();
        }
        batch.EndGameObjects(true);

        {
            Timer timer = Timer(false, &editor.m_Stats.m_RenderLinesTime);
            Visualizer::RenderLines();
        }

        Timer timer = Timer(false, &editor.m_Stats.m_RenderUITime);
        //Gui::GetInstance().Begin(); It is called in Window::NewFrame for allowing OnGuiRender calls from components.
        {
            application->OnGuiRender();
        }
        Gui::GetInstance().End();
    }
    End(m_FrameBufferUI);

    {
        Timer timer = Timer(false, &editor.m_Stats.m_RenderBloomTime);
        if (environment.m_BloomSettings.m_IsEnabled)
        {
            Bloom();
        }
    }

    Begin(m_FrameBufferOutline);
    {
        RenderOutline();
    }
    End(m_FrameBufferOutline);

    viewport.Begin();
    {
        Timer timer = Timer(false, &editor.m_Stats.m_RenderComposeTime);
        ComposeFinalImage();
    }
    viewport.End();
}

void Renderer::ShutDown()
{
    delete m_FrameBufferBloom;
    delete m_FrameBufferG;
    delete m_FrameBufferOutline;
    delete m_FrameBufferScene;
    delete m_FrameBufferShadows;
    delete m_FrameBufferSSAO;
    delete m_FrameBufferUI;

    for (size_t i = 0; i < m_FrameBufferBlur.size(); i++)
    {
        delete m_FrameBufferBlur[i];
    }

    for (size_t i = 0; i < m_FrameBufferCSM.size(); i++)
    {
        delete m_FrameBufferCSM[i];
    }

    for (size_t i = 0; i < m_FrameBufferShadowsBlur.size(); i++)
    {
        delete m_FrameBufferShadowsBlur[i];
    }

    for (size_t i = 0; i < m_FrameBufferSSAOBlur.size(); i++)
    {
        delete m_FrameBufferSSAOBlur[i];
    }
}
