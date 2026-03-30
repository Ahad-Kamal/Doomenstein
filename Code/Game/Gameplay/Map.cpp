#include "Game/Gameplay/Map.hpp"
#include "Game/Gameplay/MapDefinition.hpp"
#include "Game/Gameplay/Tile.hpp"
#include "Game/Gameplay/TileDefinition.hpp"
#include "Game/Gameplay/Game.hpp"
#include "Game/Gameplay/Actors/Actor.hpp"
#include "Game/Gameplay/Actors/Player.hpp"
#include "Engine/Math/RaycastUtils.hpp"
#include "Engine/Math/IntVec3.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/Clock.hpp"
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
	SpawnActors();
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
	Image mapImage = *m_definition->GetImage();
	unsigned int texelDataSize = mapImage.GetDimensions().x * mapImage.GetDimensions().y;
	const unsigned char* texelData = reinterpret_cast<const unsigned char*>( mapImage.GetRawData() );

	for( unsigned int texelIndex = 0; texelIndex < texelDataSize * 4; texelIndex += 4 )
	{
		Rgba8 texelColor = Rgba8( texelData[ texelIndex ], texelData[ texelIndex + 1 ], texelData[ texelIndex + 2 ], texelData[ texelIndex + 3 ] );

		for( unsigned int tileDefIndex = 0; tileDefIndex < TileDefinitions::s_tileDefs.size(); tileDefIndex++ )
		{
			TileDefinitions& tileDef = TileDefinitions::s_tileDefs[ tileDefIndex ];
			if( texelColor == tileDef.GetMapImagePixelColor() )
			{
				int tileNum = texelIndex / 4;
				int coordX = tileNum % mapImage.GetDimensions().x;
				int coordY = tileNum / mapImage.GetDimensions().x;

				Tile newTile = Tile( IntVec3( coordX, coordY, 0 ), &tileDef );
				m_tiles.push_back( newTile );
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Map::CreateGeometry()
{
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
	float deltaSeconds = static_cast<float>( g_game->m_gameClock->GetDeltaSeconds() );
	KeyboardControls( deltaSeconds );
}

//-----------------------------------------------------------------------------------------------
void Map::SpawnActors()
{
	Actor* staticActor1 = new Actor( Vec3( 7.5f, 8.5f, 0.25f ), EulerAngles(), true, Rgba8( 200, 0, 0 ) );
	m_actors.push_back( staticActor1 );

	Actor* staticActor2 = new Actor( Vec3( 8.5f, 8.5f, 0.125f ), EulerAngles(), true, Rgba8( 200, 0, 0 ) );
	m_actors.push_back( staticActor2 );

	Actor* staticActor3 = new Actor( Vec3( 9.5f, 8.5f, 0.f ), EulerAngles(), true, Rgba8( 200, 0, 0 ) );
	m_actors.push_back( staticActor3 );

	m_testActor = new Actor( Vec3( 5.5f, 8.5f, 0.f ), EulerAngles(), 0.125f, 0.0625f, false, Rgba8( 0, 0, 200 ) );
	m_actors.push_back( m_testActor );
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
	for( unsigned int actorIndex = 0; actorIndex < m_actors.size(); actorIndex++ )
	{
		CollideActorWithMap( m_actors[ actorIndex ] );
	}
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

	g_engine->m_render->SetLightConstants( Vec3(), 0.f, 1.f );
	for( unsigned int actorIndex = 0; actorIndex < m_actors.size(); actorIndex++ )
	{
		m_actors[ actorIndex ]->Render();
	}
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

//-----------------------------------------------------------------------------------------------
void Map::KeyboardControls( float deltaSeconds )
{
	if( g_game->m_currentState != GAME_STATE_PLAY )
	{
		return;
	}

	// Note: remove later
	if( g_engine->m_input->WasKeyJustPressed( KEYCODE_F1 ) )
	{
		m_isTestActor = !m_isTestActor;
	}

	if( !m_isTestActor )
	{
		return;
	}

	Vec3 forwardVector = g_game->m_player->m_orientation.GetForwardDir_IFwd_JLeft_KUp();
	float speedFactor = 1.f;
	if( g_engine->m_input->IsKeyDown( KEYCODE_SHIFT ) )
	{
		speedFactor *= 15.f;
	}

	// Left and Right
	if( g_engine->m_input->IsKeyDown( 'A' ) )
	{
		m_testActor->m_position.x += forwardVector.GetRotated90DegreesAboutZ().x * deltaSeconds * speedFactor;
		m_testActor->m_position.y += forwardVector.GetRotated90DegreesAboutZ().y * deltaSeconds * speedFactor;

	}
	if( g_engine->m_input->IsKeyDown( 'D' ) )
	{
		m_testActor->m_position.x -= forwardVector.GetRotated90DegreesAboutZ().x * deltaSeconds * speedFactor;
		m_testActor->m_position.y -= forwardVector.GetRotated90DegreesAboutZ().y * deltaSeconds * speedFactor;
	}

	// Forward and Back
	if( g_engine->m_input->IsKeyDown( 'W' ) )
	{
		m_testActor->m_position.x += forwardVector.x * deltaSeconds * speedFactor;
		m_testActor->m_position.y += forwardVector.y * deltaSeconds * speedFactor;
	}
	if( g_engine->m_input->IsKeyDown( 'S' ) )
	{
		m_testActor->m_position.x -= forwardVector.x * deltaSeconds * speedFactor;
		m_testActor->m_position.y -= forwardVector.y * deltaSeconds * speedFactor;
	}

	// Up and Down
	if( g_engine->m_input->IsKeyDown( 'Z' ) )
	{
		m_testActor->m_position.z += deltaSeconds * speedFactor;
	}
	if( g_engine->m_input->IsKeyDown( 'C' ) )
	{
		m_testActor->m_position.z -= deltaSeconds * speedFactor;
	}
}
