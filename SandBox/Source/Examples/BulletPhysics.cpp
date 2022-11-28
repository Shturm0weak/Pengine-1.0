#include "BulletPhysics.h"

#include "Core/Visualizer.h"
#include "Core/Raycast3D.h"
#include "Core/Environment.h"
#include "Core/Input.h"
#include "UI/Gui.h"

void BulletPhysics::OnStart()
{

}

void BulletPhysics::OnUpdate()
{
	if (Input::Mouse::IsMousePressed(Keycode::MOUSE_BUTTON_1))
	{
		auto camera = Environment::GetInstance().GetMainCamera();
		const glm::vec3 start = camera->m_Transform.GetPosition();
		const glm::vec3 direction = camera->GetMouseDirection();
		Raycast3D::Hit3D hit;
		Raycast3D::RayCast(GetScene()->GetBoxColliders3D(), start, direction, hit, 1000.0f);
		interesectedGameObject = hit.m_Object;

		if (hit.m_Object)
		{
			Rigidbody3D* rb3d = hit.m_Object->m_ComponentManager.GetComponent<Rigidbody3D>();
			rb3d->SetActive(true);
			glm::vec3 relpos = hit.m_Point - rb3d->GetCollisionShape()->GetPosition();
			rb3d->ApplyForce(direction * 10000.0f, relpos);
		}
	}
}

void BulletPhysics::OnClose()
{
	
}
