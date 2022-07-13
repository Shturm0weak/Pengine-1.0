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
	public:

		class FrameBuffer* m_FrameBufferScene = nullptr;
		class FrameBuffer* m_FrameBufferUI = nullptr; 
		class FrameBuffer* m_FrameBufferBloom = nullptr;
		std::vector<class FrameBuffer*> m_FrameBufferBlur;

		static Renderer& GetInstance();

		void Initialize();

		void BeginScene();

		void EndScene();

		void BeginUI();

		void EndUI();

		void ComposeFinalImage();

		void RenderFullScreenQuad();

		// The best that I can get, though it is not perfect.
		// 1. Create several frame buffers for horizontal and vertical blur and each new framebuffer is half a size of a previous one.
		// 2. Loop through all frame buffers images and blur all bright pixels and repeat needed amount of times.
		// 3. Combine all blur images.
		void Bloom();

		void Render(class Application* application);

		void ShutDown();
	};

}
