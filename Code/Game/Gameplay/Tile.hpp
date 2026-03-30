#pragma once
#include "Engine/Math/IntVec3.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
struct AABB2;
struct AABB3;
class TileDefinitions;

//-----------------------------------------------------------------------------------------------
struct Tile
{
public:
	IntVec3 m_coords;
	TileDefinitions const* m_tileDef;

public:
	Tile( IntVec3 coordinates, TileDefinitions const* tileDef );
	~Tile() = default;

	AABB2 CreateAABB2FromCoords();
	AABB3 CreateAABB3FromCoords();
	bool IsTileSolid();
};