#include "Game/Gameplay/Actors/Player.hpp"
#include "Game/Gameplay/ActorDefinition.hpp"
#include "Game/Gameplay/Game.hpp"
#include "Game/Gameplay/Weapon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
Player::Player( Map* currentMap, ActorHandle actorToPossess )
	: Controller( currentMap, actorToPossess )
{
	Actor* possessedActor = GetActor();
	ActorDefinition actorDef = *possessedActor->m_definition;

	Vec3 actorPosition = possessedActor->m_position;
	actorPosition.z = actorDef.GetCameraView().m_eyeHeight;
	m_position = actorPosition;
	m_orientation = possessedActor->m_orientation;

	m_camera = new Camera;
	m_camera->SetPerspectiveView( 2.f, actorDef.GetCameraView().m_cameraFOVDegrees, 0.1f, 1000.f );
}

//-----------------------------------------------------------------------------------------------
Player::~Player()
{

}

//-----------------------------------------------------------------------------------------------
void Player::Update( float deltaSeconds )
{
	if( g_game->m_currentState == GAME_STATE_PLAY )
	{
		UpdateInput( deltaSeconds );
		UpdateCamera();
	}
}

//-----------------------------------------------------------------------------------------------
void Player::Render() const
{

}

//-----------------------------------------------------------------------------------------------
void Player::UpdateInput( float deltaSeconds )
{
	if( g_game->m_currentState != GAME_STATE_PLAY )
	{
		return;
	}

	FirstPersonKeyboardControls( deltaSeconds );
	FirstPersonControllerControls( deltaSeconds );

	float systemDeltaTime = static_cast<float>( Clock::GetSystemClock().GetDeltaSeconds() );
	FreeFlyKeyboardControls( systemDeltaTime );
	FreeFlyControllerControls( systemDeltaTime );

	WeaponKeyboardControls();
}

//-----------------------------------------------------------------------------------------------
void Player::UpdateCamera()
{
	if( !m_isFreeFly )
	{
		Actor* possessedActor = GetActor();
		Vec3 actorPosition = possessedActor->m_position;
		actorPosition.z = possessedActor->m_definition->GetCameraView().m_eyeHeight;
		m_position = actorPosition;
		m_orientation = possessedActor->m_orientation;

		m_camera->SetPosition( m_position );
		m_camera->SetOrientation( m_orientation );
	}
}

//-----------------------------------------------------------------------------------------------
void Player::Possess( ActorHandle actorToPossess )
{
	// Note: add logic notifying actor that its been unpossessed
	if( m_actorHandle != actorToPossess )
	{
		EventArgs args;
		args.SetValue( "ActorIndex", Stringf( "%u", m_actorHandle.GetIndex() ) );
		FireEvent( "Unpossess", args );
	}

	// Note: add logic notifying actor that its been possessed
	m_actorHandle = actorToPossess;
	EventArgs args;
	args.SetValue( "ActorIndex", Stringf( "%u", m_actorHandle.GetIndex() ) );
	args.SetValue( "ControllerType", "Player" );
	FireEvent( "Possess", args );
}

//-----------------------------------------------------------------------------------------------
void Player::SwitchWeapon( int weaponToSwitchTo )
{
	Actor* possessedActor = GetActor();
	Weapons actorWeapons = possessedActor->m_weapons;

	if( weaponToSwitchTo >= actorWeapons.size() )
	{
		weaponToSwitchTo = 0;
	}
	else if( weaponToSwitchTo <= -1 )
	{
		weaponToSwitchTo = static_cast<int>( actorWeapons.size() ) - 1;
	}

	possessedActor->m_equippedWeapon = actorWeapons[ weaponToSwitchTo ];
}

//-----------------------------------------------------------------------------------------------
void Player::WeaponKeyboardControls()
{
	if( g_engine->m_input->WasKeyJustPressed( KEYCODE_LEFT_MOUSE ) && !m_isFreeFly )
	{
		Actor* possessedActor = GetActor();
		possessedActor->m_equippedWeapon->Fire( possessedActor );
	}

	if( g_engine->m_input->WasKeyJustPressed( KEYCODE_LEFT_ARROW ) )
	{
		Actor* possessedActor = GetActor();
		Weapons actorWeapons = possessedActor->m_weapons;
		for( unsigned int weaponIndex = 0; weaponIndex < actorWeapons.size(); weaponIndex++ )
		{
			if( possessedActor->m_equippedWeapon == actorWeapons[ weaponIndex ] )
			{
				SwitchWeapon( weaponIndex - 1 );
				break;
			}
		}
	}
	if( g_engine->m_input->WasKeyJustPressed( KEYCODE_RIGHT_ARROW ) )
	{
		Actor* possessedActor = GetActor();
		Weapons actorWeapons = possessedActor->m_weapons;
		for( unsigned int weaponIndex = 0; weaponIndex < actorWeapons.size(); weaponIndex++ )
		{
			if( possessedActor->m_equippedWeapon == actorWeapons[ weaponIndex ] )
			{
				SwitchWeapon( weaponIndex + 1 );
				break;
			}
		}
	}
	if( g_engine->m_input->WasKeyJustPressed( '1' ) )
	{
		Actor* possessedActor = GetActor();
		Weapons actorWeapons = possessedActor->m_weapons;
		for( unsigned int weaponIndex = 0; weaponIndex < actorWeapons.size(); weaponIndex++ )
		{
			if( possessedActor->m_equippedWeapon == actorWeapons[ weaponIndex ] )
			{
				SwitchWeapon( 0 );
				break;
			}
		}
	}
	if( g_engine->m_input->WasKeyJustPressed( '2' ) )
	{
		Actor* possessedActor = GetActor();
		Weapons actorWeapons = possessedActor->m_weapons;
		for( unsigned int weaponIndex = 0; weaponIndex < actorWeapons.size(); weaponIndex++ )
		{
			if( possessedActor->m_equippedWeapon == actorWeapons[ weaponIndex ] )
			{
				SwitchWeapon( 1 );
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Player::FirstPersonKeyboardControls( [[maybe_unused]] float deltaSeconds )
{
	Actor* possesedActor = GetActor();
	ActorDefinition definition = *possesedActor->m_definition;

	if( m_isFreeFly )
	{
		possesedActor->m_velocity = Vec3();
		return;
	}

	// Yaw
	if( g_engine->m_input->m_cursorState.m_cursorMode == CursorMode::FPS )
	{
		possesedActor->TurnInDirection( -g_engine->m_input->m_cursorState.m_cursorClientDelta.x * 0.075f );
	}

	// Pitch
	if( g_engine->m_input->m_cursorState.m_cursorMode == CursorMode::FPS )
	{
		m_orientation.m_pitchDegrees += g_engine->m_input->m_cursorState.m_cursorClientDelta.y * 0.075f;
		m_orientation.m_pitchDegrees = GetClamped( m_orientation.m_pitchDegrees, -85.f, 85.f );
	}

	Vec3 forwardVector = m_orientation.GetForwardDir_IFwd_JLeft_KUp();
	float speedFactor = definition.GetPhysics().m_walkSpeed ;

	if( g_engine->m_input->IsKeyDown( KEYCODE_SHIFT ) )
	{
		speedFactor = definition.GetPhysics().m_runSpeed ;
	}

	// Left and Right
	if( g_engine->m_input->IsKeyDown( 'A' ) )
	{
		Vec3 direction = Vec3( forwardVector.GetRotated90DegreesAboutZ().x, forwardVector.GetRotated90DegreesAboutZ().y, 0.f );
		possesedActor->MoveInDirection( direction, speedFactor );
	}
	if( g_engine->m_input->IsKeyDown( 'D' ) )
	{
		Vec3 direction = Vec3( -forwardVector.GetRotated90DegreesAboutZ().x, -forwardVector.GetRotated90DegreesAboutZ().y, 0.f );
		possesedActor->MoveInDirection( direction, speedFactor );
	}

	// Forward and Back
	if( g_engine->m_input->IsKeyDown( 'W' ) )
	{
		Vec3 direction = Vec3( forwardVector.x, forwardVector.y, 0.f );
		possesedActor->MoveInDirection( direction, speedFactor );
	}
	if( g_engine->m_input->IsKeyDown( 'S' ) )
	{
		Vec3 direction = Vec3( -forwardVector.x, -forwardVector.y, 0.f );
		possesedActor->MoveInDirection( direction, speedFactor );
	}
}

//-----------------------------------------------------------------------------------------------
void Player::FirstPersonControllerControls( float deltaSeconds )
{
	Actor* possesedActor = GetActor();
	ActorDefinition definition = *GetActor()->m_definition;

	if( m_isFreeFly )
	{
		possesedActor->m_velocity = Vec3();
		return;
	}

	XboxController const& controller = g_engine->m_input->m_controllers[ 0 ];
	// Yaw
	if( g_engine->m_input->m_cursorState.m_cursorMode == CursorMode::FPS )
	{
		possesedActor->m_orientation.m_yawDegrees -= controller.GetRightStick().GetPosition().x * 0.075f * ( definition.GetPhysics().m_turnSpeed * 0.016667f );
	}

	// Pitch
	if( g_engine->m_input->m_cursorState.m_cursorMode == CursorMode::FPS )
	{
		possesedActor->m_orientation.m_pitchDegrees -= controller.GetRightStick().GetPosition().y * 0.075f * ( definition.GetPhysics().m_turnSpeed * 0.016667f );
		possesedActor->m_orientation.m_pitchDegrees = GetClamped( m_orientation.m_pitchDegrees, -85.f, 85.f );
	}

	Vec3 forwardVector = m_orientation.GetForwardDir_IFwd_JLeft_KUp();
	float speedFactor = definition.GetPhysics().m_walkSpeed;

	if( controller.GetButton( XboxButtonID::A ).m_isPressed )
	{
		speedFactor = definition.GetPhysics().m_runSpeed;
	}

	// Left and Right
	if( controller.GetLeftStick().GetPosition().x < 0.5f )
	{
		possesedActor->m_velocity.x += forwardVector.GetRotated90DegreesAboutZ().x * deltaSeconds * speedFactor;
		possesedActor->m_velocity.y += forwardVector.GetRotated90DegreesAboutZ().y * deltaSeconds * speedFactor;

	}
	if( controller.GetLeftStick().GetPosition().x > -0.5f )
	{
		possesedActor->m_velocity.x -= forwardVector.GetRotated90DegreesAboutZ().x * deltaSeconds * speedFactor;
		possesedActor->m_velocity.y -= forwardVector.GetRotated90DegreesAboutZ().y * deltaSeconds * speedFactor;
	}

	// Forward and Back
	if( controller.GetLeftStick().GetPosition().y > 0.5f )
	{
		possesedActor->m_velocity.x += forwardVector.x * deltaSeconds * speedFactor;
		possesedActor->m_velocity.y += forwardVector.y * deltaSeconds * speedFactor;
	}
	if( controller.GetLeftStick().GetPosition().y < -0.5f ) 
	{
		possesedActor->m_velocity.x -= forwardVector.x * deltaSeconds * speedFactor;
		possesedActor->m_velocity.y -= forwardVector.y * deltaSeconds * speedFactor;
	}
}

//-----------------------------------------------------------------------------------------------
void Player::FreeFlyKeyboardControls( float deltaSeconds )
{
	if( g_game->m_currentState != GAME_STATE_PLAY )
	{
		return;
	}

	if( !m_isFreeFly )
	{
		return;
	}

	EulerAngles worldCameraOrientation = g_game->m_worldCamera->GetOrientation();
	Vec3 forwardVector = worldCameraOrientation.GetForwardDir_IFwd_JLeft_KUp();
	Vec3 velocity = Vec3();

	float speedFactor = 1.f;
	if( g_engine->m_input->IsKeyDown( KEYCODE_SHIFT ) )
	{
		speedFactor *= 15.f;
	}

	// Yaw
	if( g_engine->m_input->m_cursorState.m_cursorMode == CursorMode::FPS )
	{
		worldCameraOrientation.m_yawDegrees -= g_engine->m_input->m_cursorState.m_cursorClientDelta.x * 0.075f;
	}

	// Pitch
	if( g_engine->m_input->m_cursorState.m_cursorMode == CursorMode::FPS )
	{
		worldCameraOrientation.m_pitchDegrees += g_engine->m_input->m_cursorState.m_cursorClientDelta.y * 0.075f;
		worldCameraOrientation.m_pitchDegrees = GetClamped( worldCameraOrientation.m_pitchDegrees, -85.f, 85.f );
	}

	// Left and Right
	if( g_engine->m_input->IsKeyDown( 'A' ) )
	{
		velocity.x += forwardVector.GetRotated90DegreesAboutZ().x * deltaSeconds * speedFactor;
		velocity.y += forwardVector.GetRotated90DegreesAboutZ().y * deltaSeconds * speedFactor;
	}

	if( g_engine->m_input->IsKeyDown( 'D' ) )
	{
		velocity.x -= forwardVector.GetRotated90DegreesAboutZ().x * deltaSeconds * speedFactor;
		velocity.y -= forwardVector.GetRotated90DegreesAboutZ().y * deltaSeconds * speedFactor;
	}

	// Forward and Back
	if( g_engine->m_input->IsKeyDown( 'W' ) )
	{
		velocity.x += forwardVector.x * deltaSeconds * speedFactor;
		velocity.y += forwardVector.y * deltaSeconds * speedFactor;
		velocity.z += forwardVector.z * deltaSeconds * speedFactor;
	}
	if( g_engine->m_input->IsKeyDown( 'S' ) )
	{
		velocity.x -= forwardVector.x * deltaSeconds * speedFactor;
		velocity.y -= forwardVector.y * deltaSeconds * speedFactor;
		velocity.z -= forwardVector.z * deltaSeconds * speedFactor;
	}

	// Up and Down
	if( g_engine->m_input->IsKeyDown( 'Z' ) )
	{
		velocity.z += deltaSeconds * speedFactor;
	}
	if( g_engine->m_input->IsKeyDown( 'C' ) )
	{
		velocity.z -= deltaSeconds * speedFactor;
	}

	Vec3 currentCameraPosition = m_camera->GetPosition();
	m_camera->SetPosition( currentCameraPosition + velocity );
	m_camera->SetOrientation( worldCameraOrientation );
}

//-----------------------------------------------------------------------------------------------
void Player::FreeFlyControllerControls( float deltaSeconds )
{
	if( g_game->m_currentState != GAME_STATE_PLAY )
	{
		return;
	}

	if( g_engine->m_input->WasKeyJustPressed( 'F' ) )
	{
		m_isFreeFly = !m_isFreeFly;
	}

	if( !m_isFreeFly )
	{
		return;
	}

	XboxController const& controller = g_engine->m_input->m_controllers[ 0 ];

	EulerAngles worldCameraOrientation = g_game->m_worldCamera->GetOrientation();
	Vec3 forwardVector = worldCameraOrientation.GetForwardDir_IFwd_JLeft_KUp();
	Vec3 velocity = Vec3();

	float speedFactor = 1.f;
	if( controller.GetButton( XboxButtonID::A ).m_isPressed )
	{
		speedFactor *= 15.f;
	}

	// Yaw
	if( g_engine->m_input->m_cursorState.m_cursorMode == CursorMode::FPS )
	{
		worldCameraOrientation.m_yawDegrees -= controller.GetRightStick().GetPosition().x * 0.075f * 3.f;
	}

	// Pitch
	if( g_engine->m_input->m_cursorState.m_cursorMode == CursorMode::FPS )
	{
		worldCameraOrientation.m_pitchDegrees -= controller.GetRightStick().GetPosition().y * 0.075f * 3.f;
		worldCameraOrientation.m_pitchDegrees = GetClamped( worldCameraOrientation.m_pitchDegrees, -85.f, 85.f );
	}

	// Left and Right
	if( controller.GetLeftStick().GetPosition().x < 0.5f )
	{
		velocity.x += forwardVector.GetRotated90DegreesAboutZ().x * deltaSeconds * speedFactor;
		velocity.y += forwardVector.GetRotated90DegreesAboutZ().y * deltaSeconds * speedFactor;
	}

	if( controller.GetLeftStick().GetPosition().x > -0.5f )
	{
		velocity.x -= forwardVector.GetRotated90DegreesAboutZ().x * deltaSeconds * speedFactor;
		velocity.y -= forwardVector.GetRotated90DegreesAboutZ().y * deltaSeconds * speedFactor;
	}

	// Forward and Back
	if( controller.GetLeftStick().GetPosition().y > 0.5f )
	{
		velocity.x += forwardVector.x * deltaSeconds * speedFactor;
		velocity.y += forwardVector.y * deltaSeconds * speedFactor;
		velocity.z += forwardVector.z * deltaSeconds * speedFactor;
	}
	if( controller.GetLeftStick().GetPosition().y < -0.5f )
	{
		velocity.x -= forwardVector.x * deltaSeconds * speedFactor;
		velocity.y -= forwardVector.y * deltaSeconds * speedFactor;
		velocity.z -= forwardVector.z * deltaSeconds * speedFactor;
	}

	// Up and Down
	if( controller.IsButtonDown( XboxButtonID::LEFT_BUMPER ) )
	{
		velocity.z += deltaSeconds * speedFactor;
	}
	if( controller.IsButtonDown( XboxButtonID::RIGHT_BUMPER ) )
	{
		velocity.z -= deltaSeconds * speedFactor;
	}

	Vec3 currentCameraPosition = m_camera->GetPosition();
	m_camera->SetPosition( currentCameraPosition + velocity );
	m_camera->SetOrientation( worldCameraOrientation );
}

//-----------------------------------------------------------------------------------------------
Mat44 Player::GetModelToWorldTransform() const
{
	Mat44 modelMatrix;
	modelMatrix.AppendTranslation3D( m_position );
	modelMatrix.Append( m_orientation.GetAsMatrix_IFwd_JLeft_KUp() );
	return modelMatrix;
}

