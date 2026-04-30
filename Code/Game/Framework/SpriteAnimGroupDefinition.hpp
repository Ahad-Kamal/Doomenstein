#pragma once
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class SpriteAnimDefinition;

//-----------------------------------------------------------------------------------------------
class SpriteAnimationGroupDefinition
{
public:
	bool LoadFromXmlElement( XmlElement const& element, SpriteSheet const& spriteSheet );

	const SpriteAnimDefinition& GetAnimationForDirection( Vec3 const& direction ) const;
	float GetDuration() const;

public:
	std::string m_name;
	std::vector<Vec3> m_directions;
	std::vector<SpriteAnimDefinition> m_spriteAnimationDefinitions;
	SpriteAnimPlaybackType m_playbackMode;
	float m_framesPerSecond = 1.f;
	bool m_scaleBySpeed = false;
};