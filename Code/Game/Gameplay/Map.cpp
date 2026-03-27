#include "Game/Gameplay/Map.hpp"
#include "Game/Gameplay/Tile.hpp"
#include "Game/Gameplay/TileDefinition.hpp"
#include "Engine/Math/RaycastUtils.hpp"
#include "Engine/Math/IntVec3.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"


//-----------------------------------------------------------------------------------------------
Map::Map( MapDefinition* definition )
	: m_definition( definition )
{
	m_texture = nullptr;
	m_vertexBuffer = new VertexBuffer( g_engine->m_render->GetDevice(), 1, sizeof( Vertex_PCUTBN ) );
	m_indexBuffer = new IndexBuffer( g_engine->m_render->GetDevice(), 1 );

	CreateGeometry();
}

//-----------------------------------------------------------------------------------------------
Map::~Map()
{

}

//-----------------------------------------------------------------------------------------------
void Map::CreateTiles()
{

}

//-----------------------------------------------------------------------------------------------
void Map::CreateGeometry()
{
	AddVertsForQuad3D( m_vertexes, m_indexes, Vec3( 0.f, 1.f, 0.f ), Vec3( 0.f, 0.f, 0.f ), Vec3( 1.f, 0.f, 0.f ), Vec3( 1.f, 1.f, 0.f ) );
}

//-----------------------------------------------------------------------------------------------
void Map::AddGeometryForWall( AABB3 const& bounds, AABB2 const& UVs )
{

}

//-----------------------------------------------------------------------------------------------
void Map::AddGeometryForFloor( AABB3 const& bounds, AABB2 const& UVs )
{

}

//-----------------------------------------------------------------------------------------------
void Map::AddGeometryForCeiling( AABB3 const& bounds, AABB2 const& UVs )
{

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
Tile const* Map::GetTile( [[maybe_unused]] int x, [[maybe_unused]] int y ) const
{
	TileDefinitions* tileDef = nullptr;
	return new Tile( IntVec3(), tileDef );
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
	g_engine->m_render->RenderSetup();
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