#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/Rgba8.hpp"

//-----------------------------------------------------------------------------------------------
struct Mat44;
struct IntVec2;

//-----------------------------------------------------------------------------------------------
class Actor
{
public:
	Actor( Vec3 const& startingPosition, EulerAngles const& orientation, bool isStatic = true, Rgba8 color = Rgba8::WHITE );
	Actor( Vec3 const& startingPosition, EulerAngles const& orientation, float physicsHeight,
		float physicsRadius, bool isStatic = true, Rgba8 color = Rgba8::WHITE );
	Actor( Vec3 const& startingPosition, EulerAngles const& orientation, float physicsHeight, float cosmeticHeight,
		float physicsRadius, float cosmeticRadius, bool isStatic = true, Rgba8 color = Rgba8::WHITE );
	~Actor();

	void Update( float deltaSeconds );
	void Render() const;

	IntVec2 GetCoordsOfCurrentTile();
	Mat44 GetModelToWorldTransform() const;

public:
	Vec3		m_position;
	EulerAngles m_orientation;
	Rgba8		m_color;
	float		m_physicsHeight;
	float		m_cosmeticHeight;
	float		m_physicsRadius;
	float		m_cosmeticRadius;
	bool		m_isStatic;
};