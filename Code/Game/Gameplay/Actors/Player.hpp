#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Game/Gameplay/Actors/Actor.hpp"
#include "Game/Gameplay/Controller.hpp"
#include "Game/Framework/ActorHandle.hpp"


//-----------------------------------------------------------------------------------------------
struct Mat44;
class ActorDefinition;
class Camera;

//-----------------------------------------------------------------------------------------------
class Player : public Controller
{
public:
	Player( Map* currentMap, ActorHandle actorToPossess );
	~Player();

	void Update( float deltaSeconds );
	void Render() const;

	void UpdateFromKeyboard( float deltaSeconds );
	void UpdateFromController( float deltaSeconds );
	void FreeFlyKeyboardControls( float deltaSeconds );
	void FreeFlyControllerControls( float deltaSeconds );

	Mat44 GetModelToWorldTransform() const;

public:
	Camera* m_camera;
	Vec3	m_position;
	Vec3	m_velocity;
	EulerAngles m_orientation;
	bool	m_isFreeFly = false;
};