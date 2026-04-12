#pragma once
#include "Engine/Math/FloatRange.hpp"
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
enum class Faction
{
	NEUTRAL,
	MARINE,
	DEMON
};

//-----------------------------------------------------------------------------------------------
struct Collision
{
	FloatRange m_damageOnCollide = FloatRange( 0.f, 0.f );
	float m_impulseOnCollide = 0.f;
	float m_physicsRadius = 0.f;
	float m_physicsHeight = 0.f;
	bool m_collidesWithWorld = false;
	bool m_collidesWithActors = false;
	bool m_dieOnCollide = false;
};

//-----------------------------------------------------------------------------------------------
struct Physics
{
	float m_walkSpeed = 0.f;
	float m_runSpeed = 0.f;
	float m_drag = 0.f;
	float m_turnSpeed = 0.f;
	bool m_isSimulated = false;
	bool m_isFlying = false;
};

//-----------------------------------------------------------------------------------------------
struct CameraView
{
	float m_eyeHeight = 0.f;
	float m_cameraFOVDegrees = 60.f; 
};

//-----------------------------------------------------------------------------------------------
struct AI
{
	float m_sightRadius = 0.f;
	float m_sightAngle = 0.f;
	bool m_aiEnabled = false;
};

//-----------------------------------------------------------------------------------------------
class ActorDefinition
{
public:
	static void InitializeActorDefs();
	static void InitializeProjectileActorDefs();
	static void ClearActorDefs();

	std::string GetName() const;
	std::string GetWeapon() const;
	Collision GetCollision() const;
	Physics GetPhysics() const;
	CameraView GetCameraView() const;
	AI GetAI() const;
	int GetHealth() const;
	float GetCorpseLifetime() const;
	bool GetIsVisible() const;
	bool GetCanBePossesed() const;

public:
	static std::vector<ActorDefinition> s_actorDefs;

private:
	std::string m_name;
	std::string m_weapon;
	Collision m_collision;
	Physics m_physics;
	CameraView m_cameraView;
	AI m_ai;
	Faction m_faction = Faction::NEUTRAL;
	int m_health = 1;
	float m_corpseLifetime = 0.f;
	bool m_isVisible = false;
	bool m_canBePossessed = false;
};