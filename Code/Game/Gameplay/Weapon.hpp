#pragma once


//-----------------------------------------------------------------------------------------------
class WeaponDefinition;
class ActorDefinition;
class Actor;
struct Vec3;

//-----------------------------------------------------------------------------------------------
class Weapon
{
public:
	Weapon( WeaponDefinition* definition );

	void Fire( Actor* owner );
	Vec3 GetRandomDirectionInCone() const;

public:
	WeaponDefinition* m_definition;
	ActorDefinition* m_projectileDefinition;
};