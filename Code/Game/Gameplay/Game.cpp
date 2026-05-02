#include "Game/Gameplay/Game.hpp"
#include "Game/Gameplay/Weapon.hpp"
#include "Game/Gameplay/WeaponDefinition.hpp"
#include "Game/Gameplay/Actors/Player.hpp"
#include "Game/Framework/App.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Game/Gameplay/TileDefinition.hpp"
#include "Game/Gameplay/MapDefinition.hpp"
#include "Game/Gameplay/ActorDefinition.hpp"
#include "Game/Gameplay/WeaponDefinition.hpp"


//-----------------------------------------------------------------------------------------------
Game* g_game = nullptr;
RandomNumberGenerator* g_rng = nullptr;
SpriteSheet* g_terrainSpriteSheet = nullptr;
BitmapFont* g_bitmapFont = nullptr;
NamedStrings* g_blackboard = nullptr;

//-----------------------------------------------------------------------------------------------
Game::Game()
{
	Startup();
}

//-----------------------------------------------------------------------------------------------
Game::~Game()
{
	delete m_currentMap;
	m_currentMap = nullptr;

	ActorDefinition::ClearActorDefs();
	WeaponDefinition::ClearWeaponDefs();
	MapDefinition::ClearMapDefs();
}

//-----------------------------------------------------------------------------------------------
void Game::Startup()
{
	g_game = this;

	m_gameClock = new Clock( Clock::GetSystemClock() );

	Shader* diffuseShader = g_engine->m_render->CreateOrGetShader( "Data/Shaders/Diffuse", VertexType::VERTEX_PCUTBN );
	g_engine->m_render->BindShader( diffuseShader );

	Texture* terrainTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Images/Terrain_8x8.png" );
	g_terrainSpriteSheet = new SpriteSheet( *terrainTexture, IntVec2( 8, 8 ) );

	MapDefinition::InitializeMapDefs();
	WeaponDefinition::InitializeWeaponDefs();
	ActorDefinition::InitializeProjectileActorDefs();
	ActorDefinition::InitializeActorDefs();

	TileDefinitions::InitializeTileDefs();

	g_blackboard = new NamedStrings();

	XmlDocument gameConfig;
	gameConfig.LoadFile( "Data/GameConfig.xml" );
	XmlElement* gameConfigElement = gameConfig.FirstChildElement();
	g_blackboard->PopulateFromXmlElementAttributes( *gameConfigElement );

	std::string mapName = g_blackboard->GetValue( "defaultMap", "TestMap" );
	m_currentMap = new Map( MapDefinition::GetMapDefFromName( mapName ) );

	SpawnInfo startingSpawn = m_currentMap->GetRandomSpawnPoint( Faction::MARINE );
	m_currentMap->SpawnPlayer( "Marine", startingSpawn.m_position, startingSpawn.m_orientation, 1 );

	m_screenCameraFull = new Camera();
	m_screenCameraFull->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	m_worldCameraP1 = m_currentMap->m_player1->m_camera;
	m_screenCameraP1 = new Camera();

	m_worldCameraP1->SetCameraToRenderTransform( Mat44::DirectXCameraToRenderMatrix );
	m_screenCameraP1->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	g_bitmapFont = g_engine->m_render->CreateOrGetBitmapFont( "Data/Fonts/SquirrelFixedFont" );

	g_engine->m_devConsole->PrintCommandsToConsole();
	AddCommandsToDevConsole();
	AddControlsToDevConsole();

	InitializeStartTriangleVerts();
	TransformVertexArrayXY3D( 3, m_startVerts, 1.f, 0.f, Vec2( SCREEN_CENTER_X, SCREEN_CENTER_Y ) );

	Vec2 worldCenter( WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f );

	m_worldCameraP1->SetPosition( m_currentMap->m_player1->m_position );
	m_worldCameraP1->SetOrientation( m_currentMap->m_player1->m_orientation );
}

//-----------------------------------------------------------------------------------------------
void Game::Update()
{
	float deltaSeconds = static_cast<float>( m_gameClock->GetDeltaSeconds() );

	UpdateStates();
	UpdateKeyboardInput();
	UpdateControllerInput();

	if( m_currentState == GAME_STATE_ATTRACT )
	{
		UpdateAttractMode( deltaSeconds );
		return;
	}
	if( m_currentState == GAME_STATE_LOBBY )
	{
		UpdateLobbyMode();
		return;
	}

	UpdateEntities( deltaSeconds );
	UpdateMap( deltaSeconds );
}

//-----------------------------------------------------------------------------------------------
void Game::Render()
{
	if( m_currentState == GAME_STATE_ATTRACT )
	{
		g_engine->m_render->SetLightConstants( Vec3(), 0.f, 1.f );
		RenderAttractMode();
		return;
	}
	if( m_currentState == GAME_STATE_LOBBY )
	{
		RenderLobbyMode();
		return;
	}

	// Get Camera Area
	Vec2 topLeft = Camera::GetTopLeftInViewportSpace( m_player1CameraBounds, SCREEN_SIZE_X, SCREEN_SIZE_Y );
	float width = Camera::GetViewportWidth( m_player1CameraBounds, SCREEN_SIZE_X );
	float height = Camera::GetViewportHeight( m_player1CameraBounds, SCREEN_SIZE_Y );

	m_renderPass = PlayerRenderPass::PLAYER_ONE;
	g_engine->m_render->BeginCamera( *m_worldCameraP1, topLeft, width, height );
	RenderMap();

	g_engine->m_render->BeginCamera( *m_screenCameraP1, topLeft, width, height );
	RenderHud( m_currentMap->m_player1, m_screenCameraP1 );

	if( IsTwoPlayer() )
	{
		topLeft = Camera::GetTopLeftInViewportSpace( m_player2CameraBounds, SCREEN_SIZE_X, SCREEN_SIZE_Y );
		width = Camera::GetViewportWidth( m_player2CameraBounds, SCREEN_SIZE_X );
		height = Camera::GetViewportHeight( m_player2CameraBounds, SCREEN_SIZE_Y );

		m_renderPass = PlayerRenderPass::PLAYER_TWO;
		g_engine->m_render->BeginCamera( *m_worldCameraP2, topLeft, width, height );
		RenderMap();

		g_engine->m_render->BeginCamera( *m_screenCameraP2, topLeft, width, height );
		RenderHud( m_currentMap->m_player2, m_screenCameraP2 );
	}

	if( g_engine->m_devConsole->IsOpen() )
	{
		g_engine->m_render->RenderSetup( nullptr, BlendMode::ALPHA );
		AABB2 screenBounds = AABB2( m_screenCameraFull->GetOrthoBottomLeft(), m_screenCameraFull->GetOrthoTopRight() );
		g_engine->m_devConsole->Render( screenBounds );
	}
}

//-----------------------------------------------------------------------------------------------
void Game::Shutdown()
{
	delete m_gameClock;
	m_gameClock = nullptr;

	/*g_engine->m_audio->StopSound( m_gameMusic );
	g_engine->m_audio->StopSound( m_menuMusic );*/

	g_engine->m_devConsole->ClearLog();
}

//-----------------------------------------------------------------------------------------------
void Game::SetGameMusicSpeed( float speed )
{
	g_engine->m_audio->SetSoundPlaybackSpeed( m_music, speed );
}

void Game::UpdateStates()
{
	if( m_currentState != m_nextState )
	{
		if( m_currentState != GAME_STATE_INVALID )
		{
			g_engine->m_audio->StartSound( audio_testSound );
		}
		if( m_nextState == GAME_STATE_PLAY )
		{
			
		}
		if( m_nextState == GAME_STATE_ATTRACT )
		{
			
		}

		m_currentState = m_nextState;
	}
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateAttractMode( [[maybe_unused]] float deltaSeconds )
{
	m_time += deltaSeconds * 1.25f;
	m_startAlpha = 127.5f * cosf( m_time * 2.0f ) + 127.5f;

	for( int startIndex = 0; startIndex < 3; startIndex++ )
	{
		m_startVerts[ startIndex ].m_color = Rgba8( 0, 255, 0, (unsigned int)m_startAlpha );
	}
}

//-----------------------------------------------------------------------------------------------
void Game::RenderAttractMode() const
{
	g_engine->m_render->BeginCamera( *m_screenCameraFull );

	// Draw Start Button
	g_engine->m_render->SetModelConstants();
	g_engine->m_render->SetBlendMode( BlendMode::ALPHA );
	g_engine->m_render->SetBlendStateIfChanged();
	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->DrawVertexArray( 3, m_startVerts );

	// Draw Text
	std::vector<Vertex> textAttractVerts;
	AddVertsForTextTriangles2D( textAttractVerts, "Attract", Vec2( 600.f, 700.f ), 40.f, Rgba8( 255, 0, 0 ) );
	g_engine->m_render->SetModelConstants();
	g_engine->m_render->SetBlendStateIfChanged();
	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->DrawVertexArray( (int)textAttractVerts.size(), textAttractVerts.data() );

	std::vector<Vertex> textScreenVerts;
	AddVertsForTextTriangles2D( textScreenVerts, "Screen", Vec2( 800.f, 700.f ), 40.f, Rgba8( 0, 0, 255 ) );
	g_engine->m_render->SetModelConstants();
	g_engine->m_render->SetBlendStateIfChanged();
	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->DrawVertexArray( (int)textScreenVerts.size(), textScreenVerts.data() );
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateLobbyMode()
{
	XboxController const& controller = g_engine->m_input->m_controllers[ 0 ];
	if( controller.WasButtonJustPressed( XboxButtonID::A ) && !m_isTwoPlayer )
	{
		m_isTwoPlayer = true;

		// Spawn Player 2
		SpawnInfo startingSpawn = m_currentMap->GetRandomSpawnPoint( Faction::MARINE );
		m_currentMap->SpawnPlayer( "Marine", startingSpawn.m_position, startingSpawn.m_orientation, 2 );

		// Player 1 Camera Setup
		m_player1CameraBounds.m_mins.y = SCREEN_SIZE_Y * 0.5f;
		m_screenCameraP1->SetOrthoView( Vec2( 0.f, m_player1CameraBounds.m_mins.y ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

		// Player 2 Camera Setup
		m_worldCameraP2 = m_currentMap->m_player2->m_camera;
		m_screenCameraP2 = new Camera();

		m_player2CameraBounds = AABB2( 0.f, 0.f, SCREEN_SIZE_X, SCREEN_SIZE_Y * 0.5f );
		m_worldCameraP2->SetCameraToRenderTransform( Mat44::DirectXCameraToRenderMatrix );
		m_screenCameraP2->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, m_player2CameraBounds.m_maxs.y ) );

		m_worldCameraP2->SetPosition( m_currentMap->m_player2->m_position );
		m_worldCameraP2->SetOrientation( m_currentMap->m_player2->m_orientation );
	}
	else if( controller.WasButtonJustPressed( XboxButtonID::A ) && m_isTwoPlayer )
	{
		m_isTwoPlayer = false;
		
		Player* player2 = m_currentMap->m_player2;
		Actor* player2Actor = player2->GetActor();
		player2Actor->m_isDead = true;
		player2Actor->m_isGarbage = true;
		player2->Unposses( player2Actor->m_actorHandle );

		delete player2;
		player2 = nullptr;

		m_player1CameraBounds.m_mins.y = 0.f;
		m_screenCameraP1->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );
	}
}

//-----------------------------------------------------------------------------------------------
void Game::RenderLobbyMode() const
{
	g_engine->m_render->BeginCamera( *m_screenCameraFull );

	// Draw Text
	std::vector<Vertex> textLobbyVerts;
	AddVertsForTextTriangles2D( textLobbyVerts, "Lobby", Vec2( 700.f, 700.f ), 40.f, Rgba8( 255, 255, 255 ) );

	// Draw Text
	if( !m_isTwoPlayer )
	{
		AddVertsForTextTriangles2D( textLobbyVerts, "Mode: Single Player", Vec2( 550.f, 400.f ), 40.f, Rgba8( 255, 255, 255 ) );
	}
	else
	{
		AddVertsForTextTriangles2D( textLobbyVerts, "Mode: Two Player", Vec2( 570.f, 400.f ), 40.f, Rgba8( 255, 255, 255 ) );
	}

	AddVertsForTextTriangles2D( textLobbyVerts, "Press 'A' on a controller to join as Player 2", Vec2( 490.f, 200.f ), 20.f, Rgba8( 255, 255, 255 ) );
	AddVertsForTextTriangles2D( textLobbyVerts, "Press 'A' again to disconnect Player 2", Vec2( 540.f, 175.f ), 20.f, Rgba8( 255, 255, 255 ) );
	AddVertsForTextTriangles2D( textLobbyVerts, "Press 'Space' or 'Start' after joining to begin", Vec2( 490.f, 150.f ), 20.f, Rgba8( 255, 255, 255 ) );

	g_engine->m_render->SetModelConstants();
	g_engine->m_render->SetBlendStateIfChanged();
	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->DrawVertexArray( (int)textLobbyVerts.size(), textLobbyVerts.data() );
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateEntities( float deltaSeconds )
{
	m_time += deltaSeconds;
	m_startAlpha = 127.5f * cosf( m_time * 2.0f ) + 127.5f;

	DebugAddDebugText();
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateMap( float deltaSeconds )
{
	m_currentMap->Update( deltaSeconds );
}

//-----------------------------------------------------------------------------------------------
void Game::RenderEntities() const
{
	g_engine->m_render->BeginCamera( *m_worldCameraP1 );

	g_engine->m_render->BindTexture( nullptr );

	g_engine->m_render->EndCamera( *m_worldCameraP1 );
}

//-----------------------------------------------------------------------------------------------
void Game::RenderMap() const
{
	Vec3 normalizedLighting = m_sunDirection.GetNormalized();
	g_engine->m_render->SetLightConstants( normalizedLighting, m_sunIntensity, m_ambientIntensity );

	m_currentMap->Render();
}

//-----------------------------------------------------------------------------------------------
void Game::RenderHud( Player* player, Camera* screenCamera ) const
{
	Weapon* currentWeapon = player->GetActor()->m_equippedWeapon;
	WeaponDefinition weaponDef = *currentWeapon->m_definition;

	if( weaponDef.GetType() == WEAPON_TYPE_MELEE )
	{
		return;
	}

	float scaleMultiplyer = ( m_isTwoPlayer ) ? 0.5f : 1.f;
	float textScaleMultiplyer = ( m_isTwoPlayer ) ? 0.75f : 1.f;
	float heightOffset = ( player == m_currentMap->m_player1 ) ? 420.f : 20.f;
	heightOffset = ( m_isTwoPlayer ) ? heightOffset : 50.f;

	// Hud Base
	AABB2 hudBaseBox = AABB2( 0.f, screenCamera->GetOrthoBottomLeft().y, SCREEN_SIZE_X, screenCamera->GetOrthoBottomLeft().y + 117.4312f * scaleMultiplyer );
	VertexList hudBaseVerts;
	AddVertsForAABB2D( hudBaseVerts, hudBaseBox, Rgba8::WHITE, AABB2::ZERO_TO_ONE );
	Texture* hudBaseTexture = g_engine->m_render->CreateOrGetTextureFromFile( weaponDef.GetHud().m_baseTexture.c_str() );
	g_engine->m_render->RenderSetup( hudBaseTexture );
	g_engine->m_render->DrawVertexArray( hudBaseVerts );

	// Hud Stats
	// Health
	VertexList healthTextVerts;
	int health = player->GetActor()->m_health;
	if( health >= 100 )
	{
		g_bitmapFont->AddVertsForText2D( healthTextVerts, Vec2( 430.f, heightOffset ), 40.f * textScaleMultiplyer, std::to_string( health ) );
	}
	else
	{
		g_bitmapFont->AddVertsForText2D( healthTextVerts, Vec2( 450.f, heightOffset ), 40.f * textScaleMultiplyer, std::to_string( health ) );
	}

	// Kills
	VertexList killsTextVerts;
	g_bitmapFont->AddVertsForText2D( killsTextVerts, Vec2( 90.f, heightOffset ), 40.f * textScaleMultiplyer, std::to_string( player->m_kills ) );

	// Deaths
	VertexList deathTextVerts;
	g_bitmapFont->AddVertsForText2D( deathTextVerts, Vec2( 1480.f, heightOffset ), 40.f * textScaleMultiplyer, std::to_string( player->m_deaths ) );

	g_engine->m_render->RenderSetup( &g_bitmapFont->GetTexture() );
	g_engine->m_render->DrawVertexArray( healthTextVerts );
	g_engine->m_render->DrawVertexArray( killsTextVerts );
	g_engine->m_render->DrawVertexArray( deathTextVerts );

	// Reticle
	AABB2 reticleBox = AABB2( screenCamera->GetCenter().x - 8.f * scaleMultiplyer, screenCamera->GetCenter().y - 8.f * scaleMultiplyer,
		screenCamera->GetCenter().x + 8.f * scaleMultiplyer, screenCamera->GetCenter().y + 8.f * scaleMultiplyer );
	VertexList reticleVerts;
	AddVertsForAABB2D( reticleVerts, reticleBox, Rgba8::WHITE, AABB2::ZERO_TO_ONE );
	Texture* reticleTexture = g_engine->m_render->CreateOrGetTextureFromFile( weaponDef.GetHud().m_reticleTexture.c_str() );
	g_engine->m_render->RenderSetup( reticleTexture );
	g_engine->m_render->DrawVertexArray( reticleVerts );

	// Weapon
	Vec2 weaponSpriteSize = weaponDef.GetHud().m_spriteSize;
	Texture* weaponTexture = nullptr;
	SpriteSheet* weaponSpriteSheet = weaponDef.GetAnimationByState( currentWeapon->m_currentAnim )->m_spriteSheet;
	
	AABB2 uvBox = AABB2::ZERO_TO_ONE;
	if( weaponSpriteSheet != nullptr )
	{
		WeaponAnimation weaponAnimation = *weaponDef.GetAnimationByState( currentWeapon->m_currentAnim );
		const SpriteDefinition& spriteDef = weaponAnimation.m_animDef->GetSpriteDefAtTime( static_cast<float>( currentWeapon->m_animClock->GetTotalSeconds() ) );
		Vec2 uvMins, uvMaxs;
		spriteDef.GetUVs( uvMins, uvMaxs );
		uvBox = AABB2( uvMins, uvMaxs );
		weaponTexture = &weaponSpriteSheet->GetTexture();
	}

	AABB2 weaponBox = AABB2( screenCamera->GetCenter().x - weaponSpriteSize.x * 0.5f * scaleMultiplyer, hudBaseBox.m_maxs.y, 
		screenCamera->GetCenter().x + weaponSpriteSize.x * 0.5f * scaleMultiplyer, hudBaseBox.m_maxs.y + weaponSpriteSize.y * scaleMultiplyer );
	VertexList weaponVerts;
	AddVertsForAABB2D( weaponVerts, weaponBox, Rgba8::WHITE, uvBox );

	g_engine->m_render->RenderSetup( weaponTexture );
	g_engine->m_render->DrawVertexArray( weaponVerts );
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateKeyboardInput()
{
	if( m_currentState != GAME_STATE_PLAY )
	{
		return;
	}

	if( g_engine->m_input->WasKeyJustPressed( '7' ) )
	{
		AABB2 textBox = AABB2( 1.f, 770.f, 800.f, 785.f );
		std::string text = Stringf( "Camera Orientation: %.2f, %.2f, %.2f", m_worldCameraP1->GetOrientation().m_yawDegrees, m_worldCameraP1->GetOrientation().m_pitchDegrees, m_worldCameraP1->GetOrientation().m_rollDegrees );
		DebugAddScreenText( text, textBox, 15.f, Vec2( 0.f, 1.f ), 5.f );
	}

	if( g_engine->m_input->WasKeyJustPressed( KEYCODE_F2 ) )
	{
		m_sunDirection.x -= 1.f;
		AABB2 textBox = AABB2( 1.f, 775.f, 800.f, 785.f );
		std::string text = Stringf( "Sun Direction X: %0.f", m_sunDirection.x );
		DebugAddScreenText( text, textBox, 10.f, Vec2( 0.f, 1.f ), 2.f );
	}
	if( g_engine->m_input->WasKeyJustPressed( KEYCODE_F3 ) )
	{
		m_sunDirection.x += 1.f;
		AABB2 textBox = AABB2( 1.f, 775.f, 800.f, 785.f );
		std::string text = Stringf( "Sun Direction X: %0.f", m_sunDirection.x );
		DebugAddScreenText( text, textBox, 10.f, Vec2( 0.f, 1.f ), 2.f );
	}

	if( g_engine->m_input->WasKeyJustPressed( KEYCODE_F4 ) )
	{
		m_sunDirection.y -= 1.f;
		AABB2 textBox = AABB2( 1.f, 775.f, 800.f, 785.f );
		std::string text = Stringf( "Sun Direction Y: %0.f", m_sunDirection.y );
		DebugAddScreenText( text, textBox, 10.f, Vec2( 0.f, 1.f ), 2.f );
	}
	if( g_engine->m_input->WasKeyJustPressed( KEYCODE_F5 ) )
	{
		m_sunDirection.y += 1.f;
		AABB2 textBox = AABB2( 1.f, 775.f, 800.f, 785.f );
		std::string text = Stringf( "Sun Direction Y: %0.f", m_sunDirection.y );
		DebugAddScreenText( text, textBox, 10.f, Vec2( 0.f, 1.f ), 2.f );
	}

	if( g_engine->m_input->WasKeyJustPressed( KEYCODE_F6 ) )
	{
		m_sunIntensity -= 0.05f;
		AABB2 textBox = AABB2( 1.f, 775.f, 800.f, 785.f );
		std::string text = Stringf( "Sun Intensity: %.2f", m_sunIntensity );
		DebugAddScreenText( text, textBox, 10.f, Vec2( 0.f, 1.f ), 2.f );
	}
	if( g_engine->m_input->WasKeyJustPressed( KEYCODE_F7 ) )
	{
		m_sunIntensity += 0.05f;
		AABB2 textBox = AABB2( 1.f, 775.f, 800.f, 785.f );
		std::string text = Stringf( "Sun Intensity: %.2f", m_sunIntensity );
		DebugAddScreenText( text, textBox, 10.f, Vec2( 0.f, 1.f ), 2.f );
	}

	if( g_engine->m_input->WasKeyJustPressed( KEYCODE_F8 ) )
	{
		m_ambientIntensity -= 0.05f;
		AABB2 textBox = AABB2( 1.f, 775.f, 800.f, 785.f );
		std::string text = Stringf( "Ambient Intensity: %.2f", m_ambientIntensity );
		DebugAddScreenText( text, textBox, 10.f, Vec2( 0.f, 1.f ), 2.f );
	}
	if( g_engine->m_input->WasKeyJustPressed( KEYCODE_F9 ) )
	{
		m_ambientIntensity += 0.05f;
		AABB2 textBox = AABB2( 1.f, 775.f, 800.f, 785.f );
		std::string text = Stringf( "Ambient Intensity: %.2f", m_ambientIntensity );
		DebugAddScreenText( text, textBox, 10.f, Vec2( 0.f, 1.f ), 2.f );
	}
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateControllerInput()
{
	XboxController const& controller = g_engine->m_input->m_controllers[ 0 ];

	if( m_currentState == GAME_STATE_ATTRACT && ( controller.WasButtonJustPressed( XboxButtonID::START ) ) )
	{
		m_nextState = GAME_STATE_LOBBY;
	}
	if( m_currentState == GAME_STATE_LOBBY && ( controller.WasButtonJustPressed( XboxButtonID::START ) ) )
	{
		m_nextState = GAME_STATE_PLAY;
	}

	if( m_currentState == GAME_STATE_ATTRACT && controller.WasButtonJustPressed( XboxButtonID::SELECT ) )
	{
		g_app->m_isQuitting = true;
	}

	if( m_currentState == GAME_STATE_PLAY && controller.WasButtonJustPressed( XboxButtonID::SELECT ) )
	{
		m_nextState = GAME_STATE_ATTRACT;
	}

	if( m_currentState == GAME_STATE_LOBBY && controller.WasButtonJustPressed( XboxButtonID::SELECT ) )
	{
		m_nextState = GAME_STATE_ATTRACT;
	}
}

//-----------------------------------------------------------------------------------------------
bool Game::IsTwoPlayer() const
{
	return m_isTwoPlayer;
}

//-----------------------------------------------------------------------------------------------
void Game::InitializeStartTriangleVerts()
{
	m_startVerts[ 0 ].m_pos = Vec3( -200.f, 200.f, 0.f );
	m_startVerts[ 1 ].m_pos = Vec3( -200.f, -200.f, 0.f );
	m_startVerts[ 2 ].m_pos = Vec3( 200.f, 0.f, 0.f );

	for( int vertIndex = 0; vertIndex < 3; vertIndex++ )
	{
		m_startVerts[ vertIndex ].m_color = Rgba8( 0, 255, 0 );
	}
}

//-----------------------------------------------------------------------------------------------
void Game::DebugRenderEntities() const
{
	
}

//-----------------------------------------------------------------------------------------------
void Game::DebugDrawWorldBounds() const
{
	DebugDrawLine( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ), 5.f, Rgba8( 255, 0, 255 ) );
	DebugDrawLine( Vec2( 0.f, WORLD_SIZE_Y ), Vec2( WORLD_SIZE_X, 0.f ), 5.f, Rgba8( 255, 0, 255 ) );
	DebugDrawLine( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, 0.f ), 5.f, Rgba8( 255, 0, 255 ) );
	DebugDrawLine( Vec2( 0.f, WORLD_SIZE_Y ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ), 5.f, Rgba8( 255, 0, 255 ) );
	DebugDrawLine( Vec2( 0.f, 0.f ), Vec2( 0.f, WORLD_SIZE_Y ), 5.f, Rgba8( 255, 0, 255 ) );
	DebugDrawLine( Vec2( WORLD_SIZE_X, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ), 5.f, Rgba8( 255, 0, 255 ) );
}

//-----------------------------------------------------------------------------------------------
void Game::DebugAddDebugText() const
{
	/*AABB2 positionBox = AABB2( 1.f, 784.f, 800.f, 799.f );
	std::string positionText = Stringf( "Position: %.2f, %.2f, %.2f", m_worldCamera->GetPosition().x, m_worldCamera->GetPosition().y, m_worldCamera->GetPosition().z );
	DebugAddScreenText( positionText, positionBox, 15.f, Vec2( 0.f, 1.f ), 0.f );*/

	/*AABB2 healthBox = AABB2( 1.f, 784.f, 800.f, 799.f );
	std::string healthText = Stringf( "Health: %d", m_currentMap->GetActorByHandle( m_currentMap->m_player1->m_actorHandle )->m_health );
	DebugAddScreenText( healthText, healthBox, 15.f, Vec2( 0.f, 1.f ), 0.f );*/

	AABB2 timeBox = AABB2( 800.f, 784.f, 1599.f, 799.f );
	float totalSeconds = static_cast<float>( Clock::GetSystemClock().GetTotalSeconds() );
	float framesPerSecond = static_cast<float>( Clock::GetSystemClock().GetFrameCount() ) / totalSeconds;
	float timeScale = static_cast<float>( Clock::GetSystemClock().GetTimeScale() );
	std::string timeText = Stringf( "Time: %.2f FPS: %.1f Scale: %.2f", totalSeconds, framesPerSecond, timeScale );
	DebugAddScreenText( timeText, timeBox, 15.f, Vec2( 1.f, 0.f ), 0.f );

	//if( m_currentMap->m_isFreeFly )
	//{
	//	AABB2 actorTextbox = AABB2( 380.f, 784.f, 1179.f, 799.f );
	//	std::string actorText = "[F1] Control Mode: Actor";
	//	DebugAddScreenText( actorText, actorTextbox, 15.f, Vec2( 0.5f, 0.5f ), 0.f, Rgba8::BLUE, Rgba8::BLUE );
	//}
	//else
	//{
	//	AABB2 cameraTextbox = AABB2( 380.f, 784.f, 1179.f, 799.f );
	//	std::string cameraText = "[F1] Control Mode: Camera";
	//	DebugAddScreenText( cameraText, cameraTextbox, 15.f, Vec2( 0.5f, 0.5f ), 0.f );
	//}

	/*AABB2 lightingBox = AABB2( 800.f, 739.f, 1599.f, 784.f );
	std::string lightingText = Stringf( "Sun Direction X: %.0f [F2 / F3 to change]\nSun Direction Y: %0.f [F4 / F5 to change]\nSun Intensity: %.2f [F6 / F7 to change]\n Ambient Intensity: %.2f [F8 / F9 to change]",
		g_game->m_sunDirection.x, g_game->m_sunDirection.y, m_sunIntensity, m_ambientIntensity );
	DebugAddScreenText( lightingText, lightingBox, 25.f, Vec2( 1.f, 0.f ), 0.f );*/
}

//-----------------------------------------------------------------------------------------------
void Game::AddCommandsToDevConsole()
{

}

//-----------------------------------------------------------------------------------------------
void Game::AddControlsToDevConsole()
{
	g_engine->m_devConsole->AddLine( DevConsole::INFO_MAJOR, "Controls:" );
	g_engine->m_devConsole->AddLine( DevConsole::INFO_MINOR, " Yaw and Pitch: Mouse, Right Stick" );
	g_engine->m_devConsole->AddLine( DevConsole::INFO_MINOR, " Roll: Q/E, Left/Right Trigger" );
	g_engine->m_devConsole->AddLine( DevConsole::INFO_MINOR, " Move left and right: A/D, Left Stick" );
	g_engine->m_devConsole->AddLine( DevConsole::INFO_MINOR, " Move forward and back: W/S, Left Stick" );
	g_engine->m_devConsole->AddLine( DevConsole::INFO_MINOR, " Move up and down: C/Z, Left/Right Shoulder" );
	g_engine->m_devConsole->AddLine( DevConsole::INFO_MINOR, " Reset position and orientation: H, Start" );
	g_engine->m_devConsole->AddLine( DevConsole::INFO_MINOR, " Increase speed: Shift, A button" );
	g_engine->m_devConsole->AddLine( DevConsole::INFO_MINOR, " Pause: P" );
	g_engine->m_devConsole->AddLine( DevConsole::INFO_MINOR, " Step Frame: O" );
	g_engine->m_devConsole->AddLine( DevConsole::INFO_MINOR, " Slow motion: T" );
}