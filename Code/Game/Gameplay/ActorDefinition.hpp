#pragma once
#include "Game/Framework/SpriteAnimGroupDefinition.hpp"
#include "Game/Gameplay/Actors/Actor.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include <string>
#include <vector>
#include <map>


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
struct Visuals
{
	std::vector<SpriteAnimationGroupDefinition> m_animGroupDefs;
	std::string m_shader;
	SpriteSheet* m_spriteSheet = nullptr;
	Vec2 m_size = Vec2( 1.f, 1.f );
	Vec2 m_pivot = Vec2( 0.5f, 0.5f );
	IntVec2 m_cellCount = IntVec2( 1, 1 );
	BillboardType m_billboardType = BillboardType::NONE;
	bool m_renderLit = false;
	bool m_renderRounded = false;
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
	Visuals GetVisuals() const;
	int GetHealth() const;
	double GetCorpseLifetime() const;
	bool GetIsVisible() const;
	bool GetCanBePossesed() const;
	bool GetDieOnSpawn() const;
	Faction GetFaction() const;
	const SpriteAnimationGroupDefinition* GetAnimGroupByName( std::string const& animName );
	const SpriteAnimationGroupDefinition* GetAnimGroupByState( AnimState animState );

public:
	static std::vector<ActorDefinition> s_actorDefs;

private:
	Strings m_weapons;
	std::string m_name;
	Collision m_collision;
	Physics m_physics;
	CameraView m_cameraView;
	AIControl m_ai;
	Visuals m_visuals;
	std::map< std::string, std::string > m_soundEffects;
	Faction m_faction = Faction::NEUTRAL;
	int m_health = 1;
	double m_corpseLifetime = 0.0;
	bool m_isVisible = false;
	bool m_canBePossessed = false;
	bool m_dieOnSpawn = false;
};