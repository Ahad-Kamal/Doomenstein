#include "Game/Gameplay/Map.hpp"
#include "Game/Gameplay/MapDefinition.hpp"
#include "Game/Gameplay/Tile.hpp"
#include "Game/Gameplay/TileDefinition.hpp"
#include "Game/Gameplay/Game.hpp"
#include "Game/Gameplay/Actors/Actor.hpp"
#include "Game/Gameplay/Actors/Player.hpp"
#include "Game/Gameplay/ActorDefinition.hpp"
#include "Game/Framework/GameCommon.hpp"
#include "Game/Framework/ActorHandle.hpp"
#include "Engine/Math/RaycastUtils.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/IntVec3.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


//-----------------------------------------------------------------------------------------------
Map::Map( MapDefinition* definition )
	: m_definition( definition )
{
	m_texture = nullptr;
	
	CreateBuffer();
	CreateTiles();
	CreateGeometry();
	SpawnPlayer();
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
	m_vertexBuffer = new VertexBuffer( g_engine->m_render->GetDevice(), 1, sizeof( Vertex_PCUTBN ) );
	m_indexBuffer = new IndexBuffer( g_engine->m_render->GetDevice(), 1 );

	m_vertexBuffer->Create();
	m_indexBuffer->Create();
}

//-----------------------------------------------------------------------------------------------
bool Map::IsPositionInBounds( [[maybe_unused]] Vec3 const& position ) const
{
	IntVec2 boundsXY = m_definition->GetImage()->GetDimensions();
	float epsilon = -1e-6f;


	if( position.x >= 0.f && position.x <= static_cast<float>( boundsXY.x ) && 
		position.y >= 0.f && position.y <= static_cast<float>( boundsXY.y ) && 
		position.z >= epsilon && position.z <= 1.f - epsilon )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------
bool Map::AreCoordsInBounds( int x, int y ) const
{
	IntVec2 boundsXY = m_definition->GetImage()->GetDimensions();

	if( x >= 0.f && x <= static_cast<float>( boundsXY.x ) && y >= 0.f && y <= static_cast<float>( boundsXY.y ) )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------
bool Map::IsPointInSolid( IntVec2 const& position ) const
{
	if( !AreCoordsInBounds( position.x, position.y ) )
	{
		return false;
	}

	int tileIndex = GetTileIndexFromCoords( position );

	return m_tiles[ tileIndex ].m_tileDef->IsSolid();
}

//-----------------------------------------------------------------------------------------------
bool Map::IsPointInSolid( Vec3 const& position ) const
{
	if( !IsPositionInBounds( position ) )
	{
		return false;
	}

	IntVec2 coords = IntVec2( RoundDownToInt( position.x ), RoundDownToInt( position.y ) );
	int tileIndex = GetTileIndexFromCoords( coords );

	return m_tiles[ tileIndex ].m_tileDef->IsSolid();
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
int Map::GetTileIndexFromCoords( IntVec2 coords ) const
{
	return ( coords.y * m_definition->GetImage()->GetDimensions().x ) + coords.x;
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
void Map::Update( float deltaSeconds )
{
	UpdateActors( deltaSeconds );

	/*FreeFlyKeyboardControls( deltaSeconds );
	FreeFlyControllerControls( deltaSeconds );*/
	MouseControls();

	CollideActors();
	CollideActorsWithMap();
}

//-----------------------------------------------------------------------------------------------
void Map::UpdateActors( float deltaSeconds )
{
	for( unsigned int actorIndex = 0; actorIndex < m_actors.size(); actorIndex++ )
	{
		m_actors[ actorIndex ]->Update( deltaSeconds );
	}
}

//-----------------------------------------------------------------------------------------------
void Map::SpawnPlayer()
{
	for( unsigned int actorDefIndex = 0; actorDefIndex < ActorDefinition::s_actorDefs.size(); actorDefIndex++ )
	{
		ActorDefinition& actorDef = ActorDefinition::s_actorDefs[ actorDefIndex ];
		std::string name = actorDef.GetName();
		if( name == "Marine" )
		{
			ActorHandle newActorHandle = ActorHandle( m_currentUID, static_cast<unsigned int>( m_actors.size() ) );
			m_currentUID++;
			m_player = new Player( Vec3( 2.5f, 8.5f, 0.5f ), EulerAngles(), &actorDef, newActorHandle );
			m_actors.push_back( m_player );
		}

		//if( name == "Demon" )
		//{
		//	ActorHandle newActorHandle = ActorHandle( m_currentUID, static_cast<unsigned int>( m_actors.size() ) );
		//	m_currentUID++;
		//	Actor* staticActor1 = new Actor( Vec3( 7.5f, 8.5f, 0.25f ), EulerAngles(), &actorDef, newActorHandle, true, Rgba8( 200, 0, 0 ) );
		//	m_actors.push_back( staticActor1 );

		//	//Actor* staticActor2 = new Actor( Vec3( 8.5f, 8.5f, 0.125f ), EulerAngles(), true, Rgba8( 200, 0, 0 ) );
		//	//m_actors.push_back( staticActor2 );

		//	//Actor* staticActor3 = new Actor( Vec3( 9.5f, 8.5f, 0.f ), EulerAngles(), true, Rgba8( 200, 0, 0 ) );
		//	//m_actors.push_back( staticActor3 );

		//	//Actor* nonStaticActor = new Actor( Vec3( 6.5f, 7.5f, 0.35f ), EulerAngles(), 0.25f, 0.125f, false, Rgba8( 0, 200, 0 ) );
		//	//m_actors.push_back( nonStaticActor );
		//}
	}
	if( m_player == nullptr )
	{
		ERROR_AND_DIE( "ERROR: Player could not be created, Player Actor Definition missing." );
	}
}

//-----------------------------------------------------------------------------------------------
void Map::CollideActors()
{
	for( unsigned int actorAIndex = 0; actorAIndex < m_actors.size(); actorAIndex++ )
	{
		Actor*& actorA = m_actors[ actorAIndex ];
		if( actorA->IsAlive() )
		{
			for( unsigned int actorBIndex = 0; actorBIndex < m_actors.size(); actorBIndex++ )
			{
				Actor*& actorB = m_actors[ actorBIndex ];
				if( actorB->IsAlive() )
				{
					CollideActors( actorA, actorB );
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Map::CollideActors( Actor* actorA, Actor* actorB )
{
	if( actorA == actorB )
	{
		return;
	}

	// To-Do: Change to use m_doesPushEntities & m_isPushedByEntities? See Libra for example.
	// A and B push each other
	if( !actorA->m_isStatic && !actorB->m_isStatic )
	{
		PushCylindersOutOfEachOther( actorA->m_position, actorA->m_physicsHeight, actorA->m_physicsRadius, actorB->m_position, actorB->m_physicsHeight, actorB->m_physicsRadius );
	}
	// A pushes B
	else if( actorA->m_isStatic && !actorB->m_isStatic )
	{
		PushCylinderOutOfFixedCylinder( actorB->m_position, actorB->m_physicsHeight, actorB->m_physicsRadius, actorA->m_position, actorA->m_physicsHeight, actorA->m_physicsRadius );
	}
	// B pushes A
	else if( !actorA->m_isStatic && actorB->m_isStatic )
	{
		PushCylinderOutOfFixedCylinder( actorA->m_position, actorA->m_physicsHeight, actorA->m_physicsRadius, actorB->m_position, actorB->m_physicsHeight, actorB->m_physicsRadius );
	}
}

//-----------------------------------------------------------------------------------------------
void Map::CollideActorsWithMap()
{
	for( unsigned int actorIndex = 0; actorIndex < m_actors.size(); actorIndex++ )
	{
		Actor*& actor = m_actors[ actorIndex ];
		if( actor->IsAlive() )
		{
			CollideActorWithMap( actor );
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Map::CollideActorWithMap( Actor* actor )
{
	if( actor->m_isStatic )
	{
		return;
	}

	IntVec2 coords = actor->GetCoordsOfCurrentTile();
	IntVec2 mapDimensions = m_definition->GetImage()->GetDimensions();

	if( coords.x <= 0 || coords.x >= mapDimensions.x - 1 || coords.y <= 0 || coords.y >= mapDimensions.y - 1 )
	{
		return;
	}

	// Tile North of Actor
	Tile northTile = Tile( m_tiles[ GetTileIndexFromCoords( coords + NORTH ) ] );
	if( northTile.IsTileSolid() )
	{
		AABB2 tileBox = northTile.CreateAABB2FromCoords();
		PushCylinderOutOfFixedAABB2D( actor->m_position, actor->m_physicsRadius, tileBox );
	}

	// Tile East of Actor
	Tile eastTile = Tile( m_tiles[ GetTileIndexFromCoords( coords + EAST ) ] );
	if( eastTile.IsTileSolid() )
	{
		AABB2 tileBox = eastTile.CreateAABB2FromCoords();
		PushCylinderOutOfFixedAABB2D( actor->m_position, actor->m_physicsRadius, tileBox );
	}

	// Tile South of Actor
	Tile southTile = Tile( m_tiles[ GetTileIndexFromCoords( coords + SOUTH ) ] );
	if( southTile.IsTileSolid() )
	{
		AABB2 tileBox = southTile.CreateAABB2FromCoords();
		PushCylinderOutOfFixedAABB2D( actor->m_position, actor->m_physicsRadius, tileBox );
	}

	// Tile West of Actor
	Tile westTile = Tile( m_tiles[ GetTileIndexFromCoords( coords + WEST ) ] );
	if( westTile.IsTileSolid() )
	{
		AABB2 tileBox = westTile.CreateAABB2FromCoords();
		PushCylinderOutOfFixedAABB2D( actor->m_position, actor->m_physicsRadius, tileBox );
	}


	// Tile Northeast of Actor
	Tile northEastTile = Tile( m_tiles[ GetTileIndexFromCoords( coords + NORTH + EAST ) ] );
	if( northEastTile.IsTileSolid() )
	{
		AABB2 tileBox = northEastTile.CreateAABB2FromCoords();
		PushCylinderOutOfFixedAABB2D( actor->m_position, actor->m_physicsRadius, tileBox );
	}

	// Tile Southeast of Actor
	Tile southEastTile = Tile( m_tiles[ GetTileIndexFromCoords( coords + SOUTH + EAST ) ] );
	if( southEastTile.IsTileSolid() )
	{
		AABB2 tileBox = southEastTile.CreateAABB2FromCoords();
		PushCylinderOutOfFixedAABB2D( actor->m_position, actor->m_physicsRadius, tileBox );
	}

	// Tile Southwest of Actor
	Tile southWestTile = Tile( m_tiles[ GetTileIndexFromCoords( coords + SOUTH + WEST ) ] );
	if( southWestTile.IsTileSolid() )
	{
		AABB2 tileBox = southWestTile.CreateAABB2FromCoords();
		PushCylinderOutOfFixedAABB2D( actor->m_position, actor->m_physicsRadius, tileBox );
	}

	// Tile Northwest of Actor
	Tile northWestTile = Tile( m_tiles[ GetTileIndexFromCoords( coords + NORTH + WEST ) ] );
	if( northWestTile.IsTileSolid() )
	{
		AABB2 tileBox = northWestTile.CreateAABB2FromCoords();
		PushCylinderOutOfFixedAABB2D( actor->m_position, actor->m_physicsRadius, tileBox );
	}

	// Tile(?) Above Actor
	if( actor->m_position.z > ( 1.f - actor->m_cosmeticHeight ) )
	{
		actor->m_position.z = 1.f - actor->m_cosmeticHeight;
	}

	// Tile(?) Below Actor<
	if( actor->m_position.z < 0.f )
	{
		actor->m_position.z = 0.f;
	}
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
RaycastResult3D Map::RaycastAll( Vec3 const& start, Vec3 const& direction, float distance, Actor* owner /*= nullptr */ ) const
{
	RaycastResult3D raycastResult = RaycastResult3D();
	RaycastResult3D raycastResultXY = RaycastWorldXY( start, direction, distance );
	RaycastResult3D raycastResultZ = RaycastWorldZ( start, direction, distance );
	RaycastResult3D raycastResultWorld = RaycastWorldActors( start, direction, distance, owner );

	if( !raycastResultXY.m_didImpact && !raycastResultZ.m_didImpact && !raycastResultWorld.m_didImpact )
	{
		return raycastResult;
	}
	if( raycastResultXY.m_didImpact && !raycastResultZ.m_didImpact && !raycastResultWorld.m_didImpact )
	{
		return raycastResultXY;
	}
	if( !raycastResultXY.m_didImpact && raycastResultZ.m_didImpact && !raycastResultWorld.m_didImpact )
	{
		return raycastResultZ;
	}
	if( !raycastResultXY.m_didImpact && !raycastResultZ.m_didImpact && raycastResultWorld.m_didImpact )
	{
		return raycastResultWorld;
	}

	raycastResult = raycastResultXY;
	if( !raycastResult.m_didImpact && raycastResultZ.m_didImpact )
	{
		raycastResult = raycastResultZ;
	}
	else if( raycastResult.m_didImpact && raycastResultZ.m_didImpact )
	{
		if( raycastResult.m_implactDist > raycastResultZ.m_implactDist )
		{
			raycastResult = raycastResultZ;
		}
	}

	if( !raycastResult.m_didImpact && raycastResultWorld.m_didImpact )
	{
		raycastResult = raycastResultWorld;
	}
	else if( raycastResult.m_didImpact && raycastResultWorld.m_didImpact )
	{
		if( raycastResult.m_implactDist > raycastResultWorld.m_implactDist )
		{
			raycastResult = raycastResultWorld;
		}
	}

	return raycastResult;
}

//-----------------------------------------------------------------------------------------------
RaycastResult3D Map::RaycastWorldXY( Vec3 const& start, Vec3 const& direction, float distance ) const
{
	RaycastResult3D raycast = RaycastResult3D( start, direction, distance );

	if( IsPointInSolid( start ) )
	{
		raycast.m_didImpact = true;
		raycast.m_implactDist = 0.f;
		raycast.m_impactPos = start;
		raycast.m_impactNormal = direction * -1.f;
		return raycast;
	}

	int tileX = static_cast<int>( floorf( start.x ) );
	int tileY = static_cast<int>( floorf( start.y ) );

	// X Tile Crossing
	float fwdDistPerXCrossing = 1.f / abs( direction.x );
	int tileStepDirectionX;
	if( direction.x < 0 )
	{
		tileStepDirectionX = -1;
	}
	else
	{
		tileStepDirectionX = 1;
	}
	float xAtFirstXCrossing = static_cast<float>( tileX + ( tileStepDirectionX + 1 ) / 2 );
	float xDistToFirstXCrossing = xAtFirstXCrossing - start.x;
	float fwdDistAtNextXCrossing = fabsf( xDistToFirstXCrossing ) * fwdDistPerXCrossing;

	// Y Tile Crossing
	float fwdDistPerYCrossing = 1.f / abs( direction.y );
	int tileStepDirectionY;
	if( direction.y < 0 )
	{
		tileStepDirectionY = -1;
	}
	else
	{
		tileStepDirectionY = 1;
	}
	float yAtFirstYCrossing = static_cast<float>( tileY + ( tileStepDirectionY + 1 ) / 2 );
	float yDistToFirstYCrossing = yAtFirstYCrossing - start.y;
	float fwdDistAtNextYCrossing = fabsf( yDistToFirstYCrossing ) * fwdDistPerYCrossing;

	while( true )
	{
		if( fwdDistAtNextXCrossing <= fwdDistAtNextYCrossing )
		{
			if( fwdDistAtNextXCrossing >= distance )
			{
				raycast.m_didImpact = false;
				raycast.m_implactDist = distance;
				raycast.m_impactPos = start + ( direction * distance );
				return raycast;
			}

			tileX += tileStepDirectionX;
			if( IsPointInSolid( IntVec2( tileX, tileY ) ) )
			{
				Vec3 currentPos = ( direction * fwdDistAtNextXCrossing ) + start;
				if( currentPos.z >= 0.f && currentPos.z <= 1.f )
				{
					raycast.m_didImpact = true;
					raycast.m_implactDist = fwdDistAtNextXCrossing;
					raycast.m_impactPos = currentPos;
					if( tileStepDirectionX > 0 )
					{
						raycast.m_impactNormal = Vec3( -1.f, 0.f, 0.f );
					}
					else
					{
						raycast.m_impactNormal = Vec3( 1.f, 0.f, 0.f );
					}
					return raycast;
				}
			}

			fwdDistAtNextXCrossing += fwdDistPerXCrossing;
		}
		else
		{
			if( fwdDistAtNextYCrossing >= distance )
			{
				raycast.m_didImpact = false;
				raycast.m_implactDist = distance;
				raycast.m_impactPos = start + ( direction * distance );
				return raycast;
			}

			tileY += tileStepDirectionY;
			if( IsPointInSolid( IntVec2( tileX, tileY ) ) )
			{
				Vec3 currentPos = ( direction * fwdDistAtNextYCrossing ) + start;
				if( currentPos.z >= 0.f && currentPos.z <= 1.f )
				{
					raycast.m_didImpact = true;
					raycast.m_implactDist = fwdDistAtNextYCrossing;
					raycast.m_impactPos = currentPos;
					if( tileStepDirectionY > 0 )
					{
						raycast.m_impactNormal = Vec3( 0.f, -1.f, 0.f );
					}
					else
					{
						raycast.m_impactNormal = Vec3( 0.f, 1.f, 0.f );
					}
					return raycast;
				}
			}

			fwdDistAtNextYCrossing += fwdDistPerYCrossing;
		}
	}
}

//-----------------------------------------------------------------------------------------------
RaycastResult3D Map::RaycastWorldZ( Vec3 const& start, Vec3 const& direction, float distance ) const
{
	RaycastResult3D raycast = RaycastResult3D( start, direction, distance );

	if( IsPointInSolid( start ) )
	{
		raycast.m_didImpact = true;
		raycast.m_implactDist = 0.f;
		raycast.m_impactPos = start;
		raycast.m_impactNormal = direction * -1.f;
		return raycast;
	}

	int tileZ = static_cast<int>( floorf( start.z ) );

	// Z Tile Crossing
	float fwdDistPerZCrossing = 1.f / abs( direction.z );
	int tileStepDirectionZ;
	if( direction.z < 0 )
	{
		tileStepDirectionZ = -1;
	}
	else
	{
		tileStepDirectionZ = 1;
	}
	float zAtFirstZCrossing = static_cast<float>( tileZ + ( tileStepDirectionZ + 1 ) / 2 );
	float zDistToFirstZCrossing = zAtFirstZCrossing - start.z;
	float fwdDistAtNextZCrossing = fabsf( zDistToFirstZCrossing ) * fwdDistPerZCrossing;

	while( true )
	{
		if( fwdDistAtNextZCrossing >= distance )
		{
			raycast.m_didImpact = false;
			raycast.m_implactDist = distance;
			raycast.m_impactPos = ( direction * distance ) + start;
			return raycast;
		}

		tileZ += tileStepDirectionZ;
		Vec3 currentPos = ( direction * fwdDistAtNextZCrossing ) + start;
		if( IsPositionInBounds( currentPos ) )
		{
			raycast.m_didImpact = true;
			raycast.m_implactDist = fwdDistAtNextZCrossing;
			raycast.m_impactPos = currentPos;
			if( tileStepDirectionZ > 0 )
			{
				raycast.m_impactNormal = Vec3( 0.f, 0.f, -1.f );
			}
			else
			{
				raycast.m_impactNormal = Vec3( 0.f, 0.f, 1.f );
			}
			return raycast;
		}

		fwdDistAtNextZCrossing += fwdDistPerZCrossing;
	}
}

//-----------------------------------------------------------------------------------------------
RaycastResult3D Map::RaycastWorldActors( [[maybe_unused]] Vec3 const& start, [[maybe_unused]] Vec3 const& direction, [[maybe_unused]] float distance, [[maybe_unused]] Actor* owner /*= nullptr */ ) const
{
	RaycastResult3D result = RaycastResult3D();
	std::vector<RaycastResult3D> raycastResults;
	raycastResults.resize( m_actors.size() );

	for( unsigned int actorIndex = 0; actorIndex < m_actors.size(); actorIndex++ )
	{
		Actor* actor = m_actors[ actorIndex ];

		// Note: remove later //-----------------------------------------------------------------------------------------------
		if( actor == m_player )
		{
			continue;
		}

		raycastResults[ actorIndex ] = RaycastVsCylinder(start, direction, distance, actor->m_position, actor->m_physicsRadius, actor->m_physicsHeight);

		RaycastResult3D* raycast = &raycastResults[ actorIndex ];
		if( !result.m_didImpact && raycast->m_didImpact )
		{
			result = *raycast;
		}
		else if( result.m_didImpact && raycast->m_didImpact )
		{
			if( result.m_implactDist > raycast->m_implactDist )
			{
				result = *raycast;
			}
		}
	}

	return result;
}

//-----------------------------------------------------------------------------------------------
void Map::MouseControls()
{
	if( g_game->m_currentState != GAME_STATE_PLAY )
	{
		return;
	}

	if( g_engine->m_input->WasKeyJustPressed( KEYCODE_LEFT_MOUSE ) )
	{
		Vec3 startPosition = m_player->m_position;
		Vec3 direction = m_player->GetModelToWorldTransform().GetIBasis3D();
		Vec3 endPosition = startPosition + ( direction * 10.f );
		DebugAddWorldCylinder( startPosition, endPosition, 0.01f, 10.f, Rgba8::WHITE, Rgba8::WHITE, DebugRenderMode::X_RAY );

		RaycastResult3D raycast = RaycastAll( startPosition, direction, 10.f );

		if( raycast.m_didImpact )
		{
			DebugAddWorldSphere( raycast.m_impactPos, 0.06f, 10.f );
			Vec3 arrowStart = raycast.m_impactPos;
			Vec3 arrowEnd = raycast.m_impactPos + ( raycast.m_impactNormal * 0.3f );
			DebugAddWorldArrow( arrowStart, arrowEnd, 0.03f, 10.f, Rgba8::BLUE, Rgba8::BLUE );
		}
	}

	if( g_engine->m_input->WasKeyJustPressed( KEYCODE_RIGHT_MOUSE ) )
	{
		Vec3 startPosition = m_player->m_position;
		Vec3 direction = m_player->GetModelToWorldTransform().GetIBasis3D();
		Vec3 endPosition = startPosition + ( direction * 0.25f );
		DebugAddWorldCylinder( startPosition, endPosition, 0.01f, 10.f, Rgba8::WHITE, Rgba8::WHITE, DebugRenderMode::X_RAY );

		RaycastResult3D raycast = RaycastAll( startPosition, direction, 0.25f );

		if( raycast.m_didImpact )
		{
			DebugAddWorldSphere( raycast.m_impactPos, 0.06f, 10.f );
			Vec3 arrowStart = raycast.m_impactPos;
			Vec3 arrowEnd = raycast.m_impactPos + ( raycast.m_impactNormal * 0.3f );
			DebugAddWorldArrow( arrowStart, arrowEnd, 0.03f, 10.f, Rgba8::BLUE, Rgba8::BLUE );
		}
	}
}
