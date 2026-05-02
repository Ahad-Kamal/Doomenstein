#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Game/Gameplay/Actors/Actor.hpp"
#include <string>
#include <vector>
#include "Engine/Renderer/SpriteAnimDefinition.hpp"


//-----------------------------------------------------------------------------------------------
class SpriteSheet;

//-----------------------------------------------------------------------------------------------
enum WeaponType
{
	WEAPON_TYPE_RAY,
	WEAPON_TYPE_PROJECTILE,
	WEAPON_TYPE_MELEE
};

//-----------------------------------------------------------------------------------------------
struct RayWeapon
{
	FloatRange m_rayDamage = FloatRange( 0.f, 0.f );
	int m_rayCount = 0;
	float m_rayCone = 0.f;
	float m_rayRange = 0.f;
	float m_rayImpulse = 0.f;
};

//-----------------------------------------------------------------------------------------------
struct ProjectileWeapon
{
	std::string m_projectileActor;
	int m_projectileCount = 0;
	float m_projectileCone = 0.f;
	float m_projectileSpeed = 0.f;
};

//-----------------------------------------------------------------------------------------------
struct MeleeWeapon
{
	FloatRange m_meleeDamage = FloatRange( 0.f, 0.f );
	int m_meleeCount = 0;
	float m_meleeRange = 0.f;
	float m_meleeArc = 0.f;
	float m_meleeImpulse = 0.f;
};

//-----------------------------------------------------------------------------------------------
struct HUD
{
	std::string m_shader;
	std::string m_baseTexture;
	std::string m_reticleTexture;
	Vec2 m_reticleSize = Vec2( 1.f, 1.f );
	Vec2 m_spriteSize = Vec2( 1.f, 1.f );
	Vec2 m_spritePivot = Vec2( 0.5f, 0.f );
};

//-----------------------------------------------------------------------------------------------
struct WeaponAnimation
{
	SpriteAnimDefinition* m_animDef = nullptr;
	std::string m_name;
	std::string m_shader;
	SpriteSheet* m_spriteSheet = nullptr;
	IntVec2 m_cellCount = IntVec2( 1, 1 );
	float m_framesPerSecond = 1.f;
	int m_startFrame = 0;
	int m_endFrame = 0;
};

//-----------------------------------------------------------------------------------------------
class WeaponDefinition
{
public:
	static void InitializeWeaponDefs();
	static void ClearWeaponDefs();

	std::string GetName() const;
	WeaponType GetType() const;
	float GetRefireTime() const;
	RayWeapon GetRayWeaponInfo() const;
	ProjectileWeapon GetProjectileWeaponInfo() const;
	MeleeWeapon GetMeleeWeaponInfo() const;
	HUD GetHud() const;
	WeaponAnimation* GetAnimationByName( std::string const& animName );
	WeaponAnimation* GetAnimationByState( AnimState animState );
	float GetAnimationDuration( AnimState animState );

public:
	static std::vector<WeaponDefinition> s_weaponDefs;

private:
	RayWeapon m_ray;
	ProjectileWeapon m_projectile;
	MeleeWeapon m_melee;
	std::string m_name;
	float m_refireTime = 0.f;
	WeaponType m_type;
	HUD m_hud;
	std::vector<WeaponAnimation> m_animations;
};