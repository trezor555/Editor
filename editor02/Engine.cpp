#include "Engine.h"
#include <time.h> 


Engine::Engine()
{
	m_SceneManager = new SceneManager();
}

void Engine::Init()
{
	m_SceneManager->Init();
	m_StartTime = timeGetTime();
	m_LastFrameTime = timeGetTime();

	InitGraphics();
}

void Engine::Update()
{
	CalculateTime();
}

void Engine::CalculateTime()
{
	m_RunningTimeInMiliSec = timeGetTime() - m_StartTime;
	m_RunningTimeInSec = m_RunningTimeInMiliSec / 1000;

	m_dDeltaTime = (timeGetTime() - m_LastFrameTime)/1000.0f;
	m_LastFrameTime = timeGetTime();
}

Engine::~Engine()
{
	m_SceneManager->ClearScene();
	delete m_SceneManager;
}

void Engine::Draw()
{
	ClearScreen();
}

void Engine::ClearScreen()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Engine::InitGraphics()
{
	/*if (glewInit() != GLEW_OK)
		return;*/

	glViewport(0, 0, 800, 600);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CLIP_PLANE0);
	glClearColor(0.2f, 0.8f, 0.3f, 1.0f);
}

const SceneManager* Engine::GetSceneManager() const
{
	return m_SceneManager;
}
