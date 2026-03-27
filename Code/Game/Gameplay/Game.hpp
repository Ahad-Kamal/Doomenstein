#pragma once
#include "Game/Framework/GameCommon.hpp"
#include "Game/Gameplay/Map.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/Vertex.hpp"


//-----------------------------------------------------------------------------------------------
class Game;
class App;
class Camera;
class Entity;
class Clock;
class Player;
class Prop;
class BitmapFont;
class BillboardText;
class SpriteSheet;

//-----------------------------------------------------------------------------------------------
extern Game* g_game;
extern RandomNumberGenerator* g_rng;
extern SpriteSheet* g_terrainSpriteSheet;
extern BitmapFont* g_bitmapFont;

//-----------------------------------------------------------------------------------------------
constexpr float MAX_SCREEN_SHAKE_AMOUNT = 5.f;
constexpr float SCREEN_SHAKE_REDUCTION = 5.f;

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

	void AddCameraShake( float shakeAmount );

	void SetGameMusicSpeed( float speed );

private:
	void UpdateStates();
	void UpdateAttractMode( float deltaSeconds );
	void RenderAttractMode() const;

	void UpdateEntities( float deltaSeconds );
	void UpdateMap();
	void RenderEntities() const;
	void RenderMap() const;

	void ClampCamera( Vec2& minView, Vec2& maxView );
	void ShakeCamera( float deltaSeconds );

	//bool DoEntitiesOverlap( Entity const& a, Entity const& b);

	void UpdateKeyboardInput();
	void UpdateControllerInput();

	void InitializeStartTriangleVerts();

	void AddDebugObjects();
	void DebugRenderEntities() const;
	void DebugDrawWorldBounds() const;
	void DebugAddDebugText() const;

	void DeleteGarbageEntities();

	void AddCommandsToDevConsole();
	void AddControlsToDevConsole();

	void CreateCube( Prop* cube );
	void CreateGrid();

public:
	std::vector<Map>	m_maps;
	Map*				m_currentMap;

	Player*				m_player = nullptr;
	Prop*				m_cube1 = nullptr;
	Prop*				m_cube2 = nullptr;
	Prop*				m_sphere = nullptr;
	Prop*				m_grid = nullptr;

	Camera*				m_worldCamera;
	Camera*				m_screenCamera;

	Clock*				m_gameClock;

	GameState			m_currentState = GAME_STATE_INVALID;
	GameState			m_nextState = GAME_STATE_ATTRACT;


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