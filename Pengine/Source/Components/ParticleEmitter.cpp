#include "ParticleEmitter.h"

#include "../Core/Scene.h"
#include "../Core/ThreadPool.h"
#include "../Core/Environment.h"
#include "../EventSystem/EventSystem.h"
#include "../OpenGL/Batch.h"
#include "../Events/OnMainThreadCallback.h"

using namespace Pengine;

void ParticleEmitter::Delete()
{
	Utils::Erase(m_Owner->GetScene()->m_ParticleEmitters, this);
	delete this;
}

void ParticleEmitter::OnStart()
{
	m_MeshVertexAttributes = MeshManager::GetInstance().Get("Quad")->GetVertexAttributes();

	m_Particles.resize(m_ParticlesSize);
	std::random_device random;
	std::mt19937 engine(random());
	m_ParticlesFinished = m_ParticlesSize;

	for (size_t i = 0; i < m_ParticlesSize; i++)
	{
		m_Particles[i].Initialize(*this, engine);
	}
}

void ParticleEmitter::OnUpdate()
{
	if (!m_Owner->IsEnabled()) return;

	ThreadPool::GetInstance().Enqueue([=]()
	{
		std::random_device random;
		std::mt19937 engine(random());

		size_t size = m_Particles.size();
		for (size_t i = 0; i < size; i++)
		{
			if (m_Particles[i].m_TimeToSpawn > 0)
			{
				m_Particles[i].m_TimeToSpawn -= Time::GetDeltaTime();
				continue;
			}
			else
			{
				m_Particles[i].m_IsEnabled = true;
			}

			m_Particles[i].m_Speed += m_Gravity * Time::GetDeltaTime();
			m_Particles[i].m_Position += m_Particles[i].m_Speed * Time::GetDeltaTime();
			m_Particles[i].m_TimeLiving += Time::GetDeltaTime();
			m_Particles[i].m_Scale = glm::vec3(m_Particles[i].m_Scale * (1.0f - (0.99f * Time::GetDeltaTime() * m_Acceleration)));
			
			if (m_Particles[i].m_TimeLiving > m_MaxTimeToLive)
			{
				m_Particles[i].m_IsEnabled = false;
				m_Particles[i].m_TimeToSpawn = 1000000.0f;
				m_ParticlesFinished++;
				if (m_Loop)
				{
					m_Particles[i].Initialize(*this, engine);
				}

				if(m_ParticlesFinished == m_ParticlesSize)
				{
					for (auto& callback : m_EndCallbacks)
					{
						EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
					}
				}
			}
		}
	});
}

void ParticleEmitter::OnClose()
{
	m_Particles.clear();
}

IComponent* ParticleEmitter::New(GameObject* owner)
{
	return Create(owner);
}

void ParticleEmitter::Copy(const IComponent& component)
{
	ParticleEmitter& particleEmitter = *(ParticleEmitter*)&component;
	m_ColorStart = particleEmitter.m_ColorStart;
	m_ColorEnd = particleEmitter.m_ColorEnd;
	m_Direction = particleEmitter.m_Direction;
	m_RadiusToSpawn = particleEmitter.m_RadiusToSpawn;
	m_Scale = particleEmitter.m_Scale;
	m_TimeToSpawn = particleEmitter.m_TimeToSpawn;
	m_MaxTimeToLive = particleEmitter.m_MaxTimeToLive;
	m_Acceleration = particleEmitter.m_Acceleration;
	m_Texture = particleEmitter.m_Texture;
	m_ParticlesSize = particleEmitter.m_ParticlesSize;
	m_Gravity = particleEmitter.m_Gravity;
	m_Loop = particleEmitter.m_Loop;
	m_Intensity = particleEmitter.m_Intensity;
	m_FacingMode = particleEmitter.m_FacingMode;
	m_Type = component.GetType();
}

void ParticleEmitter::Render()
{
	size_t size = m_Particles.size();
	for (size_t i = 0; i < size; i++)
	{
		if (!m_Particles[i].m_IsEnabled) continue;

		glm::mat4 transform;

		switch (m_FacingMode)
		{
		case FacingMode::DEFAULT:
		{
			transform = glm::translate(glm::mat4(1.0f), m_Owner->m_Transform.GetPosition() + m_Particles[i].m_Position) * glm::scale(glm::mat4(1.0f), m_Particles[i].m_Scale);
			break;
		}
		case FacingMode::SPHERICAL:
		{
			const glm::vec3 position = m_Owner->m_Transform.GetPosition() + m_Particles[i].m_Position;
			transform = glm::translate(glm::mat4(1.0f), position) *
				glm::scale(glm::mat4(1.0f), m_Particles[i].m_Scale) *
				glm::inverse(glm::lookAt(glm::vec3(0.0f), Environment::GetInstance().GetMainCamera()->m_Transform.GetPosition()
					- position, glm::vec3(0.0f, 1.0f, 0.0f)));
			break;
		}
		case FacingMode::CYLINDRICAL:
		{
			const glm::vec3 cameraPosition = Environment::GetInstance().GetMainCamera()->m_Transform.GetPosition();
			const glm::vec3 position = m_Owner->m_Transform.GetPosition() + m_Particles[i].m_Position;
			transform = glm::translate(glm::mat4(1.0f), position) *
				glm::scale(glm::mat4(1.0f), m_Particles[i].m_Scale) *
				glm::inverse(glm::lookAt(glm::vec3(0.0f), glm::vec3(cameraPosition.x, position.y, cameraPosition.z) - position, glm::vec3(0.0f, 1.0f, 0.0f)));
			break;
		}
		default:
			break;
		}

		Batch::GetInstance().Submit(m_MeshVertexAttributes, transform, m_Particles[i].m_Color, { m_Particles[i].m_Texture }, { 0.0f, 0.0f, 0.0f, m_Intensity });
	}
}

ParticleEmitter::ParticleEmitter()
{
	EventSystem::GetInstance().RegisterClient(EventType::ONSTART, this);
	EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, this);
	EventSystem::GetInstance().RegisterClient(EventType::ONCLOSE, this);
}

ParticleEmitter::~ParticleEmitter()
{
	EventSystem::GetInstance().UnregisterAll(this);
}

IComponent* ParticleEmitter::Create(GameObject* owner)
{
	ParticleEmitter* particleEmitter = new ParticleEmitter();
	owner->GetScene()->m_ParticleEmitters.push_back(particleEmitter);

	return particleEmitter;
}

void ParticleEmitter::Particle::Initialize(ParticleEmitter& particleEmitter, std::mt19937& engine)
{	
	float radiusMax = glm::max<float>(particleEmitter.m_RadiusToSpawn[0].x, particleEmitter.m_RadiusToSpawn[0].y);
	float radiusMin = glm::min<float>(particleEmitter.m_RadiusToSpawn[0].x, particleEmitter.m_RadiusToSpawn[0].y);
	std::uniform_real_distribution<float> radiusX(radiusMin, radiusMax);

	radiusMax = glm::max<float>(particleEmitter.m_RadiusToSpawn[1].x, particleEmitter.m_RadiusToSpawn[1].y);
	radiusMin = glm::min<float>(particleEmitter.m_RadiusToSpawn[1].x, particleEmitter.m_RadiusToSpawn[1].y);
	std::uniform_real_distribution<float> radiusY(radiusMin, radiusMax);

	radiusMax = glm::max<float>(particleEmitter.m_RadiusToSpawn[2].x, particleEmitter.m_RadiusToSpawn[2].y);
	radiusMin = glm::min<float>(particleEmitter.m_RadiusToSpawn[2].x, particleEmitter.m_RadiusToSpawn[2].y);
	std::uniform_real_distribution<float> radiusZ(radiusMin, radiusMax);

	m_Position = glm::vec3(radiusX(engine), radiusY(engine), radiusZ(engine));
	
	float timeToSpawnMax = glm::max<float>(particleEmitter.m_TimeToSpawn.x, particleEmitter.m_TimeToSpawn.y);
	float timeToSpawnMin = glm::min<float>(particleEmitter.m_TimeToSpawn.x, particleEmitter.m_TimeToSpawn.y);
	std::uniform_real_distribution<float> timeToSpawn(timeToSpawnMin, timeToSpawnMax);

	m_TimeToSpawn = timeToSpawn(engine);
	
	float scaleMax = glm::max<float>(particleEmitter.m_Scale.x, particleEmitter.m_Scale.y);
	float scaleMin = glm::min<float>(particleEmitter.m_Scale.x, particleEmitter.m_Scale.y);
	std::uniform_real_distribution<float> scale(scaleMin, scaleMax);

	m_Scale = glm::vec3(scale(engine));
	
	float directionMax = glm::max<float>(particleEmitter.m_Direction[0].x, particleEmitter.m_Direction[0].y);
	float directionMin = glm::min<float>(particleEmitter.m_Direction[0].x, particleEmitter.m_Direction[0].y);
	std::uniform_real_distribution<float> directionX(directionMin, directionMax);

	directionMax = glm::max<float>(particleEmitter.m_Direction[1].x, particleEmitter.m_Direction[1].y);
	directionMin = glm::min<float>(particleEmitter.m_Direction[1].x, particleEmitter.m_Direction[1].y);
	std::uniform_real_distribution<float> directionY(directionMin, directionMax);

	directionMax = glm::max<float>(particleEmitter.m_Direction[2].x, particleEmitter.m_Direction[2].y);
	directionMin = glm::min<float>(particleEmitter.m_Direction[2].x, particleEmitter.m_Direction[2].y);
	std::uniform_real_distribution<float> directionZ(directionMin, directionMax);

	m_Direction = glm::vec3(directionX(engine), directionY(engine), directionZ(engine));

	float colorComponentMax = glm::max<float>(particleEmitter.m_ColorStart.x, particleEmitter.m_ColorEnd.x);
	float colorComponentMin = glm::min<float>(particleEmitter.m_ColorStart.x, particleEmitter.m_ColorEnd.x);
	std::uniform_real_distribution<float> rColor(colorComponentMin, colorComponentMax);

	colorComponentMax = glm::max<float>(particleEmitter.m_ColorStart.y, particleEmitter.m_ColorEnd.y);
	colorComponentMin = glm::min<float>(particleEmitter.m_ColorStart.y, particleEmitter.m_ColorEnd.y);
	std::uniform_real_distribution<float> gColor(colorComponentMin, colorComponentMax);

	colorComponentMax = glm::max<float>(particleEmitter.m_ColorStart.z, particleEmitter.m_ColorEnd.z);
	colorComponentMin = glm::min<float>(particleEmitter.m_ColorStart.z, particleEmitter.m_ColorEnd.z);
	std::uniform_real_distribution<float> bColor(colorComponentMin, colorComponentMax);

	colorComponentMax = glm::max<float>(particleEmitter.m_ColorStart.w, particleEmitter.m_ColorEnd.w);
	colorComponentMin = glm::min<float>(particleEmitter.m_ColorStart.w, particleEmitter.m_ColorEnd.w);
	std::uniform_real_distribution<float> aColor(colorComponentMin, colorComponentMax);

	m_Color = glm::vec4(rColor(engine), gColor(engine), bColor(engine), aColor(engine));

	m_TimeLiving = 0.0f;
	m_Texture = particleEmitter.m_Texture;
	m_Speed = m_Direction * particleEmitter.m_Acceleration;

	particleEmitter.m_ParticlesFinished--;
}
