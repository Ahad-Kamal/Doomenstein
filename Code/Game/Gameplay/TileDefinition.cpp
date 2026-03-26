#include "Game/Gameplay/TileDefinition.hpp"
#include "Game/Gameplay/Game.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"


//-----------------------------------------------------------------------------------------------
std::vector<TileDefinitions> TileDefinitions::s_tileDefs;
IntVec2 TileDefinitions::s_spriteSheetDimensions = IntVec2( 8, 8 );

//-----------------------------------------------------------------------------------------------
void TileDefinitions::InitializeTileDefs()
{
	// Read in TileDefinitions.xml
	XmlDocument tileDefs;
	tileDefs.LoadFile( "Data/Definitions/TileDefinitions.xml" );
	int numTileDefs = tileDefs.FirstChildElement()->ChildElementCount();
	s_tileDefs.resize( numTileDefs );

	XmlElement* currentElement = tileDefs.FirstChildElement()->FirstChildElement();
	for( int elementIndex = 0; elementIndex < numTileDefs; elementIndex++ )
	{
		NamedStrings tileDefBlackboard;
		tileDefBlackboard.PopulateFromXmlElementAttributes( *currentElement );

		s_tileDefs[ elementIndex ].m_name = tileDefBlackboard.GetValue( "name", "Unknown" );
		s_tileDefs[ elementIndex ].m_isSolid = tileDefBlackboard.GetValue( "isSolid", false );
		s_tileDefs[ elementIndex ].m_mapImagePixelColor = tileDefBlackboard.GetValue( "mapImagePixelColor", Rgba8( 0, 0, 0, 0 ) );
		s_tileDefs[ elementIndex ].m_floorSpriteCoords = tileDefBlackboard.GetValue( "floorSpriteCoords", IntVec2( -1, -1 ) );
		s_tileDefs[ elementIndex ].m_ceilingSpriteCoords = tileDefBlackboard.GetValue( "ceilingSpriteCoords", IntVec2( -1, -1 ) );
		s_tileDefs[ elementIndex ].m_wallSpriteCoords = tileDefBlackboard.GetValue( "wallSpriteCoords", IntVec2( -1, -1 ) );

		if( s_tileDefs[ elementIndex ].m_floorSpriteCoords != IntVec2( -1, -1 ) )
		{
			int floorSpriteIndex = GetSpriteIndexFromSpriteCoords( s_tileDefs[ elementIndex ].m_floorSpriteCoords );
			const SpriteDefinition& floorSpriteDef = g_terrainSpriteSheet->GetSpriteDef( floorSpriteIndex );
			s_tileDefs[ elementIndex ].m_floorUVs = floorSpriteDef.GetUVs();
		}

		if( s_tileDefs[ elementIndex ].m_ceilingSpriteCoords != IntVec2( -1, -1 ) )
		{
			int ceilingSpriteIndex = GetSpriteIndexFromSpriteCoords( s_tileDefs[ elementIndex ].m_ceilingSpriteCoords );
			const SpriteDefinition& ceilingSpriteDef = g_terrainSpriteSheet->GetSpriteDef( ceilingSpriteIndex );
			s_tileDefs[ elementIndex ].m_floorUVs = ceilingSpriteDef.GetUVs();
		}

		if( s_tileDefs[ elementIndex ].m_wallSpriteCoords != IntVec2( -1, -1 ) )
		{
			int wallSpriteIndex = GetSpriteIndexFromSpriteCoords( s_tileDefs[ elementIndex ].m_wallSpriteCoords );
			const SpriteDefinition& wallSpriteDef = g_terrainSpriteSheet->GetSpriteDef( wallSpriteIndex );
			s_tileDefs[ elementIndex ].m_floorUVs = wallSpriteDef.GetUVs();
		}

		currentElement = currentElement->NextSiblingElement();
	}
}

std::string TileDefinitions::GetName() const
{
	return m_name;
}

bool TileDefinitions::IsSolid() const
{
	return m_isSolid;
}

Rgba8 TileDefinitions::GetMapImagePixelColor()
{
	return m_mapImagePixelColor;
}

AABB2 TileDefinitions::GetFloorUVs()
{
	return m_floorUVs;
}

AABB2 TileDefinitions::GetCeilingUVs()
{
	return m_ceilingUVs;
}

AABB2 TileDefinitions::GetWallUVs()
{
	return m_wallUVs;
}

int TileDefinitions::GetSpriteIndexFromSpriteCoords( IntVec2 coords )
{
	return ( coords.y * s_spriteSheetDimensions.x ) + coords.x;
}