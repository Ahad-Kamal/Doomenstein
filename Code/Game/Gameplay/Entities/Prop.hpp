#pragma once
#include <vector>
#include "Game/Gameplay/Entity.hpp"
#include "Engine/Core/Rgba8.hpp"


//-----------------------------------------------------------------------------------------------
class Texture;
struct Vertex;
struct EulerAngles;

//-----------------------------------------------------------------------------------------------
class Prop : public Entity
{
public:
	Prop( Vec3 const& startingPosition, EulerAngles orientation );
	~Prop();

	void Update( float deltaSeconds ) override;
	void Render() const override;

public:
	std::vector<Vertex> m_vertexes;
	Rgba8				m_color = Rgba8::WHITE;
	Texture*			m_texture = nullptr;
};