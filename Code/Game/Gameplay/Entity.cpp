#include "Game/Gameplay/Entity.hpp"
#include "Game/Gameplay/Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"


//-----------------------------------------------------------------------------------------------
Entity::Entity( Game* owner, Vec3 const& startPos )
	: m_game( owner )
	, m_position( startPos )
{
}

//-----------------------------------------------------------------------------------------------
Entity::~Entity()
{

}

//-----------------------------------------------------------------------------------------------
Mat44 Entity::GetModelToWorldTransform() const
{
	Mat44 modelMatrix;
	modelMatrix.AppendTranslation3D( m_position );
	modelMatrix.Append( m_orientation.GetAsMatrix_IFwd_JLeft_KUp() );
	return modelMatrix;
}

//-----------------------------------------------------------------------------------------------
//void Entity::DebugRender() const
//{
//	Vec2 forwardNormal = GetForwardNormal();
//	Vec2 leftNormal = forwardNormal.GetRotatedBy90Degrees();
//
//	Vec2 forwardVector = m_position + (forwardNormal * m_physicsRadius );
//	Vec2 leftVector = m_position + ( leftNormal * m_physicsRadius );
//
//	Vec2 worldCenter = Vec2( WORLD_CENTER_X, WORLD_CENTER_Y );
//
//	DebugDrawLine( worldCenter, m_position, DEBUG_LINE_THICKNESS, Rgba8( 50, 50, 50, 255 ) );
//	DebugDrawRing( m_position, m_physicsRadius, DEBUG_LINE_THICKNESS, Rgba8( 0, 255, 255 ) );
//	DebugDrawRing( m_position, m_cosmeticRadius, DEBUG_LINE_THICKNESS, Rgba8( 255, 0, 255 ) );
//	DebugDrawLine( m_position, forwardVector, DEBUG_LINE_THICKNESS, Rgba8( 255, 0, 0) );
//	DebugDrawLine( m_position, leftVector, DEBUG_LINE_THICKNESS, Rgba8( 0, 255, 0 ) );
//	DebugDrawLine( m_position, m_position + m_velocity, DEBUG_LINE_THICKNESS, Rgba8( 255, 255, 0 ) );
//}

//-----------------------------------------------------------------------------------------------
//void Entity::TakeDamage( int damage )
//{
//	m_health -= damage;
//
//	if( m_health <= 0 )
//	{
//		Die();
//	}
//}

//-----------------------------------------------------------------------------------------------
//void Entity::Die()
//{
//	m_isDead = true;
//	m_isGarbage = true;
//}

//-----------------------------------------------------------------------------------------------
//bool Entity::IsAlive() const
//{
//	if( this == nullptr )
//	{
//		return false;
//	}
//
//	return !m_isDead;
//}

//-----------------------------------------------------------------------------------------------
//Vec2 Entity::GetForwardNormal() const
//{
//	return Vec2( CosDegrees( m_orientationDegrees ), SinDegrees( m_orientationDegrees ) );
//}
