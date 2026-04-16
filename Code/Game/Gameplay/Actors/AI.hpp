#pragma once
#include "Game/Gameplay/Controller.hpp"
#include "Game/Framework/ActorHandle.hpp"


//-----------------------------------------------------------------------------------------------
class Map;

//-----------------------------------------------------------------------------------------------
class AI : public Controller
{
public:
	AI( Map* currentMap, ActorHandle actorToPossess );

	void Update( float deltaSeconds );
	void DamagedBy( ActorHandle target );

public:
	ActorHandle m_targetActorHandle;
};