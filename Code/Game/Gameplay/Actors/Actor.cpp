#include "Game/Gameplay/Actors/Actor.hpp"
#include "Game/Gameplay/ActorDefinition.hpp"
#include "Game/Gameplay/Actors/Player.hpp"
#include "Game/Gameplay/Game.hpp"
#include "Game/Gameplay/Map.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
Actor::Actor( Vec3 const& startingPosition, EulerAngles const& orientation, ActorDefinition* definition, 
	ActorHandle actorHandle, Map* owningMap, bool isStatic /*= true*/, Rgba8 color /*= Rgba8::WHITE */ )
	: m_position( startingPosition )
	, m_orientation( orientation )
	, m_isStatic( isStatic )
	, m_color( color )
	, m_definition( definition )
	, m_actorHandle( actorHandle )
	, m_map( owningMap )
{
	m_physicsHeight = definition->GetCollision().m_physicsHeight;
	m_cosmeticHeight = definition->GetCollision().m_physicsHeight;
	m_physicsRadius = definition->GetCollision().m_physicsRadius;
	m_cosmeticRadius = definition->GetCollision().m_physicsRadius;

	SubscribeEventCallbackFunction( "Possess", Event_OnPossessed );
	SubscribeEventCallbackFunction( "Unpossess", Event_OnUnpossessed );
}

//-----------------------------------------------------------------------------------------------
Actor::~Actor()
{

}

//-----------------------------------------------------------------------------------------------
void Actor::Update( [[maybe_unused]] float deltaSeconds )
{
	//if( m_controller == m_map->m_player && !m_map->m_player->m_isFreeFly )
	//{
	//	/*Vec3 position = m_map->m_player->m_position;
	//	position.z = 0.f;
	//	m_position = position;*/
	//}

	UpdatePhysics( deltaSeconds );
}

//-----------------------------------------------------------------------------------------------
void Actor::Render() const
{
	std::vector<Vertex> verts;
	AddVertsForCylinder3D( verts, m_position, Vec3( m_position.x, m_position.y, m_position.z + m_cosmeticHeight ), m_cosmeticRadius, 16, m_color );

	g_engine->m_render->RenderSetup();
	g_engine->m_render->DrawVertexArray( verts );

	std::vector<Vertex> wireframeVerts;
	Rgba8 lighterColor = Rgba8( (unsigned int)GetClamped( (float)( m_color.r + 150 ), 0.f, 255.f ), (unsigned int)GetClamped( (float)( m_color.g + 150 ), 0.f, 255.f ), (unsigned int)GetClamped( (float)( m_color.b + 150 ), 0.f, 255.f ) );
	AddVertsForCylinder3D( wireframeVerts, m_position, Vec3( m_position.x, m_position.y, m_position.z + m_cosmeticHeight ), m_cosmeticRadius, 16, lighterColor );

	g_engine->m_render->SetRasterizerState( RasterizerMode::WIREFRAME_CULL_BACK );
	g_engine->m_render->SetRasterizerStateIfChanged();

	g_engine->m_render->RenderSetup();
	g_engine->m_render->DrawVertexArray( wireframeVerts );

	g_engine->m_render->SetRasterizerState( RasterizerMode::SOLID_CULL_BACK );
	g_engine->m_render->SetRasterizerStateIfChanged();
}

//-----------------------------------------------------------------------------------------------
void Actor::UpdatePhysics( [[maybe_unused]] float deltaSeconds )
{
	m_position += m_velocity;
	m_velocity += m_acceleration;
	m_acceleration = Vec3();
}

//-----------------------------------------------------------------------------------------------
void Actor::AddForce( Vec3 force )
{
	m_velocity += force;
}

//-----------------------------------------------------------------------------------------------
void Actor::AddImpulse( Vec3 impulse )
{
	m_acceleration += impulse;
}

//-----------------------------------------------------------------------------------------------
bool Actor::IsAlive() const
{
	if( this == nullptr )
	{
		return false;
	}

	return !m_isDead;
}

//-----------------------------------------------------------------------------------------------
IntVec2 Actor::GetCoordsOfCurrentTile()
{
	return IntVec2( RoundDownToInt( m_position.x ), RoundDownToInt( m_position.y ) );
}

//-----------------------------------------------------------------------------------------------
Mat44 Actor::GetModelToWorldTransform() const
{
	Mat44 modelMatrix;
	modelMatrix.AppendTranslation3D( m_position );
	modelMatrix.Append( m_orientation.GetAsMatrix_IFwd_JLeft_KUp() );
	return modelMatrix;
}

//-----------------------------------------------------------------------------------------------
bool Actor::Event_OnPossessed( EventArgs& args )
{
	unsigned int actorIndex = args.GetValue( "ActorIndex", -1 );
	if( actorIndex < 0 )
	{
		return false;
	}

	Actor* targetActor = g_game->m_currentMap->m_actors[ actorIndex ];
	if( targetActor == nullptr )
	{
		return false;
	}

	std::string controller = args.GetValue( "ControllerType", "" );
	if( controller == "Player" )
	{
		if( targetActor->m_actorHandle == g_game->m_currentMap->m_player->m_actorHandle )
		{
			targetActor->m_controller = g_game->m_currentMap->m_player;
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------------------------
bool Actor::Event_OnUnpossessed( EventArgs& args )
{
	unsigned int actorIndex = args.GetValue( "ActorIndex", -1 );
	if( actorIndex < 0 )
	{
		return false;
	}

	Actor* targetActor = g_game->m_currentMap->m_actors[ actorIndex ];
	if( targetActor == nullptr )
	{
		return false;
	}

	// TO-DO: Have the actor switch back to the AI controller, if it exists //-----------------------------------------------------------------------------------------------
	targetActor->m_controller = nullptr;

	return false;
}
