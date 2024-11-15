// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

const int thickness = 15;		//�ǂ̌���
const float paddleH = 100.0f;

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mTicksCount(0)
,mIsRunning(true)
,mPaddleDir(0)
,mPaddleDir2(0)
{
	
}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	
	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Chapter 1)", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		680,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	//
	mPaddlePos.x = 10.0f;
	mPaddlePos.y = 768.0f/2.0f;
	mPaddlePos2.x = 1014.0f-thickness;
	mPaddlePos2.y = 768.0f / 2.0f;
	mBallPos.x = 1024.0f/2.0f;
	mBallPos.y = 680.0f/2.0f;
	mBallVel.x = -200.0f;		// �{�[�����x
	mBallVel.y = 235.0f;		// �{�[�����x
	mBallVel.x = 0.0f;		// �{�[�����x
	mBallVel.y = 0.0f;		// �{�[�����x
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// �G�X�P�[�v���������΃Q�[�����I��
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	
	// W/S keys���������΃p�h���̌����X�V
	mPaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleDir += 1;
	}
	// I/K keys���������΃p�h���̌����X�V
	mPaddleDir2 = 0;
	if (state[SDL_SCANCODE_I])
	{
		mPaddleDir2 -= 1;
	}
	if (state[SDL_SCANCODE_K])
	{
		mPaddleDir2 += 1;
	}
}

void Game::UpdateGame()
{
	// �Ō�̃t���[���X�V����K��16ms�҂�
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time�͍Ō�̃t���[������̎����ԍ�
	// (�b���ւ̕ϊ�)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// Delta time�̍ő�l�𐧌�
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// �����Ԃ̍X�V (���̃t���[���ŎQ�Ƃ��邽�߂�)
	mTicksCount = SDL_GetTicks();
	
	// �p�h���̕����Ɋ�Â��ăp�h���̈ʒu���X�V
	if (mPaddleDir != 0|| mPaddleDir2 != 0)
	{
		if(mPaddleDir  !=0)	mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
		if(mPaddleDir2 !=0)	mPaddlePos2.y += mPaddleDir2 * 300.0f * deltaTime;
		// �p�h������ʊO�֍s���Ȃ����߂̏���
		if (mPaddlePos.y < (paddleH/2.0f + thickness))
		{
			mPaddlePos.y = paddleH/2.0f + thickness;
		}
		else if (mPaddlePos.y > (680.0f - paddleH/2.0f - thickness))
		{
			mPaddlePos.y = 680.0f - paddleH/2.0f - thickness;
		}
		// 2P�̃p�h������ʊO�֍s���Ȃ����߂̏���
		if (mPaddlePos2.y < (paddleH / 2.0f + thickness))
		{
			mPaddlePos2.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddlePos2.y > (680.0f - paddleH / 2.0f - thickness))
		{
			mPaddlePos2.y = 680.0f - paddleH / 2.0f - thickness;
		}
	}

	// ball�̑��x�Ɋ�Â��ă|�W�V�������X�V
	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;
	
	// ���˕Ԃ�����ł����
	// �p�h���ƃ{�[���̃|�W�V�����̍������߂�
	float diff = mPaddlePos.y - mBallPos.y;
	// �����Βl�ɕϊ�
	diff = (diff > 0.0f) ? diff : -diff;
	if (
		// Y�̍����\���ɏ���������
		diff <= paddleH / 2.0f &&
		// X�̃|�W�V����������炵���ʒu�ɗ��Ă��邩��
		mBallPos.x <= 25.0f && mBallPos.x >= 20.0f &&
		// �{�[���̌������������ł����
		mBallVel.x < 0.0f)
	{
		mBallVel.x *= -1.0f;
	}
	// �{�[������ʂ̊O�֏o����
	else if (mBallPos.x <= 0.0f||mBallPos.x>=1024)
	{
		mIsRunning = false;
	}
	// �{�[�����E�̕ǂƏՓ˂�����
	else if (mBallPos.x >= (1024.0f - thickness) && mBallVel.x > 0.0f)
	{
		mBallVel.x *= -1.0f;
	}
	
	// �{�[������̕ǂƏՓ˂�����
	if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
	{
		mBallVel.y *= -1;
	}
	// �{�[�������̕ǂƏՓ˂�����
	else if (mBallPos.y >= (680 - thickness) &&
		mBallVel.y > 0.0f)
	{
		mBallVel.y *= -1;
	}
}

void Game::GenerateOutput()
{
	// Set draw color to blue
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		255,	// B
		255		// A
	);
	
	// �o�b�N�o�b�t�@���N���A����
	SDL_RenderClear(mRenderer);

	// �w�i�̕`��
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	
	// ��̕ǂ̕`��
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);
	
	// ���̕ǂ̕`��
	wall.y = 680 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);
	
	// �E�̕ǂ̕`��
	//wall.x = 1024 - thickness;
	//wall.y = 0;
	//wall.w = thickness;
	//wall.h = 1024;
	//SDL_RenderFillRect(mRenderer, &wall);
	
	// �p�h���̕`��
	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x),
		static_cast<int>(mPaddlePos.y - paddleH/2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle);
	//2P�p�h���̕`��
	paddle.x = static_cast<int>(mPaddlePos2.x);
	paddle.y = static_cast<int>(mPaddlePos2.y - paddleH / 2);
	SDL_RenderFillRect(mRenderer, &paddle);
	
	// �{�[���̕`��
	SDL_Rect ball{	
		static_cast<int>(mBallPos.x - thickness/2),
		static_cast<int>(mBallPos.y - thickness/2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &ball);
	
	// �t�����g�o�b�t�@�ƃo�b�N�o�b�t�@������
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}
