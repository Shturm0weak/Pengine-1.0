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
			bool m_IsActive = false;

			void Update();
		} m_Animation2DMenu;

		struct TextureAtlasMenu
		{
			class TextureAtlas* m_TextureAtlas = nullptr;
			bool m_IsActive = false;

			void Update();
		} m_TextureAtlasMenu;

		struct Animation2DComponent
		{
			int m_Selected = -1;
		} m_Animation2DComponent;
		
		struct DeleteDirectoryOrFile
		{
			std::filesystem::path m_FilePath;
			std::string m_FileName;
			bool m_IsActive = false;

			void Update();
		} m_DeleteDirectoryOrFile;

		struct TextureMenu
		{
			Texture* m_Texture = nullptr;
			bool m_IsActive = false;
		} m_TextureMenu;

		struct CreatingFileMenu
		{
			std::filesystem::path m_FilePath;
			char m_FileName[64];
			bool m_IsActive = false;

			void Update();
		} m_CreatingFileMenu;

		std::vector<GameObject*> m_SelectedGameObjects;

		const char* m_RootDirectory = "Source";

		std::filesystem::path m_CurrentDirectory = std::filesystem::path(m_RootDirectory);

		Scene* m_CurrentScene = nullptr;

		bool m_IsActiveAssetList = false;

		bool m_IsEnabled = true;

		Editor() = default;
		Editor(const Editor&) = delete;
		Editor& operator=(const Editor&) { return *this; }
		~Editor() = default;
		
		void Debug();
		
		void DrawVec2Control(const std::string& label, glm::vec2& values, float resetValue = 0.0f,
			const glm::vec2& limits = glm::vec2(-25.0f, 25.0f), float speed = 0.1f, float columnWidth = 100.0f);

		void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f,
			const glm::vec2& limits = glm::vec2(-25.0f, 25.0f), float speed = 0.1f, float columnWidth = 100.0f);

		void DrawVec4Control(const std::string& label, glm::vec4& values, float resetValue = 0.0f,
			const glm::vec2& limits = glm::vec2(-25.0f, 25.0f), float speed = 0.1f, float columnWidth = 100.0f);
		
		void DrawNode(GameObject* gameObject, ImGuiTreeNodeFlags flags);
		
		void DrawChilds(GameObject* gameOBject);
		
		void AssetList();
		
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
		
		void CircleCollider2DComponent(GameObject* gameObject);
		
		void Rigidbody2DComponent(GameObject* gameObject);
		
		void Animator2DComponent(GameObject* gameObject);
		
		void ParticleEmitterComponent(GameObject* gameObject);
		
		void ScriptComponent(GameObject* gameObject);
		
		void PointLight2DComponent(GameObject* gameObject);

		void UserDefinedComponents(GameObject* gameObject);

		void OnApplicationStop();
		
		void OnApplicationPlay();
		
		void FilePopUpMenu();
		
		void ViewPopUpMenu();
		
		void CreatePopUpMenu();
		
		void TextMenu();
		
		void TextureMenu();
		
		void ComponentsMenu(GameObject* gameObject);
		
		void Settings();
		
		void AddSelectedGameObject(GameObject* gameObject);
		
		void ShortCuts();

		bool RemoveComponentMenu(GameObject* gameObject, IComponent* component);

		friend class Viewport;
		friend class Scene;
	public:

		struct Stats
		{
			size_t m_DrawCalls = 0;
			size_t m_Vertices = 0;
			size_t m_Indices = 0;
			static size_t s_AllocationsCount;
			double m_RenderTime = 0.0;
			double m_RenderSceneTime = 0.0;
			double m_RenderBloomTime = 0.0;
			double m_RenderUITime = 0.0;
			double m_RenderLinesTime = 0.0;
			double m_RenderEditor = 0.0;
			double m_RenderComposeTime = 0.0;
		} m_Stats;

		bool m_DrawColliders = false;
		bool m_PolygonMode = false;
		
		bool m_ShowNavigation = true;

		bool m_Snap = false;
		float m_SnapThreshold = 1.0f;

		int m_LineWidth = 1;

		static Editor& GetInstance();

		void ResetStats();
		
		void SetDarkThemeColors();
		
		void Update(Scene* scene);
		
		void SetEnabled(bool enabled) { m_IsEnabled = enabled; }

		std::vector<GameObject*> GetSelectedGameObjects() const { return m_SelectedGameObjects; }
	};

}
