#pragma once
#include <vector>



//-----------------------------------------------------------------------------------------------
class Game;
class MapDefinition; // To-Do, make this
class Tile; // To-Do, make this
class Actor; // To-Do, make this
class RaycastResult;
struct AABB3;
struct AABB2;
struct Vec3;
struct Vertex_PCUTBN;
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
	void AddGeometryForFlorr( AABB3 const& bounds, AABB2 const& UVs );
	void AddGeometryForCeiling( AABB3 const& bounds, AABB2 const& UVs );
	void CreateBuffer();

	bool IsPositionInBounds( Vec3 const& position ) const;
	bool AreCoordsInBounds( int x, int y );
	Tile const* GetTile( int x, int y ) const;

	void Update();
	void CollideActors();
	void CollideActors( Actor* actorA, Actor* actorB );
	void CollideActorsWithMap();
	void CollideActorWithMap( Actor* actor );

	void Render();

	RaycastResult RaycastAll( Vec3 const& start, Vec3 const& direction, float distance, Actor* owner = nullptr ) const;
	RaycastResult RaycastWorldXY( Vec3 const& start, Vec3 const& direction, float distance ) const;
	RaycastResult RaycastWorldZ( Vec3 const& start, Vec3 const& direction, float distance ) const;
	RaycastResult RaycastWorldActors( Vec3 const& start, Vec3 const& direction, float distance, Actor* owner = nullptr ) const;

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