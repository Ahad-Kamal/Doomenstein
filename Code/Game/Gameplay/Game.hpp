#pragma once
#include "Game/Framework/GameCommon.hpp"
#include "Game/Gameplay/Map.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/Vec3.hpp"


//-----------------------------------------------------------------------------------------------
class Game;
class App;
class Camera;
class Entity;
class Clock;
class Player;
class BitmapFont;
class SpriteSheet;
class NamedStrings;

//-----------------------------------------------------------------------------------------------
extern Game* g_game;
extern RandomNumberGenerator* g_rng;
extern SpriteSheet* g_terrainSpriteSheet;
extern BitmapFont* g_bitmapFont;
extern NamedStrings* g_blackboard;


//-----------------------------------------------------------------------------------------------
class Game
{
public:
	Game();
	~Game();
	void Startup();
	void Update();
	void Render() const;
	void Shutdown();

	void SetGameMusicSpeed( float speed );

private:
	void UpdateStates();
	void UpdateAttractMode( float deltaSeconds );
	void RenderAttractMode() const;

	void UpdateEntities( float deltaSeconds );
	void UpdateMap( float deltaSeconds );
	void RenderEntities() const;
	void RenderMap() const;

	void UpdateKeyboardInput();
	void UpdateControllerInput();

	void InitializeStartTriangleVerts();

	void DebugRenderEntities() const;
	void DebugDrawWorldBounds() const;
	void DebugAddDebugText() const;

	void AddCommandsToDevConsole();
	void AddControlsToDevConsole();

public:
	std::vector<Map>	m_maps;
	Map*				m_currentMap;

	Camera*				m_worldCamera;
	Camera*				m_screenCamera;

	Clock*				m_gameClock;

	GameState			m_currentState = GAME_STATE_INVALID;
	GameState			m_nextState = GAME_STATE_ATTRACT;

	Vec3 m_sunDirection = Vec3( 2.f, 1.f, -1.f );
	float m_sunIntensity = 0.85f;
	float m_ambientIntensity = 0.35f;

private:
	Vertex m_startVerts[ 3 ];
	float m_startAlpha = 255.f;
	float m_time = 0.f;
	Rgba8 m_clearColor = Rgba8( 0, 0, 0, 1 );
	SoundPlaybackID m_music;

	float m_screenShakeAmount = 0.f;
	bool m_isShaking = false;
	bool m_debugDraw = false;
};