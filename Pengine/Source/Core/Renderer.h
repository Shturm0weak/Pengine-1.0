#pragma once

#define MAX_POINT_LIGHTS 1000
#define MAX_SPOT_LIGHTS 1000
#define MAX_MATERIALS 1000

#include "Core.h"

#include "../OpenGL/UniformBuffer.h"

#include <vector>
#include <memory>

namespace Pengine
{

	class PENGINE_API Renderer
	{
	private:

		struct PointLightUniform
		{
			glm::vec3 position;
			float constant;

			glm::vec3 color;
			int drawShadows;

			float _linear;
			float quadratic;
			float farPlane;
			float fog;
			
			int shadowMapIndex;
			float _unused1;
			float _unused2;
			float _unused3;
		};

		struct PointLights
		{
			std::vector<int> m_ShadowSamplers;
			std::vector<int> m_ShadowCubeMaps;
			UniformBuffer m_UniformBuffer;
			size_t m_Size = 0;
			size_t m_MaxShadowsSize = 10;
		} m_PointLights;

		struct SpotLightUniform
		{
			glm::vec3 position;
			float constant;

			glm::vec3 color;
			float _linear;

			glm::vec3 direction;
			float quadratic;

			float innerCutOff;
			float outerCutOff;
			float _unused1;
			float _unused2;
		};

		struct SpotLights
		{
			std::vector<int> m_ShadowSamplers;
			UniformBuffer m_UniformBuffer;
			size_t m_Size = 0;
		} m_SpotLights;

		struct MaterialUniform
		{
			glm::vec3 ambient;
			int baseColor;

			int normalColor;
			float useNormalMap;
			float alpha;
			float shiness;
		};

		struct Materials
		{
			UniformBuffer m_UniformBuffer;
			size_t m_Size = 0;
		} m_Materials;

		std::shared_ptr<class FrameBuffer> m_FrameBufferScene = nullptr;
		std::shared_ptr<class FrameBuffer> m_FrameBufferSSAO = nullptr;
		std::shared_ptr<class FrameBuffer> m_FrameBufferG = nullptr;
		std::shared_ptr<class FrameBuffer> m_FrameBufferGDownSampled = nullptr;
		std::shared_ptr<class FrameBuffer> m_FrameBufferUI = nullptr; 
		std::shared_ptr<class FrameBuffer> m_FrameBufferBloom = nullptr;
		std::shared_ptr<class FrameBuffer> m_FrameBufferOutline = nullptr;
		std::shared_ptr<class FrameBuffer> m_FrameBufferShadows = nullptr;
		std::vector<std::shared_ptr<class FrameBuffer>> m_FrameBufferBlur;
		std::vector<std::shared_ptr<class FrameBuffer>> m_FrameBufferCSM;
		std::vector<std::shared_ptr<class FrameBuffer>> m_FrameBufferShadowsBlur;
		std::vector<std::shared_ptr<class FrameBuffer>> m_FrameBufferSSAOBlur;

		std::vector<glm::mat4> m_LightSpaceMatrices;

		uint32_t m_SSAO = 0;
		std::vector<glm::vec3> m_SSAOKernel;

		static Renderer& GetInstance();

		Renderer() = default;
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) { return *this; }
		~Renderer() = default;

		void Initialize();

		void Begin(std::shared_ptr<FrameBuffer>& frameBuffer, const glm::vec4& clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), float depth = 1.0f);

		void End(std::shared_ptr<FrameBuffer>& frameBuffer);

		void ComposeFinalImage();

		void GenerateSSAOKernel();

		void GenerateSSAONoiseTexture();

		void PrepareUniformBuffers(class Scene* scene);

		void PrepareVertexAttrib(class Scene* scene);

		void PreparePointLightsUniformBuffer(const std::vector<class PointLight*>& pointLights);

		void PrepareSpotLightsUniformBuffer(const std::vector<class SpotLight*>& spotLights);

		void PrepareMaterialsUniformBuffer();

		bool RenderCascadeShadowMaps(class Scene* scene);

		void RenderCascadeShadowsToScene(class Scene* scene);

		void RenderPointLightShadows(class Scene* scene);

		glm::mat4 Renderer::GetLightSpaceMatrix(class DirectionalLight* light, const float nearPlane, const float farPlane);

		std::vector<glm::mat4> GetLightSpaceMatrices(class DirectionalLight* light);

		void RenderDeferred(Scene* scene);

		void RenderFullScreenQuad();

		void RenderOutline();

		void RenderSSAO();

		void Blur(std::shared_ptr<class FrameBuffer>& frameBufferSource, std::vector<std::shared_ptr<class FrameBuffer>>& frameBuffers, 
			int blurPasses, float brightnessThreshold, int pixelsBlured);

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

		friend class Instancing;
		friend class Viewport;
		friend class EntryPoint;
		friend class Editor;
		friend class PointLight;
		friend class Renderer3D;
	};

}
