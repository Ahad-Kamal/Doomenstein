#include "Game/Gameplay/ActorDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/StringUtils.hpp"


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
Faction ActorDefinition::GetFaction() const
{
	return m_faction;
}