#include "Game/Gameplay/Actors/Actor.hpp"
#include "Game/Gameplay/ActorDefinition.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
Actor::Actor( Vec3 const& startingPosition, EulerAngles const& orientation, bool isStatic /* = true */, Rgba8 color /* = Rgba8::WHITE */)
	: m_position( startingPosition )
	, m_orientation( orientation )
	, m_isStatic( isStatic )
	, m_color( color )
	, m_definition( nullptr )
{
	m_physicsHeight = 0.75f;
	m_cosmeticHeight = 0.75f;
	m_physicsRadius = 0.35f;
	m_cosmeticRadius = 0.35f;
}

//-----------------------------------------------------------------------------------------------
Actor::Actor( Vec3 const& startingPosition, EulerAngles const& orientation, float physicsHeight, float cosmeticHeight, float physicsRadius, float cosmeticRadius, bool isStatic /* = true */, Rgba8 color /* = Rgba8::WHITE */ )
	: m_position( startingPosition )
	, m_orientation( orientation )
	, m_physicsHeight( physicsHeight )
	, m_cosmeticHeight( cosmeticHeight )
	, m_physicsRadius( physicsRadius )
	, m_cosmeticRadius( cosmeticRadius )
	, m_isStatic( isStatic )
	, m_color( color )
	, m_definition( nullptr )
{
}

//-----------------------------------------------------------------------------------------------
Actor::Actor( Vec3 const& startingPosition, EulerAngles const& orientation, float physicsHeight, float physicsRadius, bool isStatic /*= true*/, Rgba8 color /*= Rgba8::WHITE */ )
	: m_position( startingPosition )
	, m_orientation( orientation )
	, m_physicsHeight( physicsHeight )
	, m_cosmeticHeight( physicsHeight )
	, m_physicsRadius( physicsRadius )
	, m_cosmeticRadius( physicsRadius )
	, m_isStatic( isStatic )
	, m_color( color )
	, m_definition( nullptr )
{
}

//-----------------------------------------------------------------------------------------------
Actor::Actor( Vec3 const& startingPosition, EulerAngles const& orientation, ActorDefinition* definition, ActorHandle actorHandle, bool isStatic /*= true*/, Rgba8 color /*= Rgba8::WHITE */ )
	: m_position( startingPosition )
	, m_orientation( orientation )
	, m_isStatic( isStatic )
	, m_color( color )
	, m_definition( definition )
	, m_actorHandle( actorHandle )
{
	m_physicsHeight = definition->GetCollision().m_physicsHeight;
	m_cosmeticHeight = definition->GetCollision().m_physicsHeight;
	m_physicsRadius = definition->GetCollision().m_physicsRadius;
	m_cosmeticRadius = definition->GetCollision().m_physicsRadius;
}

//-----------------------------------------------------------------------------------------------
Actor::~Actor()
{

}

//-----------------------------------------------------------------------------------------------
void Actor::Update( [[maybe_unused]] float deltaSeconds )
{

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