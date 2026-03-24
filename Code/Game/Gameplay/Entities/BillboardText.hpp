#pragma once
#include <vector>
#include "Game/Gameplay/Entity.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"


//-----------------------------------------------------------------------------------------------
class Texture;
struct Vertex;
struct EulerAngles;

//-----------------------------------------------------------------------------------------------
class BillboardText : public Entity
{
public:
	BillboardText( Vec3 const& startingPosition, EulerAngles orientation, BillboardType mode = BillboardType::NONE );
	~BillboardText();

	void Update( float deltaSeconds ) override;
	void Render() const override;

public:
	std::vector<Vertex> m_vertexes;
	Rgba8				m_color = Rgba8::WHITE;
	Texture*			m_texture = nullptr;
	BillboardType		m_mode = BillboardType::NONE;
	Mat44				m_transform;
};