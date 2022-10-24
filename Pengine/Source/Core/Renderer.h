#pragma once

#include "Core.h"

#include <vector>

namespace Pengine
{

	class PENGINE_API Renderer
	{
	private:

		Renderer() = default;
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) { return *this; }
		~Renderer() = default;

		class FrameBuffer* m_FrameBufferScene = nullptr;
		class FrameBuffer* m_FrameBufferSSAO = nullptr;
		class FrameBuffer* m_FrameBufferG = nullptr;
		class FrameBuffer* m_FrameBufferGDownSampled = nullptr;
		class FrameBuffer* m_FrameBufferUI = nullptr; 
		class FrameBuffer* m_FrameBufferBloom = nullptr;
		class FrameBuffer* m_FrameBufferOutline = nullptr;
		class FrameBuffer* m_FrameBufferShadows = nullptr;
		std::vector<class FrameBuffer*> m_FrameBufferBlur;
		std::vector<class FrameBuffer*> m_FrameBufferCSM;
		std::vector<class FrameBuffer*> m_FrameBufferShadowsBlur;
		std::vector<class FrameBuffer*> m_FrameBufferSSAOBlur;
		std::vector<class FrameBuffer*> m_FrameBufferPointLights;

		std::vector<glm::mat4> m_LightSpaceMatrices;

		uint32_t m_SSAO = 0;
		std::vector<glm::vec3> m_SSAOKernel;

		static Renderer& GetInstance();

		void Initialize();

		void Begin(FrameBuffer* frameBuffer, const glm::vec4& clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), float depth = 1.0f);

		void End(FrameBuffer* frameBuffer);

		void ComposeFinalImage();

		void GenerateSSAOKernel();

		void GenerateSSAONoiseTexture();

		void RenderCascadeShadowMaps(class Scene* scene);

		void RenderCascadeShadowsToScene(class Scene* scene);

		void RenderPointLightShadows(class Scene* scene);

		glm::mat4 Renderer::GetLightSpaceMatrix(class DirectionalLight* light, const float nearPlane, const float farPlane);

		std::vector<glm::mat4> GetLightSpaceMatrices(class DirectionalLight* light);

		void RenderDeferred(Scene* scene);

		void RenderFullScreenQuad();

		void RenderOutline();

		void RenderSSAO();

		void Blur(class FrameBuffer* frameBufferSource, const std::vector<class FrameBuffer*>& frameBuffers, int blurPasses,
			float brightnessThreshold, int pixelsBlured);

		// The best that I can get, though it is not perfect.
		// 1. Create several frame buffers for horizontal and vertical blur and each new framebuffer is half a size of a previous one.
		// 2. Loop through all frame buffers images and blur all bright pixels and repeat needed amount of times.
		// 3. Combine all blur images.
		void Bloom();

		void Render(class Application* application);

		void GeometryPass(Scene* scene);

		void SSAOPass(Scene* scene);

		void LightingPass(Scene* scene);

		void ShadowPass(Scene* scene);

		void PostProcessingPass(Application* application);

		void ShutDown();

		friend class Instancing;
		friend class Viewport;
		friend class EntryPoint;
		friend class Editor;
		friend class PointLight;
	};

}
