#include "Game/Gameplay/Entities/BillboardText.hpp"
#include "Game/Gameplay/Entities/Player.hpp"
#include "Game/Gameplay/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/EulerAngles.hpp"


//-----------------------------------------------------------------------------------------------
BillboardText::BillboardText( Vec3 const& startingPosition, EulerAngles orientation, BillboardType mode /*= BillboardType::NONE */ )
	:	Entity( g_game, startingPosition )
{
	m_orientation = orientation;
	m_mode = mode;
}

//-----------------------------------------------------------------------------------------------
BillboardText::~BillboardText()
{

}

//-----------------------------------------------------------------------------------------------
void BillboardText::Update( [[maybe_unused]] float deltaSeconds )
{
	m_transform = GetBillboardTransform( m_mode, g_game->m_player->GetModelToWorldTransform(), m_position );
	m_transform.SetTranslation3D( m_position );
}

//-----------------------------------------------------------------------------------------------
void BillboardText::Render() const
{
	g_engine->m_render->SetModelConstants( m_transform, m_color);
	g_engine->m_render->SetBlendMode( BlendMode::OPAQUE );
	g_engine->m_render->SetBlendStateIfChanged();
	g_engine->m_render->BindTexture( m_texture );
	g_engine->m_render->DrawVertexArray( m_vertexes );
}

