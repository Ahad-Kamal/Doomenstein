#include "Game/Gameplay/Actors/Player.hpp"
#include "Game/Gameplay/Game.hpp"
#include "Game/Gameplay/Entity.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
Player::Player( Vec3 const& startingPosition, EulerAngles const& orientation )
	: m_position( startingPosition )
	, m_orientation( orientation )
{
}

//-----------------------------------------------------------------------------------------------
Player::~Player()
{

}

//-----------------------------------------------------------------------------------------------
void Player::Update( [[maybe_unused]] float deltaSeconds )
{
	float systemDeltaSeconds = static_cast<float>( Clock::GetSystemClock().GetDeltaSeconds() );
	m_position += m_velocity;

	if( g_game->m_currentState == GAME_STATE_PLAY )
	{
		CameraControlsKeyboard( systemDeltaSeconds );
		CameraControlsController( systemDeltaSeconds );
	}

	g_game->m_worldCamera->SetPosition( m_position );
	g_game->m_worldCamera->SetOrientation( m_orientation );
}

//-----------------------------------------------------------------------------------------------
void Player::Render() const
{

}

//-----------------------------------------------------------------------------------------------
void Player::CameraControlsKeyboard( float deltaSeconds )
{
	// Note: remove later
	if( g_game->m_currentMap->m_isTestActor )
	{
		m_velocity = Vec3();
		return;
	}

	Vec3 forwardVector = m_orientation.GetForwardDir_IFwd_JLeft_KUp();
	float speedFactor = 1.f;

	if( g_engine->m_input->IsKeyDown( KEYCODE_SHIFT ) )
	{
		speedFactor *= 15.f;
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
		m_velocity.z += forwardVector.z * deltaSeconds * speedFactor;
	}
	if( g_engine->m_input->IsKeyDown( 'S' ) )
	{
		m_velocity.x -= forwardVector.x * deltaSeconds * speedFactor;
		m_velocity.y -= forwardVector.y * deltaSeconds * speedFactor;
		m_velocity.z -= forwardVector.z * deltaSeconds * speedFactor;
	}

	// Up and Down
	if( g_engine->m_input->IsKeyDown( 'Z' ) )
	{
		m_velocity.z += deltaSeconds * speedFactor;
	}
	if( g_engine->m_input->IsKeyDown( 'C' ) )
	{
		m_velocity.z -= deltaSeconds * speedFactor;
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

	// Roll
	if( g_engine->m_input->IsKeyDown( 'E' ) )
	{
		m_orientation.m_rollDegrees += 90.f * deltaSeconds;
		m_orientation.m_rollDegrees = GetClamped( m_orientation.m_rollDegrees, -45.f, 45.f );
	}
	if( g_engine->m_input->IsKeyDown( 'Q' ) )
	{
		m_orientation.m_rollDegrees -= 90.f * deltaSeconds;
		m_orientation.m_rollDegrees = GetClamped( m_orientation.m_rollDegrees, -45.f, 45.f );
	}

	// Reset
	if( g_engine->m_input->WasKeyJustPressed( 'H' ) )
	{
		m_position = Vec3();
		m_velocity = Vec3();
		m_orientation = EulerAngles();
	}
}

//-----------------------------------------------------------------------------------------------
void Player::CameraControlsController( float deltaSeconds )
{
	// Note: remove later
	if( g_game->m_currentMap->m_isTestActor )
	{
		m_velocity = Vec3();
		return;
	}

	XboxController const& controller = g_engine->m_input->m_controllers[ 0 ];
	Vec3 forwardVector = m_orientation.GetForwardDir_IFwd_JLeft_KUp();
	float speedFactor = 1.f;

	if( controller.GetButton( XboxButtonID::A ).m_isPressed )
	{
		speedFactor *= 15.f;
	}

	// Yaw
	if( g_engine->m_input->m_cursorState.m_cursorMode == CursorMode::FPS )
	{
		m_orientation.m_yawDegrees -= controller.GetRightStick().GetPosition().x * 0.125f;
	}

	// Pitch
	if( g_engine->m_input->m_cursorState.m_cursorMode == CursorMode::FPS )
	{
		m_orientation.m_pitchDegrees -= controller.GetRightStick().GetPosition().y * 0.125f;
		m_orientation.m_pitchDegrees = GetClamped( m_orientation.m_pitchDegrees, -85.f, 85.f );
	}

	// Roll
	if( controller.GetLeftTrigger() )
	{
		m_orientation.m_rollDegrees += 90.f * deltaSeconds;
		m_orientation.m_rollDegrees = GetClamped( m_orientation.m_rollDegrees, -45.f, 45.f );
	}
	if( controller.GetRightTrigger() )
	{
		m_orientation.m_rollDegrees -= 90.f * deltaSeconds;
		m_orientation.m_rollDegrees = GetClamped( m_orientation.m_rollDegrees, -45.f, 45.f );
	}

	// Up and Down
	if( controller.GetButton( XboxButtonID::RIGHT_BUMPER ).m_isPressed )
	{
		m_velocity.z += deltaSeconds * speedFactor;
	}
	if( controller.GetButton( XboxButtonID::LEFT_BUMPER ).m_isPressed )
	{
		m_velocity.z -= deltaSeconds * speedFactor;
	}

	// Left and Right
	if( controller.GetLeftStick().GetPosition().x > 0.5f )
	{
		m_velocity.x += forwardVector.GetRotated90DegreesAboutZ().x * deltaSeconds * speedFactor;
		m_velocity.y += forwardVector.GetRotated90DegreesAboutZ().y * deltaSeconds * speedFactor;

	}
	if( controller.GetLeftStick().GetPosition().x < -0.5f )
	{
		m_velocity.x -= forwardVector.GetRotated90DegreesAboutZ().x * deltaSeconds * speedFactor;
		m_velocity.y -= forwardVector.GetRotated90DegreesAboutZ().y * deltaSeconds * speedFactor;
	}

	// Forward and Back
	if( controller.GetLeftStick().GetPosition().y > 0.5f )
	{
		m_velocity.x += forwardVector.x * deltaSeconds * speedFactor;
		m_velocity.y += forwardVector.y * deltaSeconds * speedFactor;
		m_velocity.z += forwardVector.z * deltaSeconds * speedFactor;
	}
	if( controller.GetLeftStick().GetPosition().y < -0.5f ) 
	{
		m_velocity.x -= forwardVector.x * deltaSeconds * speedFactor;
		m_velocity.y -= forwardVector.y * deltaSeconds * speedFactor;
		m_velocity.z -= forwardVector.z * deltaSeconds * speedFactor;
	}

	if( controller.GetButton( XboxButtonID::START ).m_isPressed && ! controller.GetButton( XboxButtonID::START ).m_wasPressedLastFrame )
	{
		m_position = Vec3();
		m_velocity = Vec3();
		m_orientation = EulerAngles();
	}
}

//-----------------------------------------------------------------------------------------------
Mat44 Player::GetModelToWorldTransform() const
{
	Mat44 modelMatrix;
	modelMatrix.AppendTranslation3D( m_position );
	modelMatrix.Append( m_orientation.GetAsMatrix_IFwd_JLeft_KUp() );
	return modelMatrix;
}

