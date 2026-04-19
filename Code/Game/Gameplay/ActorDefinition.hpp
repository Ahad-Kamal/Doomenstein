#pragma once
#include "Engine/Math/FloatRange.hpp"
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
typedef std::vector<std::string> Strings;

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
struct AIControl
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
	Strings GetWeapons() const;
	Collision GetCollision() const;
	Physics GetPhysics() const;
	CameraView GetCameraView() const;
	AIControl GetAI() const;
	int GetHealth() const;
	double GetCorpseLifetime() const;
	bool GetIsVisible() const;
	bool GetCanBePossesed() const;
	Faction GetFaction() const;

public:
	static std::vector<ActorDefinition> s_actorDefs;

private:
	Strings m_weapons;
	std::string m_name;
	Collision m_collision;
	Physics m_physics;
	CameraView m_cameraView;
	AIControl m_ai;
	Faction m_faction = Faction::NEUTRAL;
	int m_health = 1;
	double m_corpseLifetime = 0.0;
	bool m_isVisible = false;
	bool m_canBePossessed = false;
};