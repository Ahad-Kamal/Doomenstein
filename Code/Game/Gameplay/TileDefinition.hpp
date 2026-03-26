#pragma once
#include "Game/Gameplay/Tile.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>
#include <string>


//-----------------------------------------------------------------------------------------------
class TileDefinitions
{
public:
	static void InitializeTileDefs();
	std::string GetName() const;
	bool IsSolid() const;
	Rgba8 GetMapImagePixelColor();
	AABB2 GetFloorUVs();
	AABB2 GetCeilingUVs();
	AABB2 GetWallUVs();

	static int GetSpriteIndexFromSpriteCoords( IntVec2 coords );

public:
	static std::vector<TileDefinitions> s_tileDefs;

private:
	static IntVec2 s_spriteSheetDimensions;

	std::string m_name;
	Rgba8 m_mapImagePixelColor;
	AABB2 m_floorUVs;
	AABB2 m_ceilingUVs;
	AABB2 m_wallUVs;
	IntVec2 m_floorSpriteCoords;
	IntVec2 m_ceilingSpriteCoords;
	IntVec2 m_wallSpriteCoords;
	bool m_isSolid = false;
};