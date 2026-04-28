#pragma once
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
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
struct Animation
{
	Vec3 m_vector = Vec3( 1.f, 0.f, 0.f );
	int m_startFrame = 0;
	int m_endFrame = 0;
};

//-----------------------------------------------------------------------------------------------
struct AnimationGroup
{
	std::vector<Animation> m_animations;
	std::vector<SpriteAnimDefinition> m_spriteAnimDefs;
	std::string m_name;
	float m_secondsPerFrame = 1.f;
	SpriteAnimPlaybackType m_playbackMode = SpriteAnimPlaybackType::ONCE;
};

//-----------------------------------------------------------------------------------------------
struct Visuals
{
	std::vector<AnimationGroup> m_animationGroups;
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
	Faction m_faction = Faction::NEUTRAL;
	int m_health = 1;
	double m_corpseLifetime = 0.0;
	bool m_isVisible = false;
	bool m_canBePossessed = false;
	bool m_dieOnSpawn = false;
};