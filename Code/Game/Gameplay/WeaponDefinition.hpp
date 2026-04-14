#pragma once
#include "Engine/Math/FloatRange.hpp"
#include <string>
#include <vector>


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
class WeaponDefinition
{
public:
	static void InitializeWeaponDefs();
	static void ClearWeaponDefs();

	std::string GetName() const;
	WeaponType GetType() const;

public:
	static std::vector<WeaponDefinition> s_weaponDefs;

private:
	RayWeapon m_ray;
	ProjectileWeapon m_projectile;
	MeleeWeapon m_melee;
	std::string m_name;
	float m_refireTime = 0.f;
	WeaponType m_type;
};