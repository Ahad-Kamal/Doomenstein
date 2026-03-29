#pragma once
#include "Game/Gameplay/Entity.hpp"


//-----------------------------------------------------------------------------------------------
struct EulerAngles;

//-----------------------------------------------------------------------------------------------
class Player
{
public:
	Player( Vec3 const& startingPosition, EulerAngles orientation );
	~Player();

	void Update( float deltaSeconds );
	void Render() const;

	void CameraControlsKeyboard( float deltaSeconds );
	void CameraControlsController( float deltaSeconds );

	Mat44 GetModelToWorldTransform() const;

public:
	Game*	m_game = nullptr;
	Vec3	m_position;
	Vec3	m_velocity;
	EulerAngles m_orientation;
	float	m_angualrVelocity = 0.f; // spin rate, in degrees per second
	Rgba8	m_color = Rgba8::WHITE;
};