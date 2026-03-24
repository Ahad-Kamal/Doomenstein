#pragma once
#include "Engine/Core/DebugRender.hpp"


//-----------------------------------------------------------------------------------------------
class App;
class Game;
extern App* g_app;
class NamedStrings;

//-----------------------------------------------------------------------------------------------
typedef NamedStrings EventArgs;

//-----------------------------------------------------------------------------------------------
class App
{
public:
	App();
	~App();

	void RunMainLoop();
	void RunFrame();
	void Update();
	void Render() const;

	void SetIsQuitting( bool quit );
	bool IsQuitting() const;

	static bool EventQuit( EventArgs& args );

	void UpdateKeyboardInput();
	void RestartGame();

public:
	DebugRenderConfig m_debugConfig;

	float m_lastFrameTime = 0.f;
	bool m_isQuitting = false;
	bool m_isPaused = false;
	bool m_pauseNextFrame = false;
	bool m_isSlowMo = false;
	bool m_isFastMo = false;
	bool m_pauseAfterNextUpdate = false;
};