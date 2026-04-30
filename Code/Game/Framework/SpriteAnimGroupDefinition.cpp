#include "Game/Framework/SpriteAnimGroupDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/XmlUtils.hpp"


//-----------------------------------------------------------------------------------------------
bool SpriteAnimationGroupDefinition::LoadFromXmlElement( XmlElement const& element, SpriteSheet const& spriteSheet )
{
	if( !&element )
	{
		return false;
	}

	NamedStrings actorDefAnimGroupBlackboard;
	actorDefAnimGroupBlackboard.PopulateFromXmlElementAttributes( element );

	m_name = actorDefAnimGroupBlackboard.GetValue( "name", "" );
	
	float secondsPerFrame = actorDefAnimGroupBlackboard.GetValue( "secondsPerFrame", 1.f );
	m_framesPerSecond = 1.f / secondsPerFrame;

	std::string playbackString = actorDefAnimGroupBlackboard.GetValue( "playbackMode", "ONCE" );
	m_playbackMode = GetPlaybackTypeFromString( playbackString );

	const XmlElement* animElement = element.FirstChildElement();
	while( animElement )
	{
		NamedStrings actorDefAnimBlackboard;
		actorDefAnimBlackboard.PopulateFromXmlElementAttributes( *animElement );
		Vec3 direction = actorDefAnimBlackboard.GetValue( "vector", Vec3( 1.f, 0.f, 0.f ) );
		m_directions.push_back( direction.GetNormalized() );

		const XmlElement* animChildElement = animElement->FirstChildElement();
		NamedStrings actorDefAnimChildBlackboard;
		actorDefAnimChildBlackboard.PopulateFromXmlElementAttributes( *animChildElement );
		int startFrame = actorDefAnimChildBlackboard.GetValue( "startFrame", 0 );
		int endFrame = actorDefAnimChildBlackboard.GetValue( "endFrame", 0 );
		SpriteAnimDefinition newAimDef = SpriteAnimDefinition( spriteSheet, startFrame, endFrame, m_framesPerSecond, m_playbackMode );
		m_spriteAnimationDefinitions.push_back( newAimDef );

		animElement = animElement->NextSiblingElement();
	}	

	return true;
}

//-----------------------------------------------------------------------------------------------
const SpriteAnimDefinition& SpriteAnimationGroupDefinition::GetAnimationForDirection( Vec3 const& direction ) const
{
	Vec3 dirNormalized = direction.GetNormalized();
	float greatestDotProduct = -1.f;
	int closestDirectionIndex = 0;

	for( unsigned int dirIndex = 0; dirIndex < m_directions.size(); dirIndex++ )
	{
		float dotProd = DotProduct3D( m_directions[ dirIndex ], dirNormalized);
		if( dotProd > greatestDotProduct )
		{
			greatestDotProduct = dotProd;
			closestDirectionIndex = dirIndex;
		}
	}

	return m_spriteAnimationDefinitions[ closestDirectionIndex ];
}

//-----------------------------------------------------------------------------------------------
float SpriteAnimationGroupDefinition::GetDuration() const
{
	SpriteAnimDefinition animDef = m_spriteAnimationDefinitions[ 0 ];
	int numFrames = animDef.GetEndIndex() - animDef.GetStartIndex();
	return ( 1.f / m_framesPerSecond ) * numFrames;
}

