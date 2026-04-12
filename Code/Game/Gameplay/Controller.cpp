#include "Game/Gameplay/Controller.hpp"
#include "Game/Gameplay/Map.hpp"


//-----------------------------------------------------------------------------------------------
Controller::Controller( Map* currentMap, ActorHandle actorToPosses /*= ActorHandle::INVALID */ )
	: m_currentMap( currentMap )
	, m_actorHandle( actorToPosses )
{
	// Note: add logic notifying actor that its been possessed
}

//-----------------------------------------------------------------------------------------------
void Controller::Possess( ActorHandle actorToPossess )
{
	// Note: add logic notifying actor that its been unpossessed

	m_actorHandle = actorToPossess;
	// Note: add logic notifying actor that its been possessed
}

//-----------------------------------------------------------------------------------------------
Actor* Controller::GetActor()
{
	int actorIndex = m_actorHandle.GetIndex();
	Actor* possesedActor = m_currentMap->m_actors[ actorIndex ];
	return possesedActor;
}