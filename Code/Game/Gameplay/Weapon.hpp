#pragma once


//-----------------------------------------------------------------------------------------------
class WeaponDefinition;
class ActorDefinition;
class Actor;
struct Vec3;
struct EulerAngles;
class Timer;

//-----------------------------------------------------------------------------------------------
class Weapon
{
public:
	Weapon( WeaponDefinition* definition );

	void Fire( Actor* owner );
	EulerAngles GetRandomDirectionInCone( EulerAngles const& orientation, float yawOffset, float pitchOffset ) const;

public:
	WeaponDefinition* m_definition;
	ActorDefinition* m_projectileDefinition = nullptr;
	Timer* m_cooldownTimer;
};