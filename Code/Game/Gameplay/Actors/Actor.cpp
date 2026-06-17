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
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>


//-----------------------------------------------------------------------------------------------
Actor::Actor( Vec3 const& startingPosition, EulerAngles const& orientation, ActorDefinition* definition, 
	ActorHandle actorHandle, Map* owningMap, AnimState startingState /*= WALK*/, bool isStatic /*= true*/, Rgba8 color /*= Rgba8::WHITE */)
	: m_position( startingPosition )
	, m_orientation( orientation )
	, m_isStatic( isStatic )
	, m_color( color )
	, m_definition( definition )
	, m_actorHandle( actorHandle )
	, m_map( owningMap )
	, m_currentAnim( startingState )
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

	m_animClock = new Clock( *g_game->m_gameClock );
	m_animTimer = new Timer( static_cast<double>( m_definition->GetAnimGroupByState( m_currentAnim )->GetDuration() ), m_animClock );
	m_animTimer->Start();
}

//-----------------------------------------------------------------------------------------------
Actor::~Actor()
{
	//NOTE: Change this check for multiplayer//-----------------------------------------------------------------------------------------------
	if( m_controller != m_map->m_player1 && m_controller != m_map->m_player2 )
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
	else if( m_isGarbage )
	{
		return;
	}
	else
	{
		if( m_deathTimer->HasPeriodElapsed() )
		{
			m_isGarbage = true;

			if( m_controller == m_map->m_player1 )
			{
				SpawnInfo newSpawnPoint = m_map->GetRandomSpawnPoint( Faction::MARINE );
				m_map->SpawnPlayer( "Marine", newSpawnPoint.m_position, newSpawnPoint.m_orientation, m_map->m_player1->m_playerNum );				
			}
			else if( m_controller == m_map->m_player2 && m_controller != nullptr )
			{
				SpawnInfo newSpawnPoint = m_map->GetRandomSpawnPoint( Faction::MARINE );
				m_map->SpawnPlayer( "Marine", newSpawnPoint.m_position, newSpawnPoint.m_orientation, m_map->m_player2->m_playerNum );
			}
		}
		else if( m_controller != nullptr && ( m_controller == m_map->m_player1 || m_controller == m_map->m_player2 ) )
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

	// Update Animations
	SpriteAnimationGroupDefinition animGroupDef = *m_definition->GetAnimGroupByState( m_currentAnim );
	if( animGroupDef.m_scaleBySpeed )
	{
		double timeScale = static_cast<double>( m_velocity.GetLength() / m_definition->GetPhysics().m_walkSpeed );
		m_animClock->SetTimeScale( timeScale );
	}
	else
	{
		m_animClock->SetTimeScale( g_game->m_gameClock->GetTimeScale() );
	}
	if( animGroupDef.m_playbackMode == SpriteAnimPlaybackType::ONCE && !m_isDead )
	{
		if( m_animTimer->HasPeriodElapsed() )
		{
			SwitchAnimState( AnimState::WALK );
		}
	}

	if( m_equippedWeapon != nullptr && m_equippedWeapon->m_currentAnim == AnimState::ATTACK && m_equippedWeapon->m_definition->GetType() != WEAPON_TYPE_MELEE )
	{
		WeaponAnimation weaponAnim = *m_equippedWeapon->m_definition->GetAnimationByState( AnimState::ATTACK );
		if( (float)m_equippedWeapon->m_animClock->GetTotalSeconds() >= m_equippedWeapon->m_definition->GetAnimationDuration( AnimState::ATTACK ) )
		{
			m_equippedWeapon->SwitchAnimState( AnimState::IDLE );
		}
	}

	// Update Sounds
	g_engine->m_audio->SetSoundPosition( m_soundEffect, m_position );
}

//-----------------------------------------------------------------------------------------------
void Actor::Render() const
{
	PlayerRenderPass renderPass = g_game->m_renderPass;
	Camera* worldCamera = nullptr;

	if( renderPass == PlayerRenderPass::PLAYER_ONE )
	{
		if( m_controller == m_map->m_player1 && m_map->m_player1->m_cameraMode == CAMERA_MODE_FIRST_PERSON )
		{
			return;
		}

		worldCamera = m_map->m_player1->m_camera;
	}
	if( renderPass == PlayerRenderPass::PLAYER_TWO )
	{
		if( m_controller == m_map->m_player2 )
		{
			return;
		}

		worldCamera = m_map->m_player2->m_camera;
	}
	if( !worldCamera )
	{
		return;
	}

	// Create Transform Matrix
	Mat44 transformMatrix;
	transformMatrix.AppendTranslation3D( m_position );
	transformMatrix.AppendZRotation( m_orientation.m_yawDegrees );
	
	// Create Vertex and Index Buffers
	VertexBuffer vertexBuffer = VertexBuffer( g_engine->m_render->GetDevice(), 12, sizeof( Vertex ) );
	IndexBuffer indexBuffer = IndexBuffer( g_engine->m_render->GetDevice(), 12 );
	vertexBuffer.Create();
	indexBuffer.Create();

	// Create Anim Def
	Visuals visuals = m_definition->GetVisuals();
	SpriteAnimationGroupDefinition animGroupDef = *m_definition->GetAnimGroupByState( m_currentAnim );

	// Get Billboard Transform
	Mat44 cameraToWorldTransform = worldCamera->GetCameraToWorldTransform();
	Mat44 billboardTransform = GetBillboardTransform( visuals.m_billboardType, cameraToWorldTransform, m_position );

	// Calculate Direction 
	Vec3 cameraToActor = m_position - cameraToWorldTransform.GetTranslation3D();
	Mat44 orthoInverse = GetModelToWorldTransform().GetOrthonormalInverse();
	Vec3 facingVector = orthoInverse.TransformVectorQuantity3D( cameraToActor );
	facingVector.Normalize();

	SpriteAnimDefinition animDef = animGroupDef.GetAnimationForDirection( facingVector );
	float framesPerSecond = 0.f;
	if( m_currentAnim != AnimState::IDLE )
	{
		framesPerSecond = animGroupDef.m_framesPerSecond;
	}
	SpriteAnimDefinition spriteAnim = SpriteAnimDefinition( *visuals.m_spriteSheet, animDef.GetStartIndex(), animDef.GetEndIndex(), framesPerSecond, animGroupDef.m_playbackMode );

	// Get UVs
	Texture* actorTexture = nullptr;
	AABB2 uvBox = AABB2::ZERO_TO_ONE;
	if( visuals.m_spriteSheet != nullptr )
	{
		const SpriteDefinition& spriteDef = spriteAnim.GetSpriteDefAtTime( static_cast<float>( m_animTimer->GetElaspedTime() ) );
		Vec2 uvMins, uvMaxs;
		spriteDef.GetUVs( uvMins, uvMaxs );
		uvBox = AABB2( uvMins, uvMaxs );
		actorTexture = &visuals.m_spriteSheet->GetTexture();
	}

	// Add Verts
	VertexList verts;
	IndexList indexes;
	float halfXSize = visuals.m_size.x * 0.5f;
	AddVertsForDoubleQuad3D( verts, indexes, Vec3( 0.f, -halfXSize, 0.f ), Vec3( 0.f, halfXSize, 0.f ), 
		Vec3( 0.f, halfXSize, visuals.m_size.y ), Vec3( 0.f, -halfXSize, visuals.m_size.y ), m_color, uvBox );

	// Draw Sprite
	if( m_definition->GetVisuals().m_renderLit )
	{
		Vec3 normalizedLighting = g_game->m_sunDirection.GetNormalized();
		SetLightConstants( normalizedLighting, g_game->m_sunIntensity, g_game->m_ambientIntensity );
	}

	g_engine->m_render->RenderSetup( actorTexture, BlendMode::ALPHA, billboardTransform, Rgba8::WHITE );
	g_engine->m_render->DrawVertexArray( verts, indexes, &vertexBuffer, &indexBuffer );

	//// Draw Wireframe
	g_engine->m_render->SetRasterizerState( RasterizerMode::WIREFRAME_CULL_BACK );
	g_engine->m_render->SetRasterizerStateIfChanged();

	g_engine->m_render->RenderSetup();
	
	VertexList tempWireframeVerts;
	AddVertsForCylinder3D( tempWireframeVerts, Vec3(), Vec3( 0.f, 0.f, 0.f + m_cosmeticHeight), m_cosmeticRadius, 16, m_color);
	TransformVertexArray3D( tempWireframeVerts, transformMatrix );
	g_engine->m_render->DrawVertexArray( tempWireframeVerts );

	g_engine->m_render->SetRasterizerState( RasterizerMode::SOLID_CULL_BACK );
	g_engine->m_render->SetRasterizerStateIfChanged();
}

//-----------------------------------------------------------------------------------------------
void Actor::RenderSetup()
{
	//AddVertsForCylinder3D( m_vertexes, Vec3(), Vec3( 0.f, 0.f, 0.f + m_cosmeticHeight), m_cosmeticRadius, 16, m_color);

  	//AddVertsForDoubleQuad3D( m_vertexes, m_indexes, Vec3( 0.f, -m_physicsRadius, 0.f ), Vec3( 0.f, m_physicsRadius, 0.f ), Vec3( 0.f, m_physicsRadius, m_physicsHeight ), Vec3( 0.f, -m_physicsRadius, m_physicsHeight ) );

	//Vec3 arrowStart = Vec3( m_cosmeticRadius, 0.f, m_definition->GetCameraView().m_eyeHeight );
	//Vec3 arrowEnd =  Vec3( m_cosmeticRadius + 0.075f, 0.f, m_definition->GetCameraView().m_eyeHeight );

	//Rgba8 lighterColor = Rgba8( (unsigned int)GetClamped( (float)( m_color.r + 150 ), 0.f, 255.f ), (unsigned int)GetClamped( (float)( m_color.g + 150 ), 0.f, 255.f ), (unsigned int)GetClamped( (float)( m_color.b + 150 ), 0.f, 255.f ) );
	//AddVertsForCylinder3D( m_wireframeVertexes, Vec3(), Vec3( 0.f, 0.f, 0.f + m_cosmeticHeight), m_cosmeticRadius, 16, lighterColor );

	//if( m_definition->GetName() != "PlasmaProjectile" )
	//{
	//	AddVertsForArrow3D( m_vertexes, arrowStart, arrowStart, arrowEnd, m_cosmeticRadius * 0.33f, 16, m_color );
	//	AddVertsForArrow3D( m_wireframeVertexes, arrowStart, arrowStart, arrowEnd, m_cosmeticRadius * 0.33f, 16, lighterColor );
	//}
}

//-----------------------------------------------------------------------------------------------
void Actor::SetColor( Rgba8 const& color )
{
	/*for( unsigned int vertIndex = 0; vertIndex < m_vertexes.size(); vertIndex++ )
	{
		m_vertexes[ vertIndex ].m_color = color;
	}*/

	m_color = color;
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
		m_health = 0;
		m_isDead = true;
		SwitchAnimState( AnimState::DEATH );

		if( m_definition->GetName() != "PlasmaProjectile" )
		{
			Rgba8 deadColor = Rgba8( (unsigned int)GetClamped( (float)( m_color.r - 100 ), 0.f, 255.f ), (unsigned int)GetClamped( (float)( m_color.g - 100 ), 0.f, 255.f ), (unsigned int)GetClamped( (float)( m_color.b - 100 ), 0.f, 255.f ) );
			SetColor( deadColor );
		}

		m_deathTimer = new Timer( m_definition->GetCorpseLifetime(), g_game->m_gameClock );
		m_deathTimer->Start();

		if( m_definition->GetFaction() == Faction::DEMON )
		{
			Actor* killer = m_map->GetActorByHandle( damagingActor );
			if( killer != nullptr && killer->m_controller == m_map->m_player1 )
			{
				m_map->m_player1->m_kills++;
			}
			else if( killer != nullptr && killer->m_controller == m_map->m_player2 )
			{
				m_map->m_player2->m_kills++;
			}
		}
		if( m_controller != nullptr && m_controller == m_map->m_player1 )
		{
			m_map->m_player1->m_deaths++;
		}
		else if( m_controller != nullptr && m_controller == m_map->m_player2 )
		{
			m_map->m_player2->m_deaths++;
		}

		// Death SFX
		if( m_controller != nullptr && ( m_controller == m_map->m_player1 || m_controller == m_map->m_player2 ) )
		{
			std::string sfxName = m_definition->GetSoundEffects().at( "Death" );
			SoundID sfx = g_engine->m_audio->CreateOrGetSound( sfxName );
			m_soundEffect = g_engine->m_audio->StartSound( sfx, false, 0.3f );
		}
		else if( m_controller != nullptr )
		{
			std::string sfxName = m_definition->GetSoundEffects().at( "Death" );
			SoundID sfx = g_engine->m_audio->CreateOrGetSound( sfxName, true );
			m_soundEffect = g_engine->m_audio->StartSoundAt( sfx, m_position, false, 0.6f );
		}
	}
	else
	{
		SwitchAnimState( AnimState::HURT );

		// Hurt SFX
		if( m_controller != nullptr && ( m_controller == m_map->m_player1 || m_controller == m_map->m_player2 ) )
		{
			if( !g_engine->m_audio->IsPlaying( m_soundEffect ) )
			{
				std::string sfxName = m_definition->GetSoundEffects().at( "Hurt" );
				SoundID sfx = g_engine->m_audio->CreateOrGetSound( sfxName );
				m_soundEffect = g_engine->m_audio->StartSound( sfx, false, 0.3f );
			}
		}
		else if( m_controller != nullptr )
		{
			if( !g_engine->m_audio->IsPlaying( m_soundEffect ) )
			{
				std::string sfxName = m_definition->GetSoundEffects().at( "Hurt" );
				SoundID sfx = g_engine->m_audio->CreateOrGetSound( sfxName, true );
				m_soundEffect = g_engine->m_audio->StartSoundAt( sfx, m_position, false, 0.6f );
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Actor::OnCollide( Actor* collidingActor )
{
	if( m_definition->GetCollision().m_dieOnCollide )
	{
		Damage( m_health, collidingActor->m_actorHandle );
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
void Actor::SwitchAnimState( AnimState newState )
{
	if( m_currentAnim != newState )
	{
		m_currentAnim = newState;

		m_animTimer->Stop();
		delete m_animTimer;
		m_animTimer = new Timer( static_cast<double>( m_definition->GetAnimGroupByState( m_currentAnim )->GetDuration() ), g_game->m_gameClock );
		m_animTimer->Start();
	}
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
		//NOTE: Change this check for multiplayer//-----------------------------------------------------------------------------------------------
		if( targetActor->m_actorHandle == g_game->m_currentMap->m_player1->m_actorHandle )
		{
			targetActor->m_controller = g_game->m_currentMap->m_player1;
			return true;
		}
		else if( targetActor->m_actorHandle == g_game->m_currentMap->m_player2->m_actorHandle )
		{
			targetActor->m_controller = g_game->m_currentMap->m_player2;
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
