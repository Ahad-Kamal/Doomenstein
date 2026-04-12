#include "Game/Gameplay/Controller.hpp"
#include "Game/Gameplay/Map.hpp"


//-----------------------------------------------------------------------------------------------
void Controller::Possess( ActorHandle actorToPossess )
{
	// Note add logic notifying actor that its been unpossesed

	m_actorHandle = actorToPossess;
}

//-----------------------------------------------------------------------------------------------
Actor* Controller::GetActor()
{
	int actorIndex = m_actorHandle.GetIndex();
	Actor* possesedActor = m_currentMap->m_actors[ actorIndex ];
}