#include "Game/Gameplay/Tile.hpp"
#include "Game/Gameplay/TileDefinition.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"


//-----------------------------------------------------------------------------------------------
Tile::Tile( IntVec3 coordinates, TileDefinitions const* tileDef )
	: m_coords( coordinates )
	, m_tileDef( tileDef )
{
}

//-----------------------------------------------------------------------------------------------
AABB2 Tile::CreateAABB2FromCoords()
{
	return AABB2( static_cast< float >( m_coords.x ), static_cast< float >( m_coords.y ), static_cast< float >( m_coords.x + 1 ), static_cast< float >( m_coords.y + 1 ) );
}

//-----------------------------------------------------------------------------------------------
AABB3 Tile::CreateAABB3FromCoords()
{
	return AABB3( static_cast<float>( m_coords.x ), static_cast<float>( m_coords.y ), static_cast<float>( m_coords.z ),
		static_cast<float>( m_coords.x + 1 ), static_cast<float>( m_coords.y + 1 ), static_cast<float>( m_coords.z + 1 ) );
}

//-----------------------------------------------------------------------------------------------
bool Tile::IsTileSolid()
{
	return m_tileDef->IsSolid();
}

