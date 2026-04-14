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

	void UpdatePhysics( float deltaSeconds );
	void AddForce( Vec3 force );
	void AddImpulse( Vec3 impulse );

	void Damage( int incomingDamage );
	bool IsAlive() const;

	IntVec2 GetCoordsOfCurrentTile();
	Mat44 GetModelToWorldTransform() const;

	static bool Event_OnPossessed( EventArgs& args );
	static bool Event_OnUnpossessed( EventArgs& args );

public:
	VertexList	m_vertexes;
	VertexList	m_wireframeVertexes; // Note: delete this later

	Weapons		m_weapons;
	Controller* m_controller = nullptr;
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