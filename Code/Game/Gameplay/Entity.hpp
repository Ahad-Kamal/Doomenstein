#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"


//-----------------------------------------------------------------------------------------------
class Game;

//-----------------------------------------------------------------------------------------------
class Entity
{
public:
	Entity( Game* owner, Vec3 const& startPos );
	virtual ~Entity();

	virtual void Update( float deltaSeconds ) = 0;
	virtual void Render() const = 0;

	virtual Mat44 GetModelToWorldTransform() const;

public:
	Game*	m_game = nullptr;
	Vec3	m_position;
	Vec3	m_velocity;
	EulerAngles m_orientation;
	float	m_angualrVelocity = 0.f; // spin rate, in degrees per second
	Rgba8	m_color = Rgba8::WHITE;
};