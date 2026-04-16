#pragma once
#include "Game/Gameplay/Controller.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Math/RaycastUtils.hpp"
#include <vector>
#include <string>


//-----------------------------------------------------------------------------------------------
class Game;
class Actor;
class MapDefinition;
class TileDefinitions;
struct Tile;
struct IntVec2;
struct AABB3;
struct AABB2;
struct Vec3;
class Texture;
class Shader;
class VertexBuffer;
class IndexBuffer;
class Player;
struct EulerAngles;

//-----------------------------------------------------------------------------------------------
struct WeaponRaycastResult : public RaycastResult3D
{
	Actor* m_impactedActor;

	void operator=( RaycastResult3D const& raycastResult )
	{
		RaycastResult3D::operator=( raycastResult );
	}
};

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
	bool AreCoordsInBounds( int x, int y ) const;
	bool IsPointInSolid( IntVec2 const& position ) const;
	bool IsPointInSolid( Vec3 const& position ) const;
	Tile const* GetTile( int x, int y ) const;
	int	 GetTileIndexFromCoords( int x, int y ) const;
	int	 GetTileIndexFromCoords( IntVec2 coords ) const;
	IntVec2 GetTileCoordsFromIndex( int index ) const;
	TileDefinitions* GetTileDefinition( std::string name ) const;

	void Update( float deltaSeconds );
	void UpdateActors( float deltaSeconds );

	void SpawnPlayer( std::string actorName, Vec3 const& position, EulerAngles const& orientation, Rgba8 color = Rgba8::WHITE );
	Actor* SpawnActor( std::string actorName, Vec3 const& position, EulerAngles const& orientation, Rgba8 color = Rgba8::WHITE );

	void CollideActors();
	void CollideActors( Actor* actorA, Actor* actorB );
	void CollideActorsWithMap();
	void CollideActorWithMap( Actor* actor );

	void Render();

	RaycastResult3D RaycastAll( Vec3 const& start, Vec3 const& direction, float distance, Actor* owner = nullptr ) const;
	RaycastResult3D RaycastWorldXY( Vec3 const& start, Vec3 const& direction, float distance ) const;
	RaycastResult3D RaycastWorldZ( Vec3 const& start, Vec3 const& direction, float distance ) const;
	RaycastResult3D RaycastWorldActors( Vec3 const& start, Vec3 const& direction, float distance, Actor* owner = nullptr ) const;
	WeaponRaycastResult WeaponRaycastAll( Vec3 const& start, Vec3 const& direction, float distance, Actor* owner ) const;
	WeaponRaycastResult WeaponRaycastActors( Vec3 const& start, Vec3 const& direction, float distance, Actor* owner = nullptr ) const;

	void DeleteGarbageActors();

	void DebugPossessNext();
	void DebugRaycast();

public:
	Player*	m_player = nullptr;
	std::vector<Actor*> m_actors;

protected:
	MapDefinition const* m_definition = nullptr;
	std::vector<Tile> m_tiles;

	std::vector<Vertex_PCUTBN> m_vertexes;
	std::vector<unsigned int> m_indexes;
	Texture const* m_texture;
	Shader* m_shader = nullptr;
	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;
	
	unsigned int m_currentUID = 0;
};