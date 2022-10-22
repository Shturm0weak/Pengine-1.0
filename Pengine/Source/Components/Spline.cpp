#include "Spline.h"

#include "../Core/Utils.h"
#include "../Core/Visualizer.h"
#include "../Core/GameObject.h"
#include "../EventSystem/EventSystem.h"

#include "glm/gtx/spline.hpp"
#define GLM_GTX_spline

using namespace Pengine;

IComponent* Spline::New(GameObject* owner)
{
    return Create(owner);
}
void Spline::Copy(const IComponent& component)
{
    Spline& spline = *(Spline*)&component;
	m_Lengths = spline.m_Lengths;
	m_Speed = spline.m_Speed;
    m_Type = component.GetType();
}

void Spline::Delete()
{
	delete this;
}

Spline::~Spline()
{
	EventSystem::GetInstance().UnregisterAll(this);
}

void Spline::OnRegisterClient()
{
	EventSystem::GetInstance().RegisterClient(EventType::ONSTART, this);
	EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, this);
}

IComponent* Spline::Create(GameObject* owner)
{
    return new Spline();
}

void Spline::OnStart()
{
}

void Spline::OnUpdate()
{
	m_Points = m_Owner->GetChilds();
	CalculateLengths();

	if (m_Visualize)
	{
		Visualize();
	}
}

float Spline::GetLength() const
{
	float totalLength = 0.0f;

	for (auto length : m_Lengths)
	{
		totalLength += length;
	}
	return totalLength;
}

float Spline::GetNormalizedOffset(float& t) const
{
	if (t >= GetLength())
	{
		t = 0.0f;
	}

	int i = 0;
	float normalizedT = t;
	while (normalizedT > m_Lengths[i])
	{
		normalizedT -= m_Lengths[i];
		i++;
	}

	return (float)i + (normalizedT / m_Lengths[i]);
}

glm::vec3 Spline::GetPoint(float t) const
{
	int p0, p1, p2, p3;
	
	p1 = (int)t;
	p2 = (p1 + 1) % m_Points.size();
	p3 = (p2 + 1) % m_Points.size();
	p0 = p1 >= 1 ? p1 - 1 : m_Points.size() - 1;

	t = t - (int)t;

	float tt = t * t;
	float ttt = tt * t;

	float q1 = -ttt + 2.0f * tt - t;
	float q2 = 3.0f * ttt - 5.0f * tt + 2.0f;
	float q3 = -3.0f * ttt + 4.0f * tt + t;
	float q4 = ttt - tt;

	float tx = 0.5f * (m_Points[p0]->m_Transform.GetPosition().x * q1 + m_Points[p1]->m_Transform.GetPosition().x * q2 + m_Points[p2]->m_Transform.GetPosition().x * q3 + m_Points[p3]->m_Transform.GetPosition().x * q4);
	float ty = 0.5f * (m_Points[p0]->m_Transform.GetPosition().y * q1 + m_Points[p1]->m_Transform.GetPosition().y * q2 + m_Points[p2]->m_Transform.GetPosition().y * q3 + m_Points[p3]->m_Transform.GetPosition().y * q4);
	float tz = 0.5f * (m_Points[p0]->m_Transform.GetPosition().z * q1 + m_Points[p1]->m_Transform.GetPosition().z * q2 + m_Points[p2]->m_Transform.GetPosition().z * q3 + m_Points[p3]->m_Transform.GetPosition().z * q4);

	return{ tx, ty, tz };

	/*if (m_Points.size() == 0)
	{
		return { 0.0f, 0.0f, 0.0f };
	}

	if (m_Points.size() == 1)
	{
		return m_Points[0]->m_Transform.GetPosition();
	}

	int i = (int)t;
	t -= (float)i;

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
		nextPoint->m_Transform.GetPosition(), nextNextPoint->m_Transform.GetPosition(), t);*/
}

void Spline::CalculateLengths()
{
	m_Lengths.resize(m_Points.size());

	for (int i = 0; i < m_Points.size(); i++)
	{
		m_Lengths[i] = 0.0f;

		glm::vec3 previousPosition = GetPoint((float)i);
		for (float j = 0.01f; j < 1.0f; j += 0.01f)
		{
			glm::vec3 position = GetPoint((float)i + j);

			m_Lengths[i] += glm::distance(previousPosition, position);

			previousPosition = position;
		}
	}
}

void Spline::Visualize()
{
	for (int i = 0; i < m_Points.size(); i++)
	{
		glm::vec3 previousPosition = GetPoint((float)i);
		for (float j = 0.005f; j < 1.0f; j += 0.005f)
		{
			glm::vec3 position = GetPoint((float)i + j);

			Visualizer::DrawLine(previousPosition, position, glm::vec4(1.0f, 0.0f, 0.0f, 1.0));

			previousPosition = position;
		}

		//Visualizer::DrawQuad(m_Points[i]->m_Transform.GetTransform());
	}
}
