#include "Game/Gameplay/Weapon.hpp"
#include "Game/Gameplay/WeaponDefinition.hpp"
#include "Game/Gameplay/Actors/Actor.hpp"
#include "Engine/Math/Vec3.hpp"


//-----------------------------------------------------------------------------------------------
Weapon::Weapon( WeaponDefinition* definition )
	: m_definition( definition )
{
}

//-----------------------------------------------------------------------------------------------
void Weapon::Fire( Actor* owner )
{

}

//-----------------------------------------------------------------------------------------------
Vec3 Weapon::GetRandomDirectionInCone() const
{
	return Vec3();
}

