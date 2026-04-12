#pragma once
#include "Game/Framework/ActorHandle.hpp"


//-----------------------------------------------------------------------------------------------
class Actor;
class Map;

//-----------------------------------------------------------------------------------------------
class Controller
{
public:
	void Possess( ActorHandle actorToPossess );
	Actor* GetActor();

public:
	ActorHandle m_actorHandle;
	Map* m_currentMap;
};