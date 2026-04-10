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
			currentActorDef.m_faction;
		}

		// Collision
	}
}

//-----------------------------------------------------------------------------------------------
std::string ActorDefinition::GetName() const
{
	return m_name;
}