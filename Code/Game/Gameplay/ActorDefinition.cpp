#include "Game/Gameplay/ActorDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"


//-----------------------------------------------------------------------------------------------
std::vector<ActorDefinition> ActorDefinition::s_actorDefs;

//-----------------------------------------------------------------------------------------------
void ActorDefinition::InitializeActorDefs()
{
	// Read in TileDefinitions.xml
	XmlDocument actorDefs;
	actorDefs.LoadFile( "Data/Definitions/ActorDefinitions.xml" );
	int numActorDefs = actorDefs.FirstChildElement()->ChildElementCount();
	s_actorDefs.resize( numActorDefs );

	XmlElement* currentElement = actorDefs.FirstChildElement()->FirstChildElement();
	for( int elementIndex = 0; elementIndex < numActorDefs; elementIndex++ )
	{
		NamedStrings actorDefBlackboard;
		actorDefBlackboard.PopulateFromXmlElementAttributes( *currentElement );
		ActorDefinition& currentActorDef = s_actorDefs[ elementIndex ];

		// Basic Info
		currentActorDef.m_name = actorDefBlackboard.GetValue( "name", "" );
		currentActorDef.m_weapon = actorDefBlackboard.GetValue( "weapon", "" );
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

		// Collision
		XmlElement* childElement = currentElement->FirstChildElement();
		while( childElement )
		{
			NamedStrings actorDefChildBlackboard;
			actorDefChildBlackboard.PopulateFromXmlElementAttributes( *childElement );
			std::string attributeName = childElement->Name();

			if( attributeName == "Collision" )
			{
				currentActorDef.m_collision->m_physicsRadius = actorDefChildBlackboard.GetValue( "radius", 0.f );
				currentActorDef.m_collision->m_physicsHeight = actorDefChildBlackboard.GetValue( "height", 0.f );
				currentActorDef.m_collision->m_collidesWithWorld = actorDefChildBlackboard.GetValue( "collidesWithWorld", false );
				currentActorDef.m_collision->m_collidesWithActors = actorDefChildBlackboard.GetValue( "collidesWithActors", false );
				currentActorDef.m_collision->m_dieOnCollide = actorDefChildBlackboard.GetValue( "dieOnCollide", false );
				currentActorDef.m_collision->m_impulseOnCollide = actorDefChildBlackboard.GetValue( "impulseOnCollide", 0.f );
				float damageOnCollideMin = actorDefChildBlackboard.GetValue( "damageOnCollideMin", 0.f );
				float damageOnCollideMax = actorDefChildBlackboard.GetValue( "damageOnCollideMax", 0.f );
				currentActorDef.m_collision->m_damageOnCollide = FloatRange( damageOnCollideMin, damageOnCollideMax );
			}
			else if( attributeName == "Physics" )
			{
				currentActorDef.m_physics->m_walkSpeed = actorDefChildBlackboard.GetValue( "walkSpeed", 0.f );
				currentActorDef.m_physics->m_runSpeed = actorDefChildBlackboard.GetValue( "runSpeed", 0.f );
				currentActorDef.m_physics->m_turnSpeed = actorDefChildBlackboard.GetValue( "turnSpeed", 0.f );
				currentActorDef.m_physics->m_drag = actorDefChildBlackboard.GetValue( "drag", 0.f );
				currentActorDef.m_physics->m_isSimulated = actorDefChildBlackboard.GetValue( "simulated", false );
				currentActorDef.m_physics->m_isFlying = actorDefChildBlackboard.GetValue( "flying", false );
			}
			else if( attributeName == "Camera" )
			{
				currentActorDef.m_cameraView->m_eyeHeight = actorDefChildBlackboard.GetValue( "eyeHeight", 0.f );
				currentActorDef.m_cameraView->m_cameraFOVDegrees = actorDefChildBlackboard.GetValue( "cameraFOV", 60.f );
			}
			else if( attributeName == "AI" )
			{
				currentActorDef.m_ai->m_aiEnabled = actorDefChildBlackboard.GetValue( "aiEnabled", true );
				currentActorDef.m_ai->m_sightRadius = actorDefChildBlackboard.GetValue( "sightRadius", 0.f );
				currentActorDef.m_ai->m_sightAngle = actorDefChildBlackboard.GetValue( "sightAngle", 0.f );
			}

			childElement = childElement->NextSiblingElement();
		}

		currentElement = currentElement->NextSiblingElement();
	}
}

//-----------------------------------------------------------------------------------------------
std::string ActorDefinition::GetName() const
{
	return m_name;
}