#ifndef ENGINE_H
#define ENHINE_H
#include <Windows.h>
#include "SceneManager.h"

class Engine
{
public:
	Engine();
	~Engine();
	void Init();
	void Update();
	void Draw();
	
	const SceneManager* GetSceneManager() const;
private:
	void CalculateTime();
	void ClearScreen();
	void InitGraphics();
	SceneManager* m_SceneManager;
	
	DWORD m_StartTime;
	DWORD m_RunningTimeInMiliSec;
	DWORD m_RunningTimeInSec;
	DWORD m_LastFrameTime;
	double m_dDeltaTime;

};

#endif