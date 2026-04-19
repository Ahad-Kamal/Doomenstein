#include "Game/Gameplay/Actors/AI.hpp"
#include "Game/Gameplay/Actors/Actor.hpp"
#include "Game/Gameplay/Map.hpp"
#include "Game/Gameplay/ActorDefinition.hpp"
#include "Game/Framework/ActorHandle.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
AI::AI( Map* currentMap, ActorHandle actorToPossess )
	: Controller( currentMap, actorToPossess )
{

}

//-----------------------------------------------------------------------------------------------
void AI::Update( [[maybe_unused]] float deltaSeconds )
{
	Actor* possessedActor = GetActor();
	if( !possessedActor->IsAlive() )
	{
		return;
	}

	if( m_targetActorHandle == ActorHandle::INVALID )
	{
		ActorHandle enemyHandle = m_currentMap->GetClosestVisibleEnemy( GetActor() );
		if( enemyHandle != ActorHandle::INVALID )
		{
			m_targetActorHandle = enemyHandle;
		}
	}
	else
	{
		Actor* targetActor = m_currentMap->GetActorByHandle( m_targetActorHandle );
		Physics possesedActorPhysics = possessedActor->m_definition->GetPhysics();
		AIControl possesedActorAI = possessedActor->m_definition->GetAI();

		possessedActor->TurnInDirection( targetActor->m_position - possessedActor->m_position, possesedActorPhysics.m_turnSpeed * deltaSeconds );
		if( GetDistanceSquared3D( possessedActor->m_position, targetActor->m_position ) > ( possesedActorAI.m_sightRadius * possesedActorAI.m_sightRadius * 0.002f ) )
		{
			possessedActor->MoveInDirection( possessedActor->m_orientation.GetForwardDir_IFwd_JLeft_KUp(), possesedActorPhysics.m_runSpeed );
		}
		else
		{
			possessedActor->MoveInDirection( possessedActor->m_orientation.GetForwardDir_IFwd_JLeft_KUp(), possesedActorPhysics.m_walkSpeed );
		}

		possessedActor->Attack();

		if( !targetActor->IsAlive() )
		{
			m_targetActorHandle = ActorHandle::INVALID;
		}
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