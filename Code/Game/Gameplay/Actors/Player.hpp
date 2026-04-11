#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Game/Gameplay/Actors/Actor.hpp"


//-----------------------------------------------------------------------------------------------
struct Mat44;
class ActorDefinition;

//-----------------------------------------------------------------------------------------------
class Player : public Actor
{
public:
	Player( Vec3 const& startingPosition, EulerAngles const& orientation );
	Player( Vec3 const& startingPosition, EulerAngles const& orientation, ActorDefinition* definition );
	~Player();

	void Update( float deltaSeconds ) override;
	void Render() const override;

	void UpdateFromKeyboard( float deltaSeconds );
	void UpdateFromController( float deltaSeconds );

	Mat44 GetModelToWorldTransform() const;

public:
//	Vec3	m_position;
	Vec3	m_velocity;
//	EulerAngles m_orientation;
	float	m_angualrVelocity = 0.f; // spin rate, in degrees per second
	Rgba8	m_color = Rgba8::WHITE;
};