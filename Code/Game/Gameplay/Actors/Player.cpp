#include "Game/Gameplay/Actors/Player.hpp"
#include "Game/Gameplay/ActorDefinition.hpp"
#include "Game/Gameplay/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
Player::Player( Vec3 const& startingPosition, EulerAngles const& orientation )
	: Actor( startingPosition, orientation, false )
{
}

//-----------------------------------------------------------------------------------------------
Player::Player( Vec3 const& startingPosition, EulerAngles const& orientation, ActorDefinition* definition )
	: Actor( startingPosition, orientation, definition, false )
{
}

//-----------------------------------------------------------------------------------------------
Player::~Player()
{

}

//-----------------------------------------------------------------------------------------------
void Player::Update( float deltaSeconds )
{
	m_position += m_velocity;

	if( g_game->m_currentState == GAME_STATE_PLAY )
	{
		UpdateFromKeyboard( deltaSeconds );
		UpdateFromController( deltaSeconds );
		FreeFlyKeyboardControls( deltaSeconds );
		FreeFlyControllerControls( deltaSeconds );
	}

	if( !m_isFreeFly )
	{
		g_game->m_worldCamera->SetPosition( m_position );
		g_game->m_worldCamera->SetOrientation( m_orientation );
	}
}

//-----------------------------------------------------------------------------------------------
void Player::Render() const
{

}

//-----------------------------------------------------------------------------------------------
void Player::UpdateFromKeyboard( float deltaSeconds )
{
	if( m_isFreeFly )
	{
		m_velocity = Vec3();
		return;
	}

	// Yaw
	if( g_engine->m_input->m_cursorState.m_cursorMode == CursorMode::FPS )
	{
		m_orientation.m_yawDegrees -= g_engine->m_input->m_cursorState.m_cursorClientDelta.x * 0.075f;
	}

	// Pitch
	if( g_engine->m_input->m_cursorState.m_cursorMode == CursorMode::FPS )
	{
		m_orientation.m_pitchDegrees += g_engine->m_input->m_cursorState.m_cursorClientDelta.y * 0.075f;
		m_orientation.m_pitchDegrees = GetClamped( m_orientation.m_pitchDegrees, -85.f, 85.f );
	}

	Vec3 forwardVector = m_orientation.GetForwardDir_IFwd_JLeft_KUp();
	float speedFactor = m_definition->GetPhysics().m_walkSpeed;

	if( g_engine->m_input->IsKeyDown( KEYCODE_SHIFT ) )
	{
		speedFactor = m_definition->GetPhysics().m_runSpeed;
	}

	m_velocity = Vec3();

	// Left and Right
	if( g_engine->m_input->IsKeyDown( 'A' ) )
	{
		m_velocity.x += forwardVector.GetRotated90DegreesAboutZ().x * deltaSeconds * speedFactor;
		m_velocity.y += forwardVector.GetRotated90DegreesAboutZ().y * deltaSeconds * speedFactor;

	}
	if( g_engine->m_input->IsKeyDown( 'D' ) )
	{
		m_velocity.x -= forwardVector.GetRotated90DegreesAboutZ().x * deltaSeconds * speedFactor;
		m_velocity.y -= forwardVector.GetRotated90DegreesAboutZ().y * deltaSeconds * speedFactor;
	}

	// Forward and Back
	if( g_engine->m_input->IsKeyDown( 'W' ) )
	{
		m_velocity.x += forwardVector.x * deltaSeconds * speedFactor;
		m_velocity.y += forwardVector.y * deltaSeconds * speedFactor;
	}
	if( g_engine->m_input->IsKeyDown( 'S' ) )
	{
		m_velocity.x -= forwardVector.x * deltaSeconds * speedFactor;
		m_velocity.y -= forwardVector.y * deltaSeconds * speedFactor;
	}
}

//-----------------------------------------------------------------------------------------------
void Player::UpdateFromController( float deltaSeconds )
{
	if( m_isFreeFly )
	{
		m_velocity = Vec3();
		return;
	}

	XboxController const& controller = g_engine->m_input->m_controllers[ 0 ];
	// Yaw
	if( g_engine->m_input->m_cursorState.m_cursorMode == CursorMode::FPS )
	{
		m_orientation.m_yawDegrees -= controller.GetRightStick().GetPosition().x * 0.075f * ( m_definition->GetPhysics().m_turnSpeed * 0.016667f );
	}

	// Pitch
	if( g_engine->m_input->m_cursorState.m_cursorMode == CursorMode::FPS )
	{
		m_orientation.m_pitchDegrees -= controller.GetRightStick().GetPosition().y * 0.075f * ( m_definition->GetPhysics().m_turnSpeed * 0.016667f );
		m_orientation.m_pitchDegrees = GetClamped( m_orientation.m_pitchDegrees, -85.f, 85.f );
	}

	Vec3 forwardVector = m_orientation.GetForwardDir_IFwd_JLeft_KUp();
	float speedFactor = m_definition->GetPhysics().m_walkSpeed;

	if( controller.GetButton( XboxButtonID::A ).m_isPressed )
	{
		speedFactor = m_definition->GetPhysics().m_runSpeed;
	}

	// Left and Right
	if( controller.GetLeftStick().GetPosition().x < 0.5f )
	{
		m_velocity.x += forwardVector.GetRotated90DegreesAboutZ().x * deltaSeconds * speedFactor;
		m_velocity.y += forwardVector.GetRotated90DegreesAboutZ().y * deltaSeconds * speedFactor;

	}
	if( controller.GetLeftStick().GetPosition().x > -0.5f )
	{
		m_velocity.x -= forwardVector.GetRotated90DegreesAboutZ().x * deltaSeconds * speedFactor;
		m_velocity.y -= forwardVector.GetRotated90DegreesAboutZ().y * deltaSeconds * speedFactor;
	}

	// Forward and Back
	if( controller.GetLeftStick().GetPosition().y > 0.5f )
	{
		m_velocity.x += forwardVector.x * deltaSeconds * speedFactor;
		m_velocity.y += forwardVector.y * deltaSeconds * speedFactor;
	}
	if( controller.GetLeftStick().GetPosition().y < -0.5f ) 
	{
		m_velocity.x -= forwardVector.x * deltaSeconds * speedFactor;
		m_velocity.y -= forwardVector.y * deltaSeconds * speedFactor;
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

	Vec3 currentCameraPosition = g_game->m_worldCamera->GetPosition();
	g_game->m_worldCamera->SetPosition( currentCameraPosition + velocity );
	g_game->m_worldCamera->SetOrientation( worldCameraOrientation );
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

	Vec3 currentCameraPosition = g_game->m_worldCamera->GetPosition();
	g_game->m_worldCamera->SetPosition( currentCameraPosition + velocity );
	g_game->m_worldCamera->SetOrientation( worldCameraOrientation );
}

//-----------------------------------------------------------------------------------------------
Mat44 Player::GetModelToWorldTransform() const
{
	Mat44 modelMatrix;
	modelMatrix.AppendTranslation3D( m_position );
	modelMatrix.Append( m_orientation.GetAsMatrix_IFwd_JLeft_KUp() );
	return modelMatrix;
}

