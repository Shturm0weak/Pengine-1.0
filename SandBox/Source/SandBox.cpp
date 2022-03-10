#include "Core/EntryPoint.h"
#include "Core/Input.h"
#include "Core/Environment.h"
#include "Core/Visualizer.h"
#include "Core/Viewport.h"
#include "Components/Renderer2D.h"
#include "EventSystem/EventSystem.h"
#include "Components/Rigidbody2D.h"
#include "Components/BoxCollider2D.h"
#include "Components/Animator2D.h"

#include "MathApp.h"

using namespace Pengine;

class Controller : public IComponent, public IListener
{
private:

	Animator2D* a2d = nullptr;
	Renderer2D* r2d = nullptr;
	Rigidbody2D* rb2d = nullptr;

	int m_lastDirection = 1;
	const float m_Speed = 3.0f;

	virtual IComponent* CreateCopy(GameObject* newOwner) override
	{
		Controller* controller = new Controller();
		controller->Copy(*this);
		return controller;
	}

	void Copy(const IComponent& component)
	{
		Controller& controller = *(Controller*)&component;
		m_Type = component.GetType();
	}

	virtual void OnStart() override
	{
		a2d = m_Owner->m_ComponentManager.GetComponent<Animator2D>();
		r2d = m_Owner->m_ComponentManager.GetComponent<Renderer2D>();
		rb2d = m_Owner->m_ComponentManager.GetComponent<Rigidbody2D>();
	}

	virtual void OnClose() override
	{
	}

	virtual void OnUpdate() override
	{
		//Visualizer::DrawLine(m_Owner->m_Transform.GetPosition(), m_Owner->m_Transform.GetPosition() + m_Owner->m_Transform.GetRight(), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		if (rb2d)
		{
			if (rb2d->m_Body)
			{
				rb2d->m_Body->SetAwake(true);
			}
		}
		if (Input::IsKeyDown(Keycode::KEY_D))
		{
			m_Owner->m_Transform.Translate(m_Owner->m_Transform.GetPosition()
				+ glm::vec3(m_Speed * Time::GetDeltaTime(), 0.0f, 0.0f));
			a2d->m_CurrentAnimation = a2d->m_Animations[1];
			r2d->OriginalUV();
			m_lastDirection = 1;
		}
		else if (Input::IsKeyDown(Keycode::KEY_A))
		{
			m_Owner->m_Transform.Translate(m_Owner->m_Transform.GetPosition()
				- glm::vec3(m_Speed * Time::GetDeltaTime(), 0.0f, 0.0f));
			a2d->m_CurrentAnimation = a2d->m_Animations[1];
			r2d->ReversedUV();
			m_lastDirection = -1;
		}
		else
		{
			m_lastDirection == 1 ? r2d->ReversedUV() : r2d->OriginalUV();
			a2d->m_CurrentAnimation = a2d->m_Animations[0];
		}
	}

	virtual void OnWindowResize(const WindowResizeEvent& event) override
	{
		printf_s("Size: %d %d\n", event.GetSize().x, event.GetSize().y);
	}
public:

	Controller()
	{
		EventSystem::GetInstance().RegisterClient(EventType::ONSTART, this);
		EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, this);
		EventSystem::GetInstance().RegisterClient(EventType::ONWINDOWRESIZE, this);
	}

	~Controller()
	{
		EventSystem::GetInstance().UnregisterAll(this);
	}

	static IComponent* Create(GameObject* owner)
	{
		Controller* controller = new Controller();
		//controller->a2d = owner->m_ComponentManager.GetComponent<Animator2D>();
		//controller->a2d->AddAnimation(Animation2DManager::GetInstance().Load("Source/Animations2D/IdleRight.anim"));
		//controller->a2d->AddAnimation(Animation2DManager::GetInstance().Load("Source/Animations2D/RunRight.anim"));

		return controller;
	}
};

class TestApp : public Application 
{
public:

	GameObject* gameObjectFloor = nullptr;
	GameObject* gameObjectPlayer = nullptr;
	GameObject* gameObjectObstacle = nullptr;

	TestApp()
	{
		NewScene("Test scene");
		//for (size_t i = 0; i < 3; i++)
		//{
		//	for (size_t j = 0; j < 3; j++)
		//	{
		//		gameObjectFloor = GetScene()->CreateGameObject(std::to_string(i) + ":" + std::to_string(j));
		//		gameObjectFloor->m_Transform.Translate(glm::vec3(i, j, 0.0f));
		//		gameObjectFloor->m_ComponentManager.AddComponent<Renderer2D>()->SetTexture(TextureManager::GetInstance().Get("dirt"));
		//		gameObjectFloor->m_ComponentManager.AddComponent<BoxCollider2D>();
		//		gameObjectFloor->m_ComponentManager.AddComponent<Rigidbody2D>()->m_BodyType = Rigidbody2D::BodyType::Dynamic;
		//	}
		//}
		//gameObjectFloor = GetScene()->CreateGameObject("Floor");
		//gameObjectFloor->m_Transform.Translate(glm::vec3(0.0f, -0.0f, 0.0f));
		//gameObjectFloor->m_Transform.Scale(glm::vec3(50.0f, 1.0f, 1.0f));
		//gameObjectFloor->m_ComponentManager.AddComponent<Renderer2D>()->SetTexture(TextureManager::GetInstance().Get("dirt"));
		//gameObjectFloor->m_ComponentManager.AddComponent<BoxCollider2D>();
		//gameObjectFloor->m_ComponentManager.AddComponent<Rigidbody2D>()->m_BodyType = Rigidbody2D::BodyType::Static;

		//gameObjectObstacle = GetScene()->CreateGameObject("Obstacle");
		//gameObjectObstacle->m_Transform.Translate(glm::vec3(0.0f, 1.0f, 0.0f));
		//gameObjectObstacle->m_Transform.Scale(glm::vec3(1.0f, 1.0f, 1.0f));
		//gameObjectObstacle->m_ComponentManager.AddComponent<Renderer2D>();
		//gameObjectObstacle->m_ComponentManager.AddComponent<BoxCollider2D>();
		//gameObjectObstacle->m_ComponentManager.AddComponent<Rigidbody2D>()->m_BodyType = Rigidbody2D::BodyType::Dynamic;

		//gameObjectPlayer = GetScene()->CreateGameObject("Player");
		//gameObjectPlayer->m_Transform.Translate(glm::vec3(0.0f, 5.0f, 0.0f));
		//gameObjectPlayer->m_Transform.Scale(glm::vec3(1.0f, 1.0f, 1.0f));
		//gameObjectPlayer->m_ComponentManager.AddComponent<Renderer2D>();
		//gameObjectPlayer->m_ComponentManager.AddComponent<BoxCollider2D>();
		//gameObjectPlayer->m_ComponentManager.AddComponent<Rigidbody2D>()->m_BodyType = Rigidbody2D::BodyType::Dynamic;
		//gameObjectPlayer->m_ComponentManager.GetComponent<Rigidbody2D>()->m_FixedRotation = true;
		//gameObjectPlayer->m_ComponentManager.AddComponent<Animator2D>();
	}

	virtual void OnStart() override
	{
		//gameObjectFloor = GetScene()->FindGameObject("Floor");
		//gameObjectObstacle = GetScene()->FindGameObject("Obstacle");
		gameObjectPlayer = GetScene()->FindGameObject("Player");
		gameObjectPlayer->m_ComponentManager.AddComponent<Controller>();
	};
	
	virtual void OnUpdate() override
	{
		Environment::GetInstance().GetMainCamera()->m_Transform.Translate(gameObjectPlayer->m_Transform.GetPosition());
	};
};

#include "UI/Gui.h"
#include "Core/Window.h"
#include "Core/Viewport.h"

class UITest : public Application
{
public:

	int m_SelectedItem = -1;
	std::vector<std::wstring> m_Items;
	Texture* m_WolfTexture = nullptr;
	float m_Slider = 0.0f;
	bool m_Checkbox = false;

	UITest()
	{
		m_Items.reserve(100);
		for (size_t i = 0; i < 150; i++)
		{
			m_Items.push_back(L"Item " + std::to_wstring(i + 1));
		}
		TextureManager::GetInstance().AsyncGet([=](Texture* t) {
			m_WolfTexture = t;
			}, "Source/Images/Wolf.png");
	}

	virtual void OnGuiRender() override
	{
		Gui& gui = Gui::GetInstance();
		glm::vec2 mousePos = Viewport::GetInstance().GetUIMousePosition();
		gui.SetFont(gui.GetStandardFont(gui.ARIAL));
		gui.m_Theme.m_FontScale = 30.0f;
		gui.m_Theme.m_ButtonSize = { 200.0f, 100.0f };
		gui.RoundCorners(Gui::Edge::ALL);
		float tempFontScale = gui.m_Theme.m_FontScale;
		gui.m_Theme.m_FontScale *= 0.5f;
		gui.Text(L"Mouse pos X = %f; Y = %f", mousePos, mousePos.x, mousePos.y);
		gui.m_Theme.m_FontScale = tempFontScale;
		//gui.m_XAlign = gui.XCENTER;
		gui.BeginPanel(L"##TestPanel", { 0.0f, 0.0f }, { 1000.0f, 1000.0f });
		gui.Button(L"1", { 0.0f, 0.0f });
		gui.Button(L"2", { gui.m_Theme.m_ButtonSize + gui.m_RelatedPanelProperties.m_Padding });
		gui.Button(L"3", { (gui.m_Theme.m_ButtonSize + gui.m_RelatedPanelProperties.m_Padding) * 2.0f });
		gui.Button(L"4", { (gui.m_Theme.m_ButtonSize + gui.m_RelatedPanelProperties.m_Padding) * 3.0f });
		gui.m_RelatedPanelProperties.m_YOffset -= (gui.m_Theme.m_ButtonSize.y + gui.m_RelatedPanelProperties.m_Padding.y) * 3.0f;
		gui.SliderFloat(L"Slider", {0.0f, 0.0f}, { 0.0f,30.0f }, &gui.m_Theme.m_RoundedRadius);
		gui.CheckBox(L"Checkbox", { 0.0f, 0.0 }, &m_Checkbox);
		if (gui.CollapsingHeader(L"Collapsing header", { 0.0f, 0.0f }, 50.0f))
		{
			gui.Text(L"Test\nmultilines\ninput", { 0.0f, 0.0f });
		}
		gui.Image({ 0.0f, 0.0f }, TextureManager::GetInstance().Get("Wolf"));
		gui.ListBox(L"List", { 0.0f, 0.0f }, m_Items, m_SelectedItem);
		gui.EndPanel();
	}
};

class Heart : public Application
{
public:

	Heart()
	{

	}

	std::vector<std::vector<char>> heartElements = {
		{'0', '#', '0', '#', '0'},
		{'#', '#', '#', '#', '#'},
		{'#', '#', '#', '#', '#'},
		{'0', '#', '#', '#', '0'},
		{'0', '0', '#', '0', '0'}
	};

	virtual void OnStart() override
	{
		NewScene("Heart");
		GetScene();
		for (int i = 0; i < heartElements.size(); i++)
		{
			for (int j = 0; j < heartElements[i].size(); j++)
			{
				if (heartElements[i][j] == '#')
				{
					GameObject* heartElement = GetScene()->CreateGameObject();
					heartElement->m_Transform.Translate({ j , -i, 0.0f });
					heartElement->m_ComponentManager.AddComponent<Renderer2D>()->m_Color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
				}
			}
		}
	}

	virtual void OnGuiRender() override
	{

	}
};


class Test3D : public Application
{
public:

	Test3D()
	{

	}

	virtual void OnStart() override
	{
		NewScene("Test3D");
		GameObject* gameObject = GetScene()->CreateGameObject("3D");
		gameObject->m_ComponentManager.AddComponent<Renderer3D>()->SetMesh(MeshManager::GetInstance().Load("Source/Meshes/Test.obj"));
	}
};

int main()
{
	EntryPoint entrypoint;

	entrypoint.OnStart();
	UITest application;
	entrypoint.SetApplication(&application);
	entrypoint.OnUpdate();

	return 0;
}