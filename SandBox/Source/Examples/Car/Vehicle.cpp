#include "Vehicle.h"

void Vehicle::Copy(const IComponent& component)
{
    Vehicle& vehicle = *(Vehicle*)&component;
    m_Type = component.GetType();
}

void Vehicle::Move(IComponent&& component)
{
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

Vehicle::Vehicle(Vehicle&& vehicle) noexcept
{
    Move(std::move(*(IComponent*)&vehicle));
}

Vehicle& Vehicle::operator=(const Vehicle& vehicle)
{
    Copy(vehicle);

    return *this;
}

Vehicle& Vehicle::operator=(Vehicle&& vehicle) noexcept
{
    Move(std::move(*(IComponent*)&vehicle));

    return *this;
}
