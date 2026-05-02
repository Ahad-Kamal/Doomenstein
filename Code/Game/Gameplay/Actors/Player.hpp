#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Game/Gameplay/Actors/Actor.hpp"
#include "Game/Gameplay/Controller.hpp"
#include "Game/Framework/ActorHandle.hpp"


enum CameraMode
{
	CAMERA_MODE_FIRST_PERSON,
	CAMERA_MODE_FREE_FLY
};


//-----------------------------------------------------------------------------------------------
struct Mat44;
class ActorDefinition;
class Camera;

//-----------------------------------------------------------------------------------------------
class Player : public Controller
{
public:
	Player( Map* currentMap, ActorHandle actorToPossess, int playerNum = 1 );
	~Player();

	void Update( float deltaSeconds );
	void Render() const;

	void UpdateInput( float deltaSeconds );
	void UpdateCamera();

	virtual void Possess( ActorHandle actorToPossess ) override;
	void Unposses( ActorHandle actorToPossess );

	void SwitchWeapon( int weaponToSwitchTo );
	void WeaponKeyboardControls();
	void WeaponControllerControls();

	void FirstPersonKeyboardControls( float deltaSeconds );
	void FirstPersonControllerControls( float deltaSeconds );
	void FreeFlyKeyboardControls( float deltaSeconds );
	void FreeFlyControllerControls( float deltaSeconds );
	void SwitchCameraMode();

	Mat44 GetModelToWorldTransform() const;

public:
	Camera* m_camera;
	Vec3	m_position;
	EulerAngles m_orientation;
	CameraMode m_cameraMode = CAMERA_MODE_FIRST_PERSON;

	int m_kills = 0;
	int m_deaths = 0;
	int m_playerNum;
};