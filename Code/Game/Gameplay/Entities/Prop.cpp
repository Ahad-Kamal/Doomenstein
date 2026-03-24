#include "Game/Gameplay/Entities/Prop.hpp"
#include "Game/Gameplay/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/EulerAngles.hpp"


//-----------------------------------------------------------------------------------------------
Prop::Prop( Vec3 const& startingPosition, EulerAngles orientation )
	:	Entity( g_game, startingPosition )
{
	m_orientation = orientation;
}

//-----------------------------------------------------------------------------------------------
Prop::~Prop()
{

}

//-----------------------------------------------------------------------------------------------
void Prop::Update( [[maybe_unused]] float deltaSeconds )
{
	
}

//-----------------------------------------------------------------------------------------------
void Prop::Render() const
{
	/*g_engine->m_render->SetModelConstants( GetModelToWorldTransform(), m_color );
	g_engine->m_render->SetBlendMode( BlendMode::OPAQUE );
	g_engine->m_render->SetBlendStateIfChanged();
	g_engine->m_render->BindTexture( m_texture );*/
	g_engine->m_render->RenderSetup( m_texture, BlendMode::OPAQUE, GetModelToWorldTransform(), m_color );
	g_engine->m_render->DrawVertexArray( m_vertexes );
}

