#include "Vehicle.h"

void Vehicle::Copy(const IComponent& component)
{
    Vehicle& vehicle = *(Vehicle*)&component;
    m_Type = component.GetType();
}

IComponent* Vehicle::New(GameObject* owner)
{
    return Create(owner);
}

IComponent* Vehicle::Create(GameObject* owner)
{
    Vehicle* vehicle = new Vehicle();
    return vehicle;
}

Vehicle::~Vehicle()
{
}

Vehicle::Vehicle(const Vehicle& vehicle)
{
    Copy(vehicle);
}

Vehicle& Vehicle::operator=(const Vehicle& vehicle)
{
    Copy(vehicle);

    return *this;
}
