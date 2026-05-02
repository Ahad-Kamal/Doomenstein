#include "Game/Gameplay/WeaponDefinition.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"


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

			// HUD
			if( attributeName == "HUD" )
			{
				currentWeaponDef.m_hud.m_shader = actorDefChildBlackboard.GetValue( "shader", "Default" );
				currentWeaponDef.m_hud.m_baseTexture = actorDefChildBlackboard.GetValue( "baseTexture", "Default" );
				currentWeaponDef.m_hud.m_reticleTexture = actorDefChildBlackboard.GetValue( "reticleTexture", "Default" );
				currentWeaponDef.m_hud.m_reticleSize = actorDefChildBlackboard.GetValue( "reticleSize", Vec2( 1.f, 1.f ) );
				currentWeaponDef.m_hud.m_spriteSize = actorDefChildBlackboard.GetValue( "spriteSize", Vec2( 1.f, 1.f ) );
				currentWeaponDef.m_hud.m_spritePivot = actorDefChildBlackboard.GetValue( "spritePivot", Vec2( 0.5f, 0.f ) );

				XmlElement* animationElement = childElement->FirstChildElement();
				while( animationElement )
				{
					NamedStrings animationBlackboard;
					animationBlackboard.PopulateFromXmlElementAttributes( *animationElement );

					WeaponAnimation newAnim;
					newAnim.m_name = animationBlackboard.GetValue( "name", "Idle" );
					newAnim.m_shader = animationBlackboard.GetValue( "shader", "Default" );
					newAnim.m_cellCount = animationBlackboard.GetValue( "cellCount", IntVec2( 1, 1 ) );
					newAnim.m_startFrame = animationBlackboard.GetValue( "startFrame", 0 );
					newAnim.m_endFrame = animationBlackboard.GetValue( "endFrame", 0 );

					float secondsPerFrame = animationBlackboard.GetValue( "secondsPerFrame", 1.f );
					newAnim.m_framesPerSecond = 1 / secondsPerFrame;

					std::string spriteSheetString = animationBlackboard.GetValue( "spriteSheet", "Default" );
					Texture* spriteTexture = nullptr;
					if( spriteSheetString != "Default" )
					{
						spriteTexture = g_engine->m_render->CreateOrGetTextureFromFile( spriteSheetString.c_str() );
						newAnim.m_spriteSheet = new SpriteSheet( *spriteTexture, newAnim.m_cellCount );
					}

					newAnim.m_animDef = new SpriteAnimDefinition( *newAnim.m_spriteSheet, newAnim.m_startFrame, newAnim.m_endFrame, newAnim.m_framesPerSecond, SpriteAnimPlaybackType::ONCE );

					currentWeaponDef.m_animations.push_back( newAnim );
					animationElement = animationElement->NextSiblingElement();
				}
			}

			childElement = childElement->NextSiblingElement();
		}

		if( currentWeaponDef.m_ray.m_rayCount > 0 )
		{
			currentWeaponDef.m_type = WEAPON_TYPE_RAY;
		}
		else if( currentWeaponDef.m_projectile.m_projectileCount > 0 )
		{
			currentWeaponDef.m_type = WEAPON_TYPE_PROJECTILE;
		}
		else if( currentWeaponDef.m_melee.m_meleeCount > 0 )
		{
			currentWeaponDef.m_type = WEAPON_TYPE_MELEE;
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

//-----------------------------------------------------------------------------------------------
WeaponType WeaponDefinition::GetType() const
{
	return m_type;
}

//-----------------------------------------------------------------------------------------------
float WeaponDefinition::GetRefireTime() const
{
	return m_refireTime;
}

//-----------------------------------------------------------------------------------------------
RayWeapon WeaponDefinition::GetRayWeaponInfo() const
{
	return m_ray;
}

//-----------------------------------------------------------------------------------------------
ProjectileWeapon WeaponDefinition::GetProjectileWeaponInfo() const
{
	return m_projectile;
}

//-----------------------------------------------------------------------------------------------
MeleeWeapon WeaponDefinition::GetMeleeWeaponInfo() const
{
	return m_melee;
}

//-----------------------------------------------------------------------------------------------
HUD WeaponDefinition::GetHud() const
{
	return m_hud;
}

//-----------------------------------------------------------------------------------------------
WeaponAnimation* WeaponDefinition::GetAnimationByName(  std::string const& animName  )
{
	for( unsigned int animIndex = 0; animIndex < m_animations.size(); animIndex++ )
	{
		if( m_animations[ animIndex ].m_name == animName )
		{
			return &m_animations[ animIndex ];
		}
	}

	return nullptr;
}

//-----------------------------------------------------------------------------------------------
WeaponAnimation* WeaponDefinition::GetAnimationByState( AnimState animState )
{
	switch( animState )
	{
		case AnimState::IDLE:
			return GetAnimationByName( "Idle" );

		case AnimState::ATTACK:
			return GetAnimationByName( "Attack" );
	}

	return nullptr;
}

//-----------------------------------------------------------------------------------------------
float WeaponDefinition::GetAnimationDuration( AnimState animState )
{
	WeaponAnimation weaponDef = *GetAnimationByState( animState );
	int frameCount = weaponDef.m_endFrame - weaponDef.m_startFrame;
	return frameCount * ( 1 / weaponDef.m_framesPerSecond );
}
