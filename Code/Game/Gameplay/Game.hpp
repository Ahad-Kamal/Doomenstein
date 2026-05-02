#pragma once
#include "Game/Framework/GameCommon.hpp"
#include "Game/Gameplay/Map.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB2.hpp"


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
	bool IsTwoPlayer() const;

private:
	void UpdateStates();

	void UpdateAttractMode( float deltaSeconds );
	void RenderAttractMode() const;

	void UpdateLobbyMode();
	void RenderLobbyMode() const;

	void UpdateEntities( float deltaSeconds );
	void RenderEntities() const;

	void UpdateMap( float deltaSeconds );
	void RenderMap() const;

	void RenderHud( Player* player, Camera* screenCamera ) const;

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

	Camera*				m_worldCameraP1 = nullptr;
	Camera*				m_worldCameraP2 = nullptr;
	Camera*				m_screenCameraP1 = nullptr;
	Camera*				m_screenCameraP2 = nullptr;
	Camera*				m_screenCameraFull = nullptr;

	Clock*				m_gameClock;

	GameState			m_currentState = GAME_STATE_INVALID;
	GameState			m_nextState = GAME_STATE_ATTRACT;

	Vec3 m_sunDirection = Vec3( 2.f, 1.f, -1.f );
	float m_sunIntensity = 0.85f;
	float m_ambientIntensity = 0.35f;

private:
	float m_time = 0.f;
	Rgba8 m_clearColor = Rgba8( 0, 0, 0, 1 );
	SoundPlaybackID m_music;

	AABB2 m_player1CameraBounds = AABB2( 0.f, 0.f, SCREEN_SIZE_X, SCREEN_SIZE_Y );
	AABB2 m_player2CameraBounds = AABB2();

	float m_screenShakeAmount = 0.f;
	bool m_isShaking = false;
	bool m_debugDraw = false;
	bool m_isTwoPlayer = false;

	float m_startAlpha = 255.f;
	Vertex m_startVerts[ 3 ];
};