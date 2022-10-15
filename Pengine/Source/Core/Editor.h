#pragma once

#include "Core.h"
#include "Scene.h"
#include "Animation2DManager.h"

namespace Pengine
{

	class PENGINE_API Editor
	{
	private:

		struct OutlineParams
		{
			glm::vec3 m_Color = { 1.0f, 1.0f, 0.0f };
			int m_Thickness = 2;
		} m_OutlineParams;

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

		float m_ThumbnailScale = 0.8f;

		int m_SelectedWindowMode = 0;
		int m_SelectedCameraType = 0;

		bool m_IsActiveAssetList = false;

		bool m_IsEnabled = true;

		bool m_DrawVecLabel = true;

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
		
		void DrawIVec2Control(const std::string& label, glm::ivec2& values, float resetValue = 0.0f,
			const glm::vec2& limits = glm::vec2(-25.0f, 25.0f), float speed = 0.1f, float columnWidth = 100.0f);

		void DrawIVec3Control(const std::string& label, glm::ivec3& values, float resetValue = 0.0f,
			const glm::vec2& limits = glm::vec2(-25.0f, 25.0f), float speed = 0.1f, float columnWidth = 100.0f);

		void DrawIVec4Control(const std::string& label, glm::ivec4& values, float resetValue = 0.0f,
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
		
		void PointLightComponent(GameObject* gameObject);
		
		void SpotLightComponent(GameObject* gameObject);

		void DirectionalLightComponent(GameObject* gameObject);

		void Renderer3DComponent(GameObject* gameObject);

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

		void DropMaterialOnViewport(const std::string& filePath);

		friend class Viewport;
		friend class Scene;
		friend class Renderer;
	public:

		struct Stats
		{
			size_t m_DrawCalls = 0;
			size_t m_Vertices = 0;
			size_t m_Indices = 0;
			static size_t s_AllocationsCount;
			double m_RenderTime = 0.0;
			double m_RenderGBufferTime = 0.0;
			double m_RenderShadowsTime = 0.0;
			double m_RenderSceneTime = 0.0;
			double m_RenderBloomTime = 0.0;
			double m_RenderUITime = 0.0;
			double m_RenderLinesTime = 0.0;
			double m_RenderEditor = 0.0;
			double m_RenderComposeTime = 0.0;
		} m_Stats;

		bool m_DrawLights = true;
		bool m_DrawColliders = false;
		bool m_PolygonMode = false;
		bool m_DrawBoundingBoxes = false;
		bool m_Snap = false;
		float m_SnapThreshold = 1.0f;

		int m_LineWidth = 1;

		bool m_ShowNavigation = true;

		static Editor& GetInstance();

		Scene& GetCurrentScene() const { return *m_CurrentScene; }

		void ResetStats();
		
		void SetDarkThemeColors();
		
		void Update(Scene* scene);
		
		void SetEnabled(bool enabled) { m_IsEnabled = enabled; }

		std::vector<GameObject*> GetSelectedGameObjects() const { return m_SelectedGameObjects; }
	};

}
