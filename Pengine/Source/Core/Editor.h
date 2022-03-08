#pragma once

#include "Core.h"
#include "Scene.h"
#include "Animation2DManager.h"

namespace Pengine
{

	class PENGINE_API Editor
	{
	private:

		struct Animation2DMenu
		{
			Animation2DManager::Animation2D* m_Animation = nullptr;

			char m_Name[64];
			bool m_Open = false;
		};

		struct TextureAtlasMenu
		{
			class TextureAtlas* m_TextureAtlas = nullptr;

			Texture* m_Texture = TextureManager::GetInstance().Get("White");
			glm::vec2 m_Size;
			char m_Name[64];
			bool m_Open = false;

			TextureAtlasMenu()
			{
				TextureManager::GetInstance().AsyncGet([=](Texture* texture) {
				m_Texture = texture; }, "White");
			}
		};

		struct Stats
		{
			size_t m_DrawCalls = 0;
			size_t m_Vertices = 0;
			size_t m_Indices = 0;
			double m_RenderTime = 0;
		};

		struct Animation2DComponent
		{
			int m_Selected = -1;
		};

		Animation2DMenu m_Animation2DMenu;
		Animation2DComponent m_Animation2DComponent;

		TextureAtlasMenu m_TextureAtlasMenu;

		std::vector<GameObject*> m_SelectedGameObjects;

		const char* m_RootDirectory = "Source";

		std::filesystem::path m_CurrentDirectory = std::filesystem::path(m_RootDirectory);

		Scene* m_CurrentScene = nullptr;

		bool m_Enable = true;

		void Debug();
		void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f,
			const glm::vec2& limits = glm::vec2(-25.0f, 25.0f), float speed = 0.1f, float columnWidth = 100.0f);
		void DrawNode(GameObject* gameObject, ImGuiTreeNodeFlags flags);
		void DrawChilds(GameObject* gameOBject);
		void DrawScene();
		void AssetBrowser();
		void Hierarchy();
		void Properties();
		void Navigation();
		void Environment();
		void ToolBar();
		void TransformComponent(Transform& transform);
		void Renderer2DComponent(GameObject* gameObject);
		void BoxCollider2DComponent(GameObject* gameObject);
		void Rigidbody2DComponent(GameObject* gameObject);
		void Animator2DComponent(GameObject* gameObject);
		void OnApplicationStop();
		void OnApplicationPlay();
		void Animation2DMenu();
		void TextureAtlasMenu();
		void FilePopUpMenu();
		void ViewPopUpMenu();
		void CreatePopUpMenu();
		void ComponentsMenu(GameObject* gameObject);
		void Settings();
		void AddSelectedGameObject(GameObject* gameObject);
		void ShortCuts();

		Editor() = default;
		Editor(const Editor&) = delete;
		Editor& operator=(const Editor&) { return *this; }
		~Editor() = default;

		friend class Viewport;
		friend class Scene;
	public:

		Stats m_Stats;

		bool m_DrawColliders = false;
		bool m_PolygonMode = false;
		
		bool m_Snap = false;
		float m_SnapThreshold = 1.0f;

		float m_LineWidth = 1.0f;

		static Editor& GetInstance();

		void ResetStats();
		void SetDarkThemeColors();
		void Update(Scene* scene);
		std::vector<GameObject*> GetSelectedGameObjects() const { return m_SelectedGameObjects; }
	};

}
