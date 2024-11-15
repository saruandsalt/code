#pragma once
#include "SDL/SDL.h"
class my_Game {
public:
	my_Game();
	bool Initiarize();
	void RunLoop();
	void Shutdown();
private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	SDL_Window* mWindow;
	bool mIsRunnning
};