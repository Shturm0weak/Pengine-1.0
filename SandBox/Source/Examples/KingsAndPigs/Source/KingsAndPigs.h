#pragma once

#include "Core/Core.h"
#include "Core/Application.h"
#include "Audio/SoundManager.h"
#include "Core/Visualizer.h"
#include "Core/Viewport.h"

#include "glm/gtx/spline.hpp"
#define GLM_GTX_spline

#include <vector>

using namespace Pengine;

class Path
{
private:

	std::vector<GameObject*> m_Points;
	std::vector<float> m_Lengths;
public:

	float GetLength() const
	{
		float totalLength = 0.0f;

		for (auto length : m_Lengths)
		{
			totalLength += length;
		}
		return totalLength;
	}

	float GetNormalizedOffset(float t)  const
	{
		int i = 0;
		while (t > m_Lengths[i])
		{
			t -= m_Lengths[i];
			i++;
		}

		return (float)i + (t / m_Lengths[i]);
	}

	glm::vec3 GetPoint(float t) const
	{
		int i = (int)t;
		t -= i;

		GameObject* currentPoint = m_Points[i];

		int nextI = i + 1;
		if (nextI == m_Points.size())
		{
			nextI = 0;
		}

		GameObject* nextPoint = m_Points[nextI];

		int nextNextI = i + 2;
		if (nextNextI > m_Points.size() - 1)
		{
			nextNextI = nextNextI - m_Points.size();
		}

		GameObject* nextNextPoint = m_Points[nextNextI];

		int previousI = i - 1;
		if (previousI < 0)
		{
			previousI = m_Points.size() - 1;
		}

		GameObject* previousPoint = m_Points[previousI];

		return glm::catmullRom(previousPoint->m_Transform.GetPosition(), currentPoint->m_Transform.GetPosition(),
			nextPoint->m_Transform.GetPosition(), nextNextPoint->m_Transform.GetPosition(), t);
	}

	void Initialize(std::vector<GameObject*> points)
	{
		m_Points = points;
	}

	void CalculateLengths()
	{
		m_Lengths.clear();
		m_Lengths.resize(m_Points.size());

		for (int i = 0; i < m_Points.size(); i++)
		{
			glm::vec3 position = { 0.0f, 0.0f, 0.0f };
			glm::vec3 previousPosition = { 0.0f, 0.0f, 0.0f };
			for (float j = 0.0f; j < 1.0f; j += 0.01f)
			{
				previousPosition = position;
				position = GetPoint(i + j);

				if (!Utils::IsEqual(previousPosition, { 0.0f, 0.0f, 0.0f }))
				{
					m_Lengths[i] += glm::distance(position, previousPosition);
				}
			}
		}
	}

	void Visualize()
	{
		for (int i = 0; i < m_Points.size(); i++)
		{
			glm::vec3 position = { 0.0f, 0.0f, 0.0f };
			glm::vec3 previousPosition = { 0.0f, 0.0f, 0.0f };
			for (float j = 0.0f; j < 1.0f; j += 0.01f)
			{
				previousPosition = position;
				position = GetPoint(i + j);

				if (!Utils::IsEqual(previousPosition, { 0.0f, 0.0f, 0.0f }))
				{
					Visualizer::DrawLine(previousPosition, position, glm::vec4(1.0f, 0.0f, 0.0f, 1.0));
				}
			}

			Visualizer::DrawQuad(m_Points[i]->m_Transform.GetTransform());
		}
	}
};

class Agent
{
private:

	GameObject* m_GameObject = nullptr;

	float m_Speed = 0.0f;
	float m_PositionAlongPath = 0.0f;
public:

	void Initialize(GameObject* gameObject, float speed)
	{
		m_GameObject = gameObject;
		m_Speed = speed;
	}

	void Move(const Path& path)
	{
		m_PositionAlongPath += m_Speed * Time::GetDeltaTime();

		if (m_PositionAlongPath > path.GetLength())
		{
			m_PositionAlongPath = 0;
		}

		m_GameObject->m_Transform.Translate(path.GetPoint(path.GetNormalizedOffset(m_PositionAlongPath)));
	}
};

class KingsAndPigs : public Application
{
private:

	//Path m_Path;
	//Agent m_Agent;

	Sound* m_AmbientSound = nullptr;
public:

	KingsAndPigs(const std::string& title = "KingsAndPigs")
		: Application(title)
	{
		//m_AmbientSound = SoundManager::GetInstance().Load("Source/Examples/KingsAndPigs/Sounds/Ambient.ogg");
	}

	virtual void OnStart() override 
	{
		//m_AmbientSound->SetVolume(0.05f);
		//SoundManager::GetInstance().Loop(m_AmbientSound);

		//m_Path.Initialize(GetScene()->FindGameObjects("Point"));
		//m_Agent.Initialize(GetScene()->FindGameObject("Agent"), 10.0f);

		/*for (size_t i = 0; i < 316; i++)
		{
			for (size_t j = 0; j < 316; j++)
			{
				GameObject* gameObject = GetScene()->CreateGameObject();
				gameObject->m_Transform.Translate({ i * 2.0f, j * 2.0f, 0.0f });
				MeshManager::GetInstance().LoadAsync("Source/Meshes/Cube.obj", 
					[gameObject](Mesh* mesh)
				{
						gameObject->m_ComponentManager.AddComponent<Renderer3D>()->SetMesh(mesh);
				}
				);
			}
		}*/
	};

	virtual void OnPostStart() override {};

	virtual void OnUpdate() override 
	{
		/*glm::vec2 normalizedMousePosition = Viewport::GetInstance().GetNormalizedMousePosition();
		printf_s("%f %f\n", normalizedMousePosition.x, normalizedMousePosition.y);*/
		//m_Path.Visualize();
		//m_Path.CalculateLengths();
		//m_Agent.Move(m_Path);
	};

	virtual void OnGuiRender() override {};

	virtual void OnImGuiRender() override {};

	virtual void OnClose() override 
	{
		//SoundManager::GetInstance().Stop(m_AmbientSound);
	};
};