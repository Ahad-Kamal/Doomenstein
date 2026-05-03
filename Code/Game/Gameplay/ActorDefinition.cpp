#include "Game/Gameplay/ActorDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Engine.hpp"


//-----------------------------------------------------------------------------------------------
std::vector<ActorDefinition> ActorDefinition::s_actorDefs;

//-----------------------------------------------------------------------------------------------
void ActorDefinition::InitializeActorDefs()
{
	// Read in ActorDefinitions.xml
	XmlDocument actorDefs;
	actorDefs.LoadFile( "Data/Definitions/ActorDefinitions.xml" );
	int numActorDefs = actorDefs.FirstChildElement()->ChildElementCount();

	int startingIndex = static_cast<int>( s_actorDefs.size() );
	s_actorDefs.resize( startingIndex + numActorDefs );

	XmlElement* currentElement = actorDefs.FirstChildElement()->FirstChildElement();
	for( int elementIndex = startingIndex; elementIndex < numActorDefs + startingIndex; elementIndex++ )
	{
		NamedStrings actorDefBlackboard;
		actorDefBlackboard.PopulateFromXmlElementAttributes( *currentElement );
		ActorDefinition& currentActorDef = s_actorDefs[ elementIndex ];

		// Basic Info
		currentActorDef.m_name = actorDefBlackboard.GetValue( "name", "" );
		currentActorDef.m_health = actorDefBlackboard.GetValue( "health", 1 );
		currentActorDef.m_corpseLifetime = actorDefBlackboard.GetValue( "corpseLifetime", 0.f );
		currentActorDef.m_isVisible = actorDefBlackboard.GetValue( "visible", false );
		currentActorDef.m_canBePossessed = actorDefBlackboard.GetValue( "canBePossessed", false );
		currentActorDef.m_dieOnSpawn = actorDefBlackboard.GetValue( "dieOnSpawn", false );
		std::string faction = actorDefBlackboard.GetValue( "faction", "neutral" );
		if( faction == "Marine" || faction == "marine" )
		{
			currentActorDef.m_faction = Faction::MARINE;
		}
		else if( faction == "Demon" || faction == "demon" )
		{
			currentActorDef.m_faction = Faction::DEMON;
		}
		else
		{
			currentActorDef.m_faction = Faction::NEUTRAL;
		}

		XmlElement* childElement = currentElement->FirstChildElement();
		while( childElement )
		{
			NamedStrings actorDefChildBlackboard;
			actorDefChildBlackboard.PopulateFromXmlElementAttributes( *childElement );
			std::string attributeName = childElement->Name();

			// Collision
			if( attributeName == "Collision" )
			{
				currentActorDef.m_collision.m_physicsRadius = actorDefChildBlackboard.GetValue( "radius", 0.f );
				currentActorDef.m_collision.m_physicsHeight = actorDefChildBlackboard.GetValue( "height", 0.f );
				currentActorDef.m_collision.m_collidesWithWorld = actorDefChildBlackboard.GetValue( "collidesWithWorld", false );
				currentActorDef.m_collision.m_collidesWithActors = actorDefChildBlackboard.GetValue( "collidesWithActors", false );
				currentActorDef.m_collision.m_dieOnCollide = actorDefChildBlackboard.GetValue( "dieOnCollide", false );
				currentActorDef.m_collision.m_impulseOnCollide = actorDefChildBlackboard.GetValue( "impulseOnCollide", 0.f );
				std::string damageOnCollide = actorDefChildBlackboard.GetValue( "damageOnCollide", "0.0~0.0" );
				Strings damageOnCollideValues = SplitStringOnDelimiter( damageOnCollide, '~' );
				currentActorDef.m_collision.m_damageOnCollide = FloatRange( stof( damageOnCollideValues[ 0 ] ), stof( damageOnCollideValues[ 1 ] ) );
			}
			// Physics
			else if( attributeName == "Physics" )
			{
				currentActorDef.m_physics.m_walkSpeed = actorDefChildBlackboard.GetValue( "walkSpeed", 0.f );
				currentActorDef.m_physics.m_runSpeed = actorDefChildBlackboard.GetValue( "runSpeed", 0.f );
				currentActorDef.m_physics.m_turnSpeed = actorDefChildBlackboard.GetValue( "turnSpeed", 0.f );
				currentActorDef.m_physics.m_drag = actorDefChildBlackboard.GetValue( "drag", 0.f );
				currentActorDef.m_physics.m_isSimulated = actorDefChildBlackboard.GetValue( "simulated", false );
				currentActorDef.m_physics.m_isFlying = actorDefChildBlackboard.GetValue( "flying", false );
			}
			// CameraView
			else if( attributeName == "Camera" )
			{
				currentActorDef.m_cameraView.m_eyeHeight = actorDefChildBlackboard.GetValue( "eyeHeight", 0.f );
				currentActorDef.m_cameraView.m_cameraFOVDegrees = actorDefChildBlackboard.GetValue( "cameraFOV", 60.f );
			}
			// AI
			else if( attributeName == "AI" )
			{
				currentActorDef.m_ai.m_aiEnabled = actorDefChildBlackboard.GetValue( "aiEnabled", true );
				currentActorDef.m_ai.m_sightRadius = actorDefChildBlackboard.GetValue( "sightRadius", 0.f );
				currentActorDef.m_ai.m_sightAngle = actorDefChildBlackboard.GetValue( "sightAngle", 0.f );
			}
			// Inventory
			else if( attributeName == "Inventory" )
			{
				XmlElement* weaponElement = childElement->FirstChildElement();
				while( weaponElement )
				{
					NamedStrings actorDefInventoryBlackboard;
					actorDefInventoryBlackboard.PopulateFromXmlElementAttributes( *weaponElement );

					std::string newWeapon = actorDefInventoryBlackboard.GetValue( "name", "" );
					currentActorDef.m_weapons.push_back( newWeapon );

					weaponElement = weaponElement->NextSiblingElement();
				}
			}
			// Visuals
			else if( attributeName == "Visuals" )
			{
				currentActorDef.m_visuals.m_size = actorDefChildBlackboard.GetValue( "size", Vec2( 1.f, 1.f ) );
				currentActorDef.m_visuals.m_pivot = actorDefChildBlackboard.GetValue( "pivot", Vec2( 0.5f, 0.5f ) );
				currentActorDef.m_visuals.m_cellCount = actorDefChildBlackboard.GetValue( "cellCount", IntVec2( 1, 1 ) );
				currentActorDef.m_visuals.m_shader = actorDefChildBlackboard.GetValue( "shader", "Default" );
				currentActorDef.m_visuals.m_renderLit = actorDefChildBlackboard.GetValue( "renderLit", false );
				currentActorDef.m_visuals.m_renderRounded = actorDefChildBlackboard.GetValue( "renderRounded", false );

				std::string spriteSheetString = actorDefChildBlackboard.GetValue( "spriteSheet", "Default" );
				Texture* spriteTexture = nullptr;
				if( spriteSheetString != "Default" )
				{
					spriteTexture = g_engine->m_render->CreateOrGetTextureFromFile( spriteSheetString.c_str() );
					currentActorDef.m_visuals.m_spriteSheet = new SpriteSheet( *spriteTexture, currentActorDef.m_visuals.m_cellCount );
				}

				std::string billboardString = actorDefChildBlackboard.GetValue( "billboardType", "NONE" );
				currentActorDef.m_visuals.m_billboardType = GetBillboardTypeFromString( billboardString );

				XmlElement* animGroupElement = childElement->FirstChildElement();
				while( animGroupElement )
				{
					SpriteAnimationGroupDefinition newSpriteAnimGroupDef;
					newSpriteAnimGroupDef.LoadFromXmlElement( *animGroupElement, *currentActorDef.m_visuals.m_spriteSheet );

					currentActorDef.m_visuals.m_animGroupDefs.push_back( newSpriteAnimGroupDef );
					animGroupElement = animGroupElement->NextSiblingElement();
				}
			}
			// Sounds
			else if( attributeName == "Sounds" )
			{
				XmlElement* soundElement = childElement->FirstChildElement();
				while( soundElement )
				{
					NamedStrings soundsBlackboard;
					soundsBlackboard.PopulateFromXmlElementAttributes( *soundElement );

					std::string sound = soundsBlackboard.GetValue( "sound", "" );
					std::string name = soundsBlackboard.GetValue( "name", "" );

					currentActorDef.m_soundEffects.emplace( sound, name );
					soundElement = soundElement->NextSiblingElement();
				}
			}

			childElement = childElement->NextSiblingElement();
		}

		currentElement = currentElement->NextSiblingElement();
	}
}

//-----------------------------------------------------------------------------------------------
void ActorDefinition::InitializeProjectileActorDefs()
{
	// Read in ActorDefinitions.xml
	XmlDocument actorDefs;
	actorDefs.LoadFile( "Data/Definitions/ProjectileActorDefinitions.xml" );
	int numActorDefs = actorDefs.FirstChildElement()->ChildElementCount();

	int startingIndex = static_cast<int>( s_actorDefs.size() );
	s_actorDefs.resize( numActorDefs );

	XmlElement* currentElement = actorDefs.FirstChildElement()->FirstChildElement();
	for( int elementIndex = startingIndex; elementIndex < numActorDefs + startingIndex; elementIndex++ )
	{
		NamedStrings actorDefBlackboard;
		actorDefBlackboard.PopulateFromXmlElementAttributes( *currentElement );
		ActorDefinition& currentActorDef = s_actorDefs[ elementIndex ];

		// Basic Info
		currentActorDef.m_name = actorDefBlackboard.GetValue( "name", "" );
		currentActorDef.m_health = actorDefBlackboard.GetValue( "health", 1 );
		currentActorDef.m_corpseLifetime = static_cast<double>( actorDefBlackboard.GetValue( "corpseLifetime", 0.f ) );
		currentActorDef.m_isVisible = actorDefBlackboard.GetValue( "visible", false );
		currentActorDef.m_canBePossessed = actorDefBlackboard.GetValue( "canBePossessed", false );
		currentActorDef.m_dieOnSpawn = actorDefBlackboard.GetValue( "dieOnSpawn", false );
		std::string faction = actorDefBlackboard.GetValue( "faction", "neutral" );
		if( faction == "Marine" || faction == "marine" )
		{
			currentActorDef.m_faction = Faction::MARINE;
		}
		else if( faction == "Demon" || faction == "demon" )
		{
			currentActorDef.m_faction = Faction::DEMON;
		}
		else
		{
			currentActorDef.m_faction = Faction::NEUTRAL;
		}

		XmlElement* childElement = currentElement->FirstChildElement();
		while( childElement )
		{
			NamedStrings actorDefChildBlackboard;
			actorDefChildBlackboard.PopulateFromXmlElementAttributes( *childElement );
			std::string attributeName = childElement->Name();

			// Collision
			if( attributeName == "Collision" )
			{
				currentActorDef.m_collision.m_physicsRadius = actorDefChildBlackboard.GetValue( "radius", 0.f );
				currentActorDef.m_collision.m_physicsHeight = actorDefChildBlackboard.GetValue( "height", 0.f );
				currentActorDef.m_collision.m_collidesWithWorld = actorDefChildBlackboard.GetValue( "collidesWithWorld", false );
				currentActorDef.m_collision.m_collidesWithActors = actorDefChildBlackboard.GetValue( "collidesWithActors", false );
				currentActorDef.m_collision.m_dieOnCollide = actorDefChildBlackboard.GetValue( "dieOnCollide", false );
				currentActorDef.m_collision.m_impulseOnCollide = actorDefChildBlackboard.GetValue( "impulseOnCollide", 0.f );
				std::string damageOnCollide = actorDefChildBlackboard.GetValue( "damageOnCollide", "0.0~0.0" );
				Strings damageOnCollideValues = SplitStringOnDelimiter( damageOnCollide, '~' );
				currentActorDef.m_collision.m_damageOnCollide = FloatRange( stof( damageOnCollideValues[ 0 ] ), stof( damageOnCollideValues[ 1 ] ) );
			}
			// Physics
			else if( attributeName == "Physics" )
			{
				currentActorDef.m_physics.m_walkSpeed = actorDefChildBlackboard.GetValue( "walkSpeed", 0.f );
				currentActorDef.m_physics.m_runSpeed = actorDefChildBlackboard.GetValue( "runSpeed", 0.f );
				currentActorDef.m_physics.m_turnSpeed = actorDefChildBlackboard.GetValue( "turnSpeed", 0.f );
				currentActorDef.m_physics.m_drag = actorDefChildBlackboard.GetValue( "drag", 0.f );
				currentActorDef.m_physics.m_isSimulated = actorDefChildBlackboard.GetValue( "simulated", false );
				currentActorDef.m_physics.m_isFlying = actorDefChildBlackboard.GetValue( "flying", false );
			}
			// Visuals
			else if( attributeName == "Visuals" )
			{
				currentActorDef.m_visuals.m_size = actorDefChildBlackboard.GetValue( "size", Vec2( 1.f, 1.f ) );
				currentActorDef.m_visuals.m_pivot = actorDefChildBlackboard.GetValue( "pivot", Vec2( 0.5f, 0.5f ) );
				currentActorDef.m_visuals.m_cellCount = actorDefChildBlackboard.GetValue( "cellCount", IntVec2( 1, 1 ) );
				currentActorDef.m_visuals.m_shader = actorDefChildBlackboard.GetValue( "shader", "Default" );
				currentActorDef.m_visuals.m_renderLit = actorDefChildBlackboard.GetValue( "renderLit", false );
				currentActorDef.m_visuals.m_renderRounded = actorDefChildBlackboard.GetValue( "renderRounded", false );

				std::string spriteSheetString = actorDefChildBlackboard.GetValue( "spriteSheet", "Default" );
				Texture* spriteTexture = nullptr;
				if( spriteSheetString != "Default" )
				{
					spriteTexture = g_engine->m_render->CreateOrGetTextureFromFile( spriteSheetString.c_str() );
					currentActorDef.m_visuals.m_spriteSheet = new SpriteSheet( *spriteTexture, currentActorDef.m_visuals.m_cellCount );
				}

				std::string billboardString = actorDefChildBlackboard.GetValue( "billboardType", "NONE" );
				currentActorDef.m_visuals.m_billboardType = GetBillboardTypeFromString( billboardString );

				XmlElement* animGroupElement = childElement->FirstChildElement();
				while( animGroupElement )
				{
					SpriteAnimationGroupDefinition newSpriteAnimGroupDef;
					newSpriteAnimGroupDef.LoadFromXmlElement( *animGroupElement, *currentActorDef.m_visuals.m_spriteSheet );

					currentActorDef.m_visuals.m_animGroupDefs.push_back( newSpriteAnimGroupDef );
					animGroupElement = animGroupElement->NextSiblingElement();
				}
			}
			childElement = childElement->NextSiblingElement();
		}
		currentElement = currentElement->NextSiblingElement();
	}
}

//-----------------------------------------------------------------------------------------------
void ActorDefinition::ClearActorDefs()
{
	s_actorDefs.clear();
}

//-----------------------------------------------------------------------------------------------
std::string ActorDefinition::GetName() const
{
	return m_name;
}

//-----------------------------------------------------------------------------------------------
Strings ActorDefinition::GetWeapons() const
{
	return m_weapons;
}

//-----------------------------------------------------------------------------------------------
Collision ActorDefinition::GetCollision() const
{
	return m_collision;
}

//-----------------------------------------------------------------------------------------------
Physics ActorDefinition::GetPhysics() const
{
	return m_physics;
}

//-----------------------------------------------------------------------------------------------
CameraView ActorDefinition::GetCameraView() const
{
	return m_cameraView;
}

//-----------------------------------------------------------------------------------------------
AIControl ActorDefinition::GetAI() const
{
	return m_ai;
}

//-----------------------------------------------------------------------------------------------
Visuals ActorDefinition::GetVisuals() const
{
	return m_visuals;
}

//-----------------------------------------------------------------------------------------------
int ActorDefinition::GetHealth() const
{
	return m_health;
}

//-----------------------------------------------------------------------------------------------
double ActorDefinition::GetCorpseLifetime() const
{
	return m_corpseLifetime;
}

//-----------------------------------------------------------------------------------------------
bool ActorDefinition::GetIsVisible() const
{
	return m_isVisible;
}

//-----------------------------------------------------------------------------------------------
bool ActorDefinition::GetCanBePossesed() const
{
	return m_canBePossessed;
}

//-----------------------------------------------------------------------------------------------
bool ActorDefinition::GetDieOnSpawn() const
{
	return m_dieOnSpawn;
}

//-----------------------------------------------------------------------------------------------
Faction ActorDefinition::GetFaction() const
{
	return m_faction;
}

//-----------------------------------------------------------------------------------------------
SoundMap ActorDefinition::GetSoundEffects() const
{
	return m_soundEffects;
}

//-----------------------------------------------------------------------------------------------
const SpriteAnimationGroupDefinition* ActorDefinition::GetAnimGroupByName( std::string const& animName )
{
	for( unsigned int groupIndex = 0; groupIndex < m_visuals.m_animGroupDefs.size(); groupIndex++ )
	{
		if( m_visuals.m_animGroupDefs[ groupIndex ].m_name == animName )
		{
			return &m_visuals.m_animGroupDefs[ groupIndex ];
		}
	}

	return nullptr;
}

//-----------------------------------------------------------------------------------------------
const SpriteAnimationGroupDefinition* ActorDefinition::GetAnimGroupByState( AnimState animState )
{
	switch( animState )
	{
		case AnimState::IDLE:
			return GetAnimGroupByName( "Idle" );
		
		case AnimState::WALK:
			return GetAnimGroupByName( "Walk" );

		case AnimState::ATTACK:
			return GetAnimGroupByName( "Attack" );

		case AnimState::HURT:
			return GetAnimGroupByName( "Hurt" );

		case AnimState::DEATH:
			return GetAnimGroupByName( "Death" );
	}

	return nullptr;
}