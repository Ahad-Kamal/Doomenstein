#include "Game/Gameplay/Game.hpp"
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

	MapDefinition::InitializeMapDefs();
	WeaponDefinition::InitializeWeaponDefs();
	ActorDefinition::InitializeProjectileActorDefs();
	ActorDefinition::InitializeActorDefs();

	Texture* terrainTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Images/Terrain_8x8.png" );
	g_terrainSpriteSheet = new SpriteSheet( *terrainTexture, IntVec2( 8, 8 ) );
	TileDefinitions::InitializeTileDefs();

	g_blackboard = new NamedStrings();

	XmlDocument gameConfig;
	gameConfig.LoadFile( "Data/GameConfig.xml" );
	XmlElement* gameConfigElement = gameConfig.FirstChildElement();
	g_blackboard->PopulateFromXmlElementAttributes( *gameConfigElement );

	std::string mapName = g_blackboard->GetValue( "defaultMap", "TestMap" );
	m_currentMap = new Map( MapDefinition::GetMapDefFromName( mapName ) );
	m_currentMap->SpawnPlayer( "Marine", Vec3(2.5f, 8.5f, 0.f), EulerAngles() );

	m_worldCamera = m_currentMap->m_player->m_camera;
	m_screenCamera = new Camera();

	m_worldCamera->SetCameraToRenderTransform( Mat44::DirectXCameraToRenderMatrix );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	g_bitmapFont = g_engine->m_render->CreateOrGetBitmapFont( "Data/Fonts/SquirrelFixedFont" );

	g_engine->m_devConsole->PrintCommandsToConsole();
	AddCommandsToDevConsole();
	AddControlsToDevConsole();

	InitializeStartTriangleVerts();
	TransformVertexArrayXY3D( 3, m_startVerts, 1.f, 0.f, Vec2( SCREEN_CENTER_X, SCREEN_CENTER_Y ) );

	Vec2 worldCenter( WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f );

	m_worldCamera->SetPosition( m_currentMap->m_player->m_position );
	m_worldCamera->SetOrientation( m_currentMap->m_player->m_orientation );
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

	UpdateEntities( deltaSeconds );
	UpdateMap( deltaSeconds );
}

//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	if( m_currentState == GAME_STATE_ATTRACT )
	{
		g_engine->m_render->SetLightConstants( Vec3(), 0.f, 1.f );
		RenderAttractMode();
		return;
	}

	Vec3 normalizedLighting = m_sunDirection.GetNormalized();
	g_engine->m_render->SetLightConstants( normalizedLighting, m_sunIntensity, m_ambientIntensity );
	RenderMap();

	if( g_engine->m_devConsole->IsOpen() )
	{
		g_engine->m_render->BeginCamera( *m_screenCamera );
		g_engine->m_render->SetLightConstants( Vec3(), 0.f, 1.f );
		AABB2 screenBounds = AABB2( m_screenCamera->GetOrthoBottomLeft(), m_screenCamera->GetOrthoTopRight() );
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
	g_engine->m_render->BeginCamera( *m_screenCamera );

	// Draw Start Button
	g_engine->m_render->SetModelConstants();
	g_engine->m_render->SetBlendMode( BlendMode::ALPHA );
	g_engine->m_render->SetBlendStateIfChanged();
	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->DrawVertexArray( 3, m_startVerts );

	// Draw Text
	std::vector<Vertex> textStarshipDropShadowVerts;
	AddVertsForTextTriangles2D( textStarshipDropShadowVerts, "Attract", Vec2( 600.f, 700.f ), 40.f, Rgba8( 255, 0, 0 ) );
	g_engine->m_render->SetModelConstants();
	g_engine->m_render->SetBlendStateIfChanged();
	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->DrawVertexArray( (int)textStarshipDropShadowVerts.size(), textStarshipDropShadowVerts.data() );

	std::vector<Vertex> textGoldDropShadowVerts;
	AddVertsForTextTriangles2D( textGoldDropShadowVerts, "Screen", Vec2( 800.f, 700.f ), 40.f, Rgba8( 0, 0, 255 ) );
	g_engine->m_render->SetModelConstants();
	g_engine->m_render->SetBlendStateIfChanged();
	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->DrawVertexArray( (int)textGoldDropShadowVerts.size(), textGoldDropShadowVerts.data() );

	// Draw Textured AABB2
	Image image = Image( "Data/Maps/TestMap.png" );
	/*std::vector<Vertex> textureVerts;
	AABB2 textureBox = AABB2( 20.f, 100.f, 532.f, 612.f );
	AddVertsForAABB2D( textureVerts, textureBox, Rgba8( 255, 255, 255 ) );
	
	Texture* testTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
	g_engine->m_render->BindTexture( testTexture );
	g_engine->m_render->DrawVertexArray( textureVerts );
	
	g_engine->m_render->BindTexture( nullptr );	*/
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
	g_engine->m_render->BeginCamera( *m_worldCamera );

	g_engine->m_render->BindTexture( nullptr );

	g_engine->m_render->EndCamera( *m_worldCamera );
}

//-----------------------------------------------------------------------------------------------
void Game::RenderMap() const
{
	m_currentMap->Render();
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateKeyboardInput()
{
	if( m_currentState != GAME_STATE_PLAY )
	{
		return;
	}

	/*if( g_engine->m_input->WasKeyJustPressed( '1' ) )
	{
		Vec3 endPosition = m_currentMap->m_player->m_position + ( m_currentMap->m_player->GetModelToWorldTransform().GetIBasis3D() * 20.f );
		DebugAddWorldCylinder( Vec3( m_currentMap->m_player->m_position ), endPosition,
			0.0625f, 10.f, Rgba8::YELLOW, Rgba8::YELLOW, DebugRenderMode::X_RAY );
	}

	if( g_engine->m_input->IsKeyDown( '2' ) )
	{
		DebugAddWorldSphere( Vec3( m_currentMap->m_player->m_position.x,m_currentMap-> m_player->m_position.y, 0.f ), 0.5f, 60.f, Rgba8( 150, 75, 0 ), Rgba8( 150, 75, 0 ) );
	}

	if( g_engine->m_input->WasKeyJustPressed( '3' ) )
	{
		Mat44 playerMat = m_currentMap->m_player->GetModelToWorldTransform();
		playerMat.AppendTranslation2D( Vec2( 2.f, 0.f ) );
		DebugAddWorldWireSphere( playerMat.GetTranslation3D(), 1.f, 5.f, Rgba8::GREEN, Rgba8::RED );
	}

	if( g_engine->m_input->WasKeyJustPressed( '4' ) )
	{
		DebugAddBasis( m_currentMap->m_player->GetModelToWorldTransform(), 20.f, 1.f, 0.125f );
	}

	if( g_engine->m_input->WasKeyJustPressed( '5' ) )
	{
		Vec3 position = m_currentMap->m_player->GetModelToWorldTransform().GetTranslation3D() + m_currentMap->m_player->GetModelToWorldTransform().GetIBasis3D() * 10.f;
		std::string text = Stringf("Position: %.2f, %.2f, %.2f", m_currentMap->m_player->m_position.x, m_currentMap->m_player->m_position.y, m_currentMap->m_player->m_position.z );
		DebugAddWorldBillboardText( text, position, 1.f, Vec2( 0.5f, 0.5f ), 5.f, Rgba8::WHITE, Rgba8::RED );
	}

	if( g_engine->m_input->WasKeyJustPressed( '6' ) )
	{
		DebugAddWorldWireCylinder( Vec3( m_currentMap->m_player->m_position.x, m_currentMap->m_player->m_position.y, m_currentMap->m_player->m_position.z - 0.5f ), 
			Vec3( m_currentMap->m_player->m_position.x, m_currentMap->m_player->m_position.y, m_currentMap->m_player->m_position.z + 0.5f ), 0.5f, 10.f, Rgba8::WHITE, Rgba8::RED );
	}*/

	if( g_engine->m_input->WasKeyJustPressed( '7' ) )
	{
		AABB2 textBox = AABB2( 1.f, 770.f, 800.f, 785.f );
		std::string text = Stringf( "Camera Orientation: %.2f, %.2f, %.2f", m_worldCamera->GetOrientation().m_yawDegrees, m_worldCamera->GetOrientation().m_pitchDegrees, m_worldCamera->GetOrientation().m_rollDegrees );
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

	if( m_currentState == GAME_STATE_ATTRACT && ( controller.WasButtonJustPressed( XboxButtonID::START ) || controller.WasButtonJustPressed( XboxButtonID::A ) ) )
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
	AABB2 positionBox = AABB2( 1.f, 784.f, 800.f, 799.f );
	std::string positionText = Stringf( "Position: %.2f, %.2f, %.2f", m_worldCamera->GetPosition().x, m_worldCamera->GetPosition().y, m_worldCamera->GetPosition().z );
	DebugAddScreenText( positionText, positionBox, 15.f, Vec2( 0.f, 1.f ), 0.f );

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

	AABB2 lightingBox = AABB2( 800.f, 739.f, 1599.f, 784.f );
	std::string lightingText = Stringf( "Sun Direction X: %.0f [F2 / F3 to change]\nSun Direction Y: %0.f [F4 / F5 to change]\nSun Intensity: %.2f [F6 / F7 to change]\n Ambient Intensity: %.2f [F8 / F9 to change]",
		g_game->m_sunDirection.x, g_game->m_sunDirection.y, m_sunIntensity, m_ambientIntensity );
	DebugAddScreenText( lightingText, lightingBox, 25.f, Vec2( 1.f, 0.f ), 0.f );
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