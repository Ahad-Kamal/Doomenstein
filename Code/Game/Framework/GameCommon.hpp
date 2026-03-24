#pragma once
#include "Engine/Audio/AudioSystem.hpp"


//-----------------------------------------------------------------------------------------------
struct Vec2;
struct Rgba8;

//-----------------------------------------------------------------------------------------------
enum GameState
{
	GAME_STATE_INVALID = -1,
	GAME_STATE_ATTRACT,
	GAME_STATE_PLAY
};

//-----------------------------------------------------------------------------------------------
constexpr char FUNCTION_KEY_1 = 'p';
constexpr char FUNCTION_KEY_8 = 'w';
constexpr float WORLD_SIZE_X = 2.f;
constexpr float WORLD_SIZE_Y = 2.f;
constexpr float WORLD_SIZE_Z = 2.f;
constexpr float WORLD_VIEW_SIZE_X = 2.f;
constexpr float WORLD_VIEW_SIZE_Y = 2.f;
constexpr float SCREEN_SIZE_X = 1600.f;
constexpr float SCREEN_SIZE_Y = 800.f;
constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;
constexpr float WORLD_CENTER_Z = WORLD_SIZE_Z / 2.f;
constexpr float SCREEN_CENTER_X = SCREEN_SIZE_X / 2.f;
constexpr float SCREEN_CENTER_Y = SCREEN_SIZE_Y / 2.f;
constexpr float DEBUG_LINE_THICKNESS = 0.2f;

//-----------------------------------------------------------------------------------------------
extern SoundID audio_music;
extern SoundID audio_selectSound;
extern SoundID audio_testSound;

//-----------------------------------------------------------------------------------------------
void DebugDrawRing( Vec2 const& center, float radius, float thickness, Rgba8 const& color);
void DebugDrawLine( Vec2 const& startPos, Vec2 const& endPos, float thickness, Rgba8 const& color);
void DrawFadedRing( Vec2 center, float innerRadius, float outerRadius, Rgba8 innerColor, Rgba8 outerColor );
void DrawGlow( Vec2 pos, Rgba8 color, float alpha, float radius );
void CreateSounds();