#include "Game/Gameplay/Controller.hpp"
#include "Game/Gameplay/Map.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/StringUtils.hpp"


//-----------------------------------------------------------------------------------------------
Controller::Controller( Map* currentMap, ActorHandle actorToPosses /*= ActorHandle::INVALID */ )
	: m_currentMap( currentMap )
	, m_actorHandle( actorToPosses )
{
}

//-----------------------------------------------------------------------------------------------
void Controller::Possess( ActorHandle actorToPossess )
{
	// Note: add logic notifying actor that its been unpossessed

	// Note: add logic notifying actor that its been possessed

	// Note: This code might be unused entirely
	m_actorHandle = actorToPossess;
	EventArgs args;
	args.SetValue( "ActorIndex", Stringf( "%u", m_actorHandle.GetIndex() ) );
	FireEvent( "Possess", args );
}

//-----------------------------------------------------------------------------------------------
Actor* Controller::GetActor()
{
	Actor* possesedActor = m_currentMap->GetActorByHandle( m_actorHandle );
	return possesedActor;
}