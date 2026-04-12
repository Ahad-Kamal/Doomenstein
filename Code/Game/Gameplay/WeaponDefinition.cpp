#include "Game/Gameplay/WeaponDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/StringUtils.hpp"


//-----------------------------------------------------------------------------------------------
std::vector<WeaponDefinition> WeaponDefinition::s_weaponDefs;

//-----------------------------------------------------------------------------------------------
void WeaponDefinition::InitializeWeaponDefs()
{
	// Read in WeaponDefinitions.xml
	XmlDocument weaponDefs;
	weaponDefs.LoadFile( "Data/Definitions/WeaponDefinitions.xml" );
	int numWeaponDefs = weaponDefs.FirstChildElement()->ChildElementCount();
	s_weaponDefs.resize( numWeaponDefs );

	XmlElement* currentElement = weaponDefs.FirstChildElement()->FirstChildElement();
	for( int elementIndex = 0; elementIndex < numWeaponDefs; elementIndex++ )
	{
		NamedStrings weaponDefBlackboard;
		weaponDefBlackboard.PopulateFromXmlElementAttributes( *currentElement );
		WeaponDefinition& currentWeaponDef = s_weaponDefs[ elementIndex ];

		// Basic Info
		currentWeaponDef.m_name = weaponDefBlackboard.GetValue( "name", "" );
		currentWeaponDef.m_refireTime = weaponDefBlackboard.GetValue( "refireTime", 0.f );

		// Ray Weapon
		currentWeaponDef.m_ray.m_rayCount = weaponDefBlackboard.GetValue( "rayCount", 0 );
		currentWeaponDef.m_ray.m_rayCone = weaponDefBlackboard.GetValue( "rayCone", 0.f );
		currentWeaponDef.m_ray.m_rayRange = weaponDefBlackboard.GetValue( "rayRange", 0.f );
		currentWeaponDef.m_ray.m_rayImpulse = weaponDefBlackboard.GetValue( "rayImpulse", 0.f );
		std::string rayDamage = weaponDefBlackboard.GetValue( "rayDamage", "0.0~0.0" );
		Strings rayDamageValues = SplitStringOnDelimiter( rayDamage, '~' );
		currentWeaponDef.m_ray.m_rayDamage = FloatRange( stof( rayDamageValues[ 0 ] ), stof( rayDamageValues[ 1 ] ) );

		// Projectile Weapon
		currentWeaponDef.m_projectile.m_projectileActor = weaponDefBlackboard.GetValue( "projectileActor", "" );
		currentWeaponDef.m_projectile.m_projectileCount = weaponDefBlackboard.GetValue( "projectileCount", 0 );
		currentWeaponDef.m_projectile.m_projectileCone = weaponDefBlackboard.GetValue( "projectileCone", 0.f );
		currentWeaponDef.m_projectile.m_projectileSpeed = weaponDefBlackboard.GetValue( "projectileSpeed", 0.f );
		
		// Melee Weapon
		currentWeaponDef.m_melee.m_meleeCount = weaponDefBlackboard.GetValue( "meleeCount", 0 );
		currentWeaponDef.m_melee.m_meleeRange = weaponDefBlackboard.GetValue( "meleeRange", 0.f );
		currentWeaponDef.m_melee.m_meleeArc = weaponDefBlackboard.GetValue( "meleeArc", 0.f );
		currentWeaponDef.m_melee.m_meleeImpulse = weaponDefBlackboard.GetValue( "meleeImpulse", 0.f );
		std::string meleeDamage = weaponDefBlackboard.GetValue( "meleeDamage", "0.0~0.0" );
		Strings meleeDamageValues = SplitStringOnDelimiter( meleeDamage, '~' );
		currentWeaponDef.m_melee.m_meleeDamage = FloatRange( stof( meleeDamageValues[ 0 ] ), stof( meleeDamageValues[ 1 ] ) );

		XmlElement* childElement = currentElement->FirstChildElement();
		while( childElement )
		{
			NamedStrings actorDefChildBlackboard;
			actorDefChildBlackboard.PopulateFromXmlElementAttributes( *childElement );
			std::string attributeName = childElement->Name();

			childElement = childElement->NextSiblingElement();
		}

		currentElement = currentElement->NextSiblingElement();
	}
}

//-----------------------------------------------------------------------------------------------
void WeaponDefinition::ClearWeaponDefs()
{
	s_weaponDefs.clear();
}

//-----------------------------------------------------------------------------------------------
std::string WeaponDefinition::GetName() const
{
	return m_name;
}


