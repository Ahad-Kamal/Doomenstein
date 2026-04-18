#include "Game/Gameplay/Actors/AI.hpp"
#include "Game/Gameplay/Actors/Actor.hpp"
#include "Game/Gameplay/Map.hpp"
#include "Game/Gameplay/ActorDefinition.hpp"
#include "Game/Framework/ActorHandle.hpp"


//-----------------------------------------------------------------------------------------------
AI::AI( Map* currentMap, ActorHandle actorToPossess )
	: Controller( currentMap, actorToPossess )
{

}

//-----------------------------------------------------------------------------------------------
void AI::Update( [[maybe_unused]] float deltaSeconds )
{
	if( m_targetActorHandle == ActorHandle::INVALID )
	{
		ActorHandle enemyHandle = m_currentMap->GetNearestVisibleEnemy( GetActor() );
		if( enemyHandle != ActorHandle::INVALID )
		{
			m_targetActorHandle = enemyHandle;
		}
	}
	else
	{
		Actor* possessedActor = m_currentMap->GetActorByHandle( m_actorHandle );
		Actor* targetActor = m_currentMap->GetActorByHandle( m_targetActorHandle );

		Physics possesedActorPhysics = possessedActor->m_definition->GetPhysics();
		possessedActor->TurnInDirection( targetActor->m_position - possessedActor->m_position, possesedActorPhysics.m_turnSpeed * deltaSeconds );
		possessedActor->MoveInDirection( possessedActor->m_orientation.GetForwardDir_IFwd_JLeft_KUp(), possesedActorPhysics.m_walkSpeed );
	}
}

//-----------------------------------------------------------------------------------------------
void AI::Possess( ActorHandle actorToPossess )
{
	m_targetActorHandle = actorToPossess;
	Actor* actor = GetActor();
	actor->m_ai = this;
	actor->m_controller = this;
}

//-----------------------------------------------------------------------------------------------
void AI::DamagedBy( ActorHandle target )
{
	m_targetActorHandle = target;
}