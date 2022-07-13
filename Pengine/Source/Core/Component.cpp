#include "Component.h"

using namespace Pengine;

IComponent* IComponent::CreateCopy(GameObject* newOwner)
{
	IComponent* component = New(newOwner);
	*component = *this;

	return component;
}
