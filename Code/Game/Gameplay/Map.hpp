#pragma once
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include <vector>
#include <string>


//-----------------------------------------------------------------------------------------------
class Game;
class Actor;
class MapDefinition;
class TileDefinitions;
struct Tile;
struct RaycastResult3D;
struct AABB3;
struct AABB2;
struct Vec3;
class Texture;
class Shader;
class VertexBuffer;
class IndexBuffer;

class Map
{
public:
	Map( MapDefinition* definition );
	~Map();

	void CreateTiles();
	void CreateGeometry();
	void AddGeometryForWall( AABB3 const& bounds, AABB2 const& UVs );
	void AddGeometryForFloor( AABB3 const& bounds, AABB2 const& UVs );
	void AddGeometryForCeiling( AABB3 const& bounds, AABB2 const& UVs );
	void CreateBuffer();

	bool IsPositionInBounds( Vec3 const& position ) const;
	bool AreCoordsInBounds( int x, int y );
	Tile const* GetTile( int x, int y ) const;
	TileDefinitions* GetTileDefinition( std::string name );

	void Update();
	void CollideActors();
	void CollideActors( Actor* actorA, Actor* actorB );
	void CollideActorsWithMap();
	void CollideActorWithMap( Actor* actor );

	void Render();

	RaycastResult3D RaycastAll( Vec3 const& start, Vec3 const& direction, float distance, Actor* owner = nullptr ) const;
	RaycastResult3D RaycastWorldXY( Vec3 const& start, Vec3 const& direction, float distance ) const;
	RaycastResult3D RaycastWorldZ( Vec3 const& start, Vec3 const& direction, float distance ) const;
	RaycastResult3D RaycastWorldActors( Vec3 const& start, Vec3 const& direction, float distance, Actor* owner = nullptr ) const;

protected:
	MapDefinition const* m_definition = nullptr;
	std::vector<Tile> m_tiles;

	std::vector<Vertex_PCUTBN> m_vertexes;
	std::vector<unsigned int> m_indexes;
	Texture const* m_texture;
	Shader* m_shader = nullptr;
	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;
};