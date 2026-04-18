#pragma once
#include "Game/Framework/ActorHandle.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
struct Mat44;
struct IntVec2;
class ActorDefinition;
class NamedStrings;
class Controller;
class Map;
class Weapon;
class AI;

//-----------------------------------------------------------------------------------------------
typedef NamedStrings EventArgs;
typedef std::vector<Vertex> VertexList;
typedef std::vector<Weapon*> Weapons;

//-----------------------------------------------------------------------------------------------
class Actor
{
public:
	Actor( Vec3 const& startingPosition, EulerAngles const& orientation, ActorDefinition* definition, ActorHandle actorHandle, Map* owningMap, bool isStatic = true, Rgba8 color = Rgba8::WHITE );
	~Actor();

	virtual void Update( float deltaSeconds );
	virtual void Render() const;

	void RenderSetup();
	void SetColor( Rgba8 const& color );

	void UpdatePhysics( float deltaSeconds );
	void AddForce( Vec3 const& force );
	void AddImpulse( Vec3 const& impulse );
	void AddDirection( float yawDegrees );
	void MoveInDirection( Vec3 const& direction, float speed );
	void TurnInDirection( Vec3 const& direction, float maxAmt );

	void Damage( int incomingDamage, ActorHandle damagingActor );
	void OnCollide( Actor* collidingActor );
	void EquipWeapon( int weaponToSwitchTo );
	bool IsAlive() const;

	IntVec2 GetCoordsOfCurrentTile() const;
	Mat44 GetModelToWorldTransform() const;

	static bool Event_OnPossessed( EventArgs& args );
	static bool Event_OnUnpossessed( EventArgs& args );

public:
	VertexList	m_vertexes;
	VertexList	m_wireframeVertexes; // Note: delete this later

	Weapons		m_weapons;
	Weapon*		m_equippedWeapon = nullptr;
	Controller* m_controller = nullptr;
	AI*			m_ai = nullptr;
	Actor*		m_owner = nullptr;
	
	ActorDefinition* m_definition;
	Map*		m_map;

	ActorHandle m_actorHandle;
	Vec3		m_position;
	Vec3		m_velocity;
	Vec3		m_acceleration;
	EulerAngles m_orientation;
	Rgba8		m_color;

	int			m_health;

	float		m_physicsHeight;
	float		m_cosmeticHeight;
	float		m_physicsRadius;
	float		m_cosmeticRadius;

	bool		m_isStatic;
	bool		m_isDead = false;
	bool		m_isGarbage = false;
};