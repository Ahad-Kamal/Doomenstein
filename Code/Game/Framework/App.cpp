#include "Game/Framework/App.hpp"
#include "Game/Gameplay/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Engine/Math/IntVec2.hpp"
#include "ThirdParty/stb/stb_image.h"

#include <math.h>


//-----------------------------------------------------------------------------------------------
App* g_app = nullptr;
Rgba8 g_clearColor = Rgba8( 50, 50, 50, 255 );


//-----------------------------------------------------------------------------------------------
App::App()
{
	EngineConfig config;
	config.m_windowConfig.m_clientAspect = 2.f;
	config.m_windowConfig.m_windowTitle = "Doomenstein";

	g_engine = new Engine( config );
	g_game = new Game();
	m_lastFrameTime = static_cast<float>( GetCurrentTimeSeconds() );

	SubscribeEventCallbackFunction( "Quit", EventQuit );

	DebugRenderSystemStartup( m_debugConfig );
}

//-----------------------------------------------------------------------------------------------
App::~App()
{
	DebugRenderSystemShutdown();

	delete g_game;
	g_game = nullptr;

	delete g_engine;
	g_engine = nullptr;
}

//-----------------------------------------------------------------------------------------------
void App::RunMainLoop()
{
	while( !IsQuitting() )
	{
		RunFrame();
	}
}

//-----------------------------------------------------------------------------------------------
void App::RunFrame()
{
	Clock::TickSystemClock();

	// One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.
	g_engine->BeginFrame(); // Allow engine subsystems to do pre-frame stuff
	Update();		
	DebugRenderBeginFrame();
	Render();		
	DebugRenderWorld( *g_game->m_worldCameraP1 );
	DebugRenderScreen( *g_game->m_screenCameraFull );
	DebugRenderEndFrame();
	g_engine->EndFrame(); // Allow engine subsystems to do post-frame stuff
}

//-----------------------------------------------------------------------------------------------
void App::Update()
{
	g_engine->m_input->BeginFrame();

	if( g_game->m_currentState == GAME_STATE_ATTRACT || g_engine->m_devConsole->IsOpen() || !g_engine->m_input->IsWindowActive() )
	{
		g_engine->m_input->SetCursorMode( CursorMode::POINTER );
	}
	else
	{
		g_engine->m_input->SetCursorMode( CursorMode::FPS );
	}

	if( m_pauseNextFrame )
	{
		m_isPaused = true;
	}
	UpdateKeyboardInput();

	if( m_isPaused )
	{
		g_game->SetGameMusicSpeed( 0.f );
	}

	if( m_isSlowMo )
	{
		g_game->SetGameMusicSpeed( 0.8f );
	}

	if( m_isFastMo )
	{
		g_game->SetGameMusicSpeed( 1.2f );
	}

	if( !m_isPaused && !m_isSlowMo && !m_isFastMo )
	{
		g_game->SetGameMusicSpeed( 1.f );
	}

	g_game->Update();
}

//-----------------------------------------------------------------------------------------------
void App::Render() const
{
	g_engine->m_render->BeginCamera( *g_game->m_worldCameraP1 );
	
	g_engine->m_render->ClearScreen( g_clearColor );

	g_game->Render();

	g_engine->m_render->EndCamera( *g_game->m_worldCameraP1 );
}

//-----------------------------------------------------------------------------------------------
void App::SetIsQuitting( bool quit )
{
	m_isQuitting = quit;
}

//-----------------------------------------------------------------------------------------------
bool App::IsQuitting() const
{
	return m_isQuitting;
}

//-----------------------------------------------------------------------------------------------
bool App::EventQuit( [[maybe_unused]] EventArgs& args )
{
	g_app->SetIsQuitting( true );
	return false;
}

//-----------------------------------------------------------------------------------------------
void App::UpdateKeyboardInput()
{
	if(g_game->m_currentState == GAME_STATE_ATTRACT && g_engine->m_input->WasKeyJustPressed( ' ' ) )
	{
		g_game->m_nextState = GAME_STATE_LOBBY;
	}

	if( g_game->m_currentState == GAME_STATE_LOBBY && g_engine->m_input->WasKeyJustPressed( ' ' ) )
	{
		g_game->m_nextState = GAME_STATE_PLAY;
	}

	if( g_game->m_currentState == GAME_STATE_PLAY && g_engine->m_input->WasKeyJustPressed( KEYCODE_ESC ) )
	{
		RestartGame();
		g_game->m_nextState = GAME_STATE_ATTRACT;
	}

	if( g_game->m_currentState == GAME_STATE_ATTRACT && g_engine->m_input->WasKeyJustPressed( KEYCODE_ESC ) )
	{
		m_isQuitting = true;
	}

	if( g_game->m_currentState == GAME_STATE_PLAY && g_engine->m_input->WasKeyJustPressed( 'P' ) )
	{
		if( !m_isPaused )
		{
			m_isPaused = true;
			g_game->m_gameClock->Pause();
		}
		else
		{
			m_isPaused = false;
			m_pauseNextFrame = false;
			g_game->m_gameClock->Unpause();
		}
	}

	if( g_game->m_currentState == GAME_STATE_PLAY && g_engine->m_input->WasKeyJustPressed( 'T' ) )
	{
		m_isSlowMo = true;
		g_game->m_gameClock->SetTimeScale( 0.1 );
	}

	if( g_game->m_currentState == GAME_STATE_PLAY && g_engine->m_input->WasKeyJustReleased( 'T' ) )
	{
		m_isSlowMo = false;
		g_game->m_gameClock->SetTimeScale( 1.0 );
	}

	if( g_game->m_currentState == GAME_STATE_PLAY && g_engine->m_input->WasKeyJustPressed( 'Y' ) )
	{
		m_isFastMo = true;
		g_game->m_gameClock->SetTimeScale( 4.0 );
	}

	if( g_game->m_currentState == GAME_STATE_PLAY && g_engine->m_input->WasKeyJustReleased( 'Y' ) )
	{
		m_isFastMo = false;
		g_game->m_gameClock->SetTimeScale( 1.0 );
	}

	if( g_game->m_currentState == GAME_STATE_PLAY && g_engine->m_input->WasKeyJustPressed( 'O' ) )
	{
		if( !m_isPaused )
		{
			m_pauseNextFrame = true;
		}
		else
		{
			m_pauseNextFrame = true;
			m_isPaused = false;
		}
		g_game->m_gameClock->StepSingleFrame();
	}

	if( g_engine->m_input->WasKeyJustPressed( KEYCODE_GRAVE_ACCENT ) )
	{
		g_engine->m_devConsole->SetMode( DevConsoleMode::OPEN_FULL );
	}
}

//-----------------------------------------------------------------------------------------------
void App::RestartGame()
{
	FireEvent( "DebugClear" );

	g_engine->m_render->BindTexture( nullptr );
	g_game->Shutdown();

	delete g_game;
	g_game = nullptr;

	g_game = new Game();
}

