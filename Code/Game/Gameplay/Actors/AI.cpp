#include "Game/Gameplay//Actors/AI.hpp"
#include "Game/Gameplay/Map.hpp"
#include "Game/Framework/ActorHandle.hpp"


//-----------------------------------------------------------------------------------------------
AI::AI( Map* currentMap, ActorHandle actorToPossess )
	: Controller( currentMap, actorToPossess )
{

}

//-----------------------------------------------------------------------------------------------
void AI::Update( [[maybe_unused]] float deltaSeconds )
{

}

//-----------------------------------------------------------------------------------------------
void AI::DamagedBy( ActorHandle target )
{
	m_targetActorHandle = target;
}