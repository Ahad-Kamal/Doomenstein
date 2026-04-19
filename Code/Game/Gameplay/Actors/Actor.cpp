#include "Game/Gameplay/Actors/Actor.hpp"
#include "Game/Gameplay/ActorDefinition.hpp"
#include "Game/Gameplay/Actors/Player.hpp"
#include "Game/Gameplay/Actors/AI.hpp"
#include "Game/Gameplay/Weapon.hpp"
#include "Game/Gameplay/Game.hpp"
#include "Game/Gameplay/Map.hpp"
#include "Game/Gameplay/WeaponDefinition.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>


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

	m_health = definition->GetHealth();
	for( unsigned int weaponIndex = 0; weaponIndex < m_definition->GetWeapons().size(); weaponIndex++ )
	{
		std::string weaponName = m_definition->GetWeapons()[ weaponIndex ];
		for( unsigned int weaponDefIndex = 0; weaponDefIndex < WeaponDefinition::s_weaponDefs.size(); weaponDefIndex++ )
		{
			std::string name = WeaponDefinition::s_weaponDefs[ weaponDefIndex ].GetName();
			if( weaponName == name )
			{
				Weapon* newWeapon = new Weapon( &WeaponDefinition::s_weaponDefs[ weaponDefIndex ] );
				m_weapons.push_back( newWeapon );
				break;
			}
		}
	}
	if( !m_weapons.empty() )
	{
		m_equippedWeapon = m_weapons[ 0 ];
	}
	if( m_definition->GetAI().m_aiEnabled )
	{
		m_ai = new AI( m_map, m_actorHandle );
		m_controller = m_ai;
	}

	SubscribeEventCallbackFunction( "Possess", Event_OnPossessed );
	SubscribeEventCallbackFunction( "Unpossess", Event_OnUnpossessed );

	RenderSetup();
}

//-----------------------------------------------------------------------------------------------
Actor::~Actor()
{
	if( m_controller != m_map->m_player )
	{
		delete m_controller;
	}
	else
	{
		delete m_ai;
	}
}

//-----------------------------------------------------------------------------------------------
void Actor::Update( [[maybe_unused]] float deltaSeconds )
{
	if( !m_isDead )
	{
		UpdatePhysics( deltaSeconds );
	}
	else
	{
		if( m_deathTimer->HasPeriodElapsed() )
		{
			m_isGarbage = true;

			if( m_controller == m_map->m_player )
			{
				EventArgs args;
				args.SetValue( "ActorIndex", Stringf( "%u", m_actorHandle.GetIndex() ) );
				FireEvent( "Unpossess", args );
			}
		}
		else if( m_controller == m_map->m_player )
		{
			float eyeHeight = m_definition->GetCameraView().m_eyeHeight;
			float deathTime = static_cast<float>( m_deathTimer->GetElaspedFraction() ) * 2.f;
			float zValue = Interpolate( eyeHeight, 0.f, deathTime );
			zValue = RangeMapClamped( zValue, 1.f, 0.f, eyeHeight, 0.f );

			m_position.z = zValue;
		}
	}
	if( m_ai != nullptr && m_ai == m_controller )
	{
		m_ai->Update( deltaSeconds );
	}
}

//-----------------------------------------------------------------------------------------------
void Actor::Render() const
{
	if( m_controller == m_map->m_player && m_map->m_player->m_cameraMode == CAMERA_MODE_FIRST_PERSON )
	{
		return;
	}

	Mat44 transformMatrix;
	transformMatrix.AppendTranslation3D( m_position );
	transformMatrix.AppendZRotation( m_orientation.m_yawDegrees );

	VertexList tempVerts = m_vertexes;
	VertexList tempWireframeVerts = m_wireframeVertexes;

	TransformVertexArray3D( tempVerts, transformMatrix );
	TransformVertexArray3D( tempWireframeVerts, transformMatrix );

	g_engine->m_render->RenderSetup();
	g_engine->m_render->DrawVertexArray( tempVerts );

	g_engine->m_render->SetRasterizerState( RasterizerMode::WIREFRAME_CULL_BACK );
	g_engine->m_render->SetRasterizerStateIfChanged();

	g_engine->m_render->RenderSetup();
	g_engine->m_render->DrawVertexArray( tempWireframeVerts );

	g_engine->m_render->SetRasterizerState( RasterizerMode::SOLID_CULL_BACK );
	g_engine->m_render->SetRasterizerStateIfChanged();
}

//-----------------------------------------------------------------------------------------------
void Actor::RenderSetup()
{
	AddVertsForCylinder3D( m_vertexes, Vec3(), Vec3( 0.f, 0.f, 0.f + m_cosmeticHeight), m_cosmeticRadius, 16, m_color);
	Vec3 arrowStart = Vec3( m_cosmeticRadius, 0.f, m_definition->GetCameraView().m_eyeHeight );
	Vec3 arrowEnd =  Vec3( m_cosmeticRadius + 0.075f, 0.f, m_definition->GetCameraView().m_eyeHeight );

	Rgba8 lighterColor = Rgba8( (unsigned int)GetClamped( (float)( m_color.r + 150 ), 0.f, 255.f ), (unsigned int)GetClamped( (float)( m_color.g + 150 ), 0.f, 255.f ), (unsigned int)GetClamped( (float)( m_color.b + 150 ), 0.f, 255.f ) );
	AddVertsForCylinder3D( m_wireframeVertexes, Vec3(), Vec3( 0.f, 0.f, 0.f + m_cosmeticHeight), m_cosmeticRadius, 16, lighterColor );

	if( m_definition->GetName() != "PlasmaProjectile" )
	{
		AddVertsForArrow3D( m_vertexes, arrowStart, arrowStart, arrowEnd, m_cosmeticRadius * 0.33f, 16, m_color );
		AddVertsForArrow3D( m_wireframeVertexes, arrowStart, arrowStart, arrowEnd, m_cosmeticRadius * 0.33f, 16, lighterColor );
	}
}

//-----------------------------------------------------------------------------------------------
void Actor::SetColor( Rgba8 const& color )
{
	for( unsigned int vertIndex = 0; vertIndex < m_vertexes.size(); vertIndex++ )
	{
		m_vertexes[ vertIndex ].m_color = color;
	}
}

//-----------------------------------------------------------------------------------------------
void Actor::UpdatePhysics( [[maybe_unused]] float deltaSeconds )
{
	Vec3 prevPosition = m_position;

	if( m_definition->GetPhysics().m_isSimulated )
	{
		AddForce( -m_velocity * m_definition->GetPhysics().m_drag );
		m_position += m_velocity * deltaSeconds;
		m_velocity += m_acceleration * deltaSeconds;
		m_acceleration = Vec3();
	}
}

//-----------------------------------------------------------------------------------------------
void Actor::AddForce( Vec3 const& force )
{
	m_acceleration += force;
}

//-----------------------------------------------------------------------------------------------
void Actor::AddImpulse( Vec3 const& impulse )
{
	m_velocity += impulse;
}

//-----------------------------------------------------------------------------------------------
void Actor::AddDirection( float yawDegrees )
{
	m_orientation.m_yawDegrees += yawDegrees;
}

//-----------------------------------------------------------------------------------------------
void Actor::MoveInDirection( Vec3 const& direction, float speed )
{
	float forceMagnitude = speed * m_definition->GetPhysics().m_drag;
	Vec3 force = Vec3( direction.x * forceMagnitude, direction.y * forceMagnitude, direction.z * forceMagnitude );
	AddForce( force );
}

//-----------------------------------------------------------------------------------------------
void Actor::TurnInDirection( Vec3 const& direction, float maxAmt )
{
	float currentYaw = m_orientation.m_yawDegrees;
	Vec3 directionNormalized = direction.GetNormalized();
	float targetYawRadians = atan2f( directionNormalized.y, directionNormalized.x );
	float targetYaw = ConvertRadiansToDegrees( targetYawRadians );
	
	float turnAmt = GetShortestAngularDispDegrees( currentYaw, targetYaw );
	m_orientation.m_yawDegrees += GetClamped( turnAmt, -maxAmt, maxAmt );
}

//-----------------------------------------------------------------------------------------------
void Actor::Attack()
{
	m_equippedWeapon->Fire( this );
}

//-----------------------------------------------------------------------------------------------
void Actor::Damage( int incomingDamage, ActorHandle damagingActor )
{
	m_health -= incomingDamage;

	if( m_ai != nullptr && m_ai == m_controller )
	{
		m_ai->m_targetActorHandle = damagingActor;
	}

	if( m_health <= 0 )
	{
		m_isDead = true;
		Rgba8 deadColor = Rgba8( (unsigned int)GetClamped( (float)( m_color.r - 100 ), 0.f, 255.f ), (unsigned int)GetClamped( (float)( m_color.g - 100 ), 0.f, 255.f ), (unsigned int)GetClamped( (float)( m_color.b - 100 ), 0.f, 255.f ) );
		SetColor( deadColor );

		m_deathTimer = new Timer( m_definition->GetCorpseLifetime(), g_game->m_gameClock );
		m_deathTimer->Start();
	}
}

//-----------------------------------------------------------------------------------------------
void Actor::OnCollide( Actor* collidingActor )
{
	if( m_definition->GetCollision().m_dieOnCollide )
	{
		m_isDead = true;
		m_isGarbage = true;
	}

	ActorDefinition collidingActorDef = *collidingActor->m_definition;
	if( m_definition->GetFaction() != collidingActorDef.GetFaction() )
	{
		FloatRange collidingActorDamageRange = collidingActorDef.GetCollision().m_damageOnCollide;
		if( collidingActorDamageRange != FloatRange() )
		{
			float randomDamage = g_rng->RollRandomFloatInRange( collidingActorDamageRange.m_min, collidingActorDamageRange.m_max );

			if( collidingActor->m_owner != nullptr )
			{
				Damage( static_cast<int>( roundf( randomDamage ) ), collidingActor->m_owner->m_actorHandle );
			}
			else
			{
				Damage( static_cast<int>( roundf( randomDamage ) ), collidingActor->m_actorHandle );
			}
		}

		float collidingActorImpulse = collidingActorDef.GetCollision().m_impulseOnCollide;
		if( collidingActorImpulse > 0.f )
		{
			Vec3 direction = ( m_position - collidingActor->m_position ).GetNormalized();
			direction.z = 0.f;
			AddImpulse( direction * collidingActorImpulse );
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Actor::EquipWeapon( int weaponToSwitchTo )
{
	if( weaponToSwitchTo >= static_cast<int>( m_weapons.size() ) )
	{
		weaponToSwitchTo = 0;
	}
	else if( weaponToSwitchTo <= -1 )
	{
		weaponToSwitchTo = static_cast<int>( m_weapons.size() ) - 1;
	}

	m_equippedWeapon = m_weapons[ weaponToSwitchTo ];
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
IntVec2 Actor::GetCoordsOfCurrentTile() const
{
	return IntVec2( RoundDownToInt( m_position.x ), RoundDownToInt( m_position.y ) );
}

//-----------------------------------------------------------------------------------------------
Mat44 Actor::GetModelToWorldTransform() const
{
	Mat44 modelMatrix;
	modelMatrix.AppendTranslation3D( m_position );
	//modelMatrix.Append( m_orientation.GetAsMatrix_IFwd_JLeft_KUp() );
	modelMatrix.AppendZRotation( m_orientation.m_yawDegrees );
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

	Actor* targetActor = g_game->m_currentMap->GetActorByIndex( actorIndex );
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
	// AI possession
	else
	{
		if( targetActor->m_actorHandle == g_game->m_currentMap->m_player->m_actorHandle )
		{

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

	Actor* targetActor = g_game->m_currentMap->GetActorByIndex( actorIndex );
	if( targetActor == nullptr )
	{
		return false;
	}

	if( targetActor->m_definition->GetAI().m_aiEnabled )
	{
		targetActor->m_controller = targetActor->m_ai;
	}
	else
	{
		targetActor->m_controller = nullptr;
	}

	return true;
}
