#pragma once
#include "Game/Framework/ActorHandle.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/Rgba8.hpp"


//-----------------------------------------------------------------------------------------------
struct Mat44;
struct IntVec2;
class ActorDefinition;
class NamedStrings;
class Controller;
class Map;

//-----------------------------------------------------------------------------------------------
typedef NamedStrings EventArgs;

//-----------------------------------------------------------------------------------------------
class Actor
{
public:
	// Note: to be deprecated
	/*Actor( Vec3 const& startingPosition, EulerAngles const& orientation, bool isStatic = true, Rgba8 color = Rgba8::WHITE );
	Actor( Vec3 const& startingPosition, EulerAngles const& orientation, float physicsHeight,
		float physicsRadius, bool isStatic = true, Rgba8 color = Rgba8::WHITE );
	Actor( Vec3 const& startingPosition, EulerAngles const& orientation, float physicsHeight, float cosmeticHeight,
		float physicsRadius, float cosmeticRadius, bool isStatic = true, Rgba8 color = Rgba8::WHITE );*/
	//
	Actor( Vec3 const& startingPosition, EulerAngles const& orientation, ActorDefinition* definition, ActorHandle actorHandle, Map* owningMap, bool isStatic = true, Rgba8 color = Rgba8::WHITE );
	~Actor();

	virtual void Update( float deltaSeconds );
	virtual void Render() const;

	bool IsAlive() const;

	IntVec2 GetCoordsOfCurrentTile();
	Mat44 GetModelToWorldTransform() const;

	static bool Event_OnPossessed( EventArgs& args );
	static bool Event_OnUnpossessed( EventArgs& args );

public:
	ActorDefinition* m_definition;
	Controller* m_controller = nullptr;
	Map*		m_map;

	ActorHandle m_actorHandle;
	Vec3		m_position;
	EulerAngles m_orientation;
	Rgba8		m_color;

	float		m_physicsHeight;
	float		m_cosmeticHeight;
	float		m_physicsRadius;
	float		m_cosmeticRadius;

	bool		m_isStatic;
	bool		m_isDead = false;
	bool		m_isGarbage = false;
};