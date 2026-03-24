#pragma once
#include "Game/Gameplay/Entity.hpp"


//-----------------------------------------------------------------------------------------------
struct EulerAngles;

//-----------------------------------------------------------------------------------------------
class Player : public Entity
{
public:
	Player( Vec3 const& startingPosition, EulerAngles orientation );
	~Player();

	void Update( float deltaSeconds ) override;
	void Render() const override;

	void CameraControlsKeyboard( float deltaSeconds );
	void CameraControlsController( float deltaSeconds );
};