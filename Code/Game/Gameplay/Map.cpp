#include "Game/Gameplay/Map.hpp"
#include "Game/Gameplay/MapDefinition.hpp"
#include "Game/Gameplay/Tile.hpp"
#include "Game/Gameplay/TileDefinition.hpp"
#include "Game/Gameplay/Game.hpp"
#include "Engine/Math/RaycastUtils.hpp"
#include "Engine/Math/IntVec3.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Camera.hpp"


//-----------------------------------------------------------------------------------------------
Map::Map( MapDefinition* definition )
	: m_definition( definition )
{
	m_texture = nullptr;
	m_vertexBuffer = new VertexBuffer( g_engine->m_render->GetDevice(), 1, sizeof( Vertex_PCUTBN ) );
	m_indexBuffer = new IndexBuffer( g_engine->m_render->GetDevice(), 1 );

	m_vertexBuffer->Create();
	m_indexBuffer->Create();

	CreateTiles();
	CreateGeometry();
}

//-----------------------------------------------------------------------------------------------
Map::~Map()
{
	delete m_vertexBuffer;
	m_vertexBuffer = nullptr;

	delete m_indexBuffer;
	m_indexBuffer = nullptr;
}

//-----------------------------------------------------------------------------------------------
void Map::CreateTiles()
{
	TileDefinitions* tileDef = GetTileDefinition( "BrickWall" );
	Tile newTile = Tile( IntVec3(), tileDef );
	m_tiles.push_back( newTile );
}

//-----------------------------------------------------------------------------------------------
void Map::CreateGeometry()
{
	/*TileDefinitions* tileDef = GetTileDefinition( "StoneFloor" );
	AABB2 floorUVs = tileDef->GetFloorUVs();
	AABB2 ceilingUVs = tileDef->GetCeilingUVs();
	AddVertsForQuad3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 0.f ), Vec3( 1.f, 0.f, 0.f ), Vec3( 1.f, 1.f, 0.f ), Vec3( 0.f, 1.f, 0.f ), Rgba8::WHITE, floorUVs );
	AddVertsForQuad3D( m_vertexes, m_indexes, Vec3( 0.f, 0.f, 1.f ), Vec3( 0.f, 1.f, 1.f ), Vec3( 1.f, 1.f, 1.f ), Vec3( 1.f, 0.f, 1.f ), Rgba8::WHITE, ceilingUVs );*/
	
	for( unsigned int tileIndex = 0; tileIndex < m_tiles.size(); tileIndex++ )
	{
		Tile& tile = m_tiles[ tileIndex ];
		AABB3 bounds = AABB3( Vec3( tile.m_coords ), Vec3( tile.m_coords + 1 ) );

		AABB2 floorUVs = tile.m_tileDef->GetFloorUVs();
		if( floorUVs != AABB2() )
		{
			AddGeometryForFloor( bounds, floorUVs );
		}

		AABB2 ceilingUVs = tile.m_tileDef->GetCeilingUVs();
		if( ceilingUVs != AABB2() )
		{
			AddGeometryForCeiling( bounds, ceilingUVs );
		}

		AABB2 wallUVs = tile.m_tileDef->GetWallUVs();
		if( wallUVs != AABB2() )
		{
			AddGeometryForWall( bounds, wallUVs );
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Map::AddGeometryForWall( AABB3 const& bounds, AABB2 const& UVs )
{
	AddVertsForQuad3D( m_vertexes, m_indexes, bounds.m_mins, Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ),
		Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), Rgba8::WHITE, UVs);

	AddVertsForQuad3D( m_vertexes, m_indexes, Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ), Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), 
		bounds.m_maxs, Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), Rgba8::WHITE, UVs);

	AddVertsForQuad3D( m_vertexes, m_indexes, Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ),
		Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ), bounds.m_maxs, Rgba8::WHITE, UVs);

	AddVertsForQuad3D( m_vertexes, m_indexes, Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), bounds.m_mins,
		Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ), Rgba8::WHITE, UVs);
}

//-----------------------------------------------------------------------------------------------
void Map::AddGeometryForFloor( AABB3 const& bounds, AABB2 const& UVs )
{
	AddVertsForQuad3D( m_vertexes, m_indexes, bounds.m_mins, Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ),
		Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), Rgba8::WHITE, UVs );
}

//-----------------------------------------------------------------------------------------------
void Map::AddGeometryForCeiling( AABB3 const& bounds, AABB2 const& UVs )
{
	AddVertsForQuad3D( m_vertexes, m_indexes, Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ),
		bounds.m_maxs, Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), Rgba8::WHITE, UVs );
}

//-----------------------------------------------------------------------------------------------
void Map::CreateBuffer()
{

}

//-----------------------------------------------------------------------------------------------
bool Map::IsPositionInBounds( [[maybe_unused]] Vec3 const& position ) const
{
	return false;
}

//-----------------------------------------------------------------------------------------------
bool Map::AreCoordsInBounds( [[maybe_unused]] int x, [[maybe_unused]] int y )
{
	return false;
}

//-----------------------------------------------------------------------------------------------
Tile const* Map::GetTile( int x, int y ) const
{
	return &m_tiles[ GetTileIndexFromCoords( x, y ) ];
}

//-----------------------------------------------------------------------------------------------
int Map::GetTileIndexFromCoords( int x, int y ) const
{
	return ( y * m_definition->GetImage()->GetDimensions().x ) + x;
}

//-----------------------------------------------------------------------------------------------
IntVec2 Map::GetTileCoordsFromIndex( int index ) const
{
	return m_tiles[ index ].m_coords;
}

//-----------------------------------------------------------------------------------------------
TileDefinitions* Map::GetTileDefinition( std::string name ) const
{
	for( unsigned int tileDefIndex = 0; tileDefIndex < TileDefinitions::s_tileDefs.size(); tileDefIndex++ )
	{
		if( TileDefinitions::s_tileDefs[ tileDefIndex ].GetName() == name )
		{
			return &TileDefinitions::s_tileDefs[ tileDefIndex ];
		}
	}

	return nullptr;
}

//-----------------------------------------------------------------------------------------------
void Map::Update()
{

}

//-----------------------------------------------------------------------------------------------
void Map::CollideActors()
{

}

//-----------------------------------------------------------------------------------------------
void Map::CollideActors( Actor* actorA, Actor* actorB )
{

}

//-----------------------------------------------------------------------------------------------
void Map::CollideActorsWithMap()
{

}

//-----------------------------------------------------------------------------------------------
void Map::CollideActorWithMap( Actor* actor )
{

}

//-----------------------------------------------------------------------------------------------
void Map::Render()
{
	g_engine->m_render->RenderSetup( &g_terrainSpriteSheet->GetTexture() );
	g_engine->m_render->DrawVertexArray( m_vertexes, m_indexes, m_vertexBuffer, m_indexBuffer );
}

//-----------------------------------------------------------------------------------------------
RaycastResult3D Map::RaycastAll( [[maybe_unused]] Vec3 const& start, [[maybe_unused]] Vec3 const& direction, [[maybe_unused]] float distance, [[maybe_unused]] Actor* owner /*= nullptr */ ) const
{
	return RaycastResult3D();
}

//-----------------------------------------------------------------------------------------------
RaycastResult3D Map::RaycastWorldXY( [[maybe_unused]] Vec3 const& start, [[maybe_unused]] Vec3 const& direction, [[maybe_unused]] float distance ) const
{
	return RaycastResult3D();
}

//-----------------------------------------------------------------------------------------------
RaycastResult3D Map::RaycastWorldZ( [[maybe_unused]] Vec3 const& start, [[maybe_unused]] Vec3 const& direction, [[maybe_unused]] float distance ) const
{
	return RaycastResult3D();
}

//-----------------------------------------------------------------------------------------------
RaycastResult3D Map::RaycastWorldActors( [[maybe_unused]] Vec3 const& start, [[maybe_unused]] Vec3 const& direction, [[maybe_unused]] float distance, [[maybe_unused]] Actor* owner /*= nullptr */ ) const
{
	return RaycastResult3D();
}