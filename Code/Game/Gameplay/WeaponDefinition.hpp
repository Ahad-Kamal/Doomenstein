#pragma once
#include "Engine/Math/FloatRange.hpp"


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

private:
	float m_refireTime = 0.f;
};