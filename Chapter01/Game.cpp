// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

const int thickness = 15;		//壁の厚さ
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
	mBallVel.x = -200.0f;		// ボール速度
	mBallVel.y = 235.0f;		// ボール速度
	mBallVel.x = 0.0f;		// ボール速度
	mBallVel.y = 0.0f;		// ボール速度
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
	// エスケープが押されればゲームを終了
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	
	// W/S keysが押されればパドルの向き更新
	mPaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleDir += 1;
	}
	// I/K keysが押されればパドルの向き更新
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
	// 最後のフレーム更新から必ず16ms待つ
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta timeは最後のフレームからの実時間差
	// (秒数への変換)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// Delta timeの最大値を制限
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// 実時間の更新 (次のフレームで参照するための)
	mTicksCount = SDL_GetTicks();
	
	// パドルの方向に基づいてパドルの位置を更新
	if (mPaddleDir != 0|| mPaddleDir2 != 0)
	{
		if(mPaddleDir  !=0)	mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
		if(mPaddleDir2 !=0)	mPaddlePos2.y += mPaddleDir2 * 300.0f * deltaTime;
		// パドルが画面外へ行かないための処理
		if (mPaddlePos.y < (paddleH/2.0f + thickness))
		{
			mPaddlePos.y = paddleH/2.0f + thickness;
		}
		else if (mPaddlePos.y > (680.0f - paddleH/2.0f - thickness))
		{
			mPaddlePos.y = 680.0f - paddleH/2.0f - thickness;
		}
		// 2Pのパドルが画面外へ行かないための処理
		if (mPaddlePos2.y < (paddleH / 2.0f + thickness))
		{
			mPaddlePos2.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddlePos2.y > (680.0f - paddleH / 2.0f - thickness))
		{
			mPaddlePos2.y = 680.0f - paddleH / 2.0f - thickness;
		}
	}

	// ballの速度に基づいてポジションを更新
	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;
	
	// 跳ね返る条件であれば
	// パドルとボールのポジションの差を求めて
	float diff = mPaddlePos.y - mBallPos.y;
	// 差を絶対値に変換
	diff = (diff > 0.0f) ? diff : -diff;
	if (
		// Yの差が十分に小さいかつ
		diff <= paddleH / 2.0f &&
		// Xのポジションがそれらしい位置に来ているかつ
		mBallPos.x <= 25.0f && mBallPos.x >= 20.0f &&
		// ボールの向きが左向きであれば
		mBallVel.x < 0.0f)
	{
		mBallVel.x *= -1.0f;
	}
	// ボールが画面の外へ出たか
	else if (mBallPos.x <= 0.0f||mBallPos.x>=1024)
	{
		mIsRunning = false;
	}
	// ボールが右の壁と衝突したか
	else if (mBallPos.x >= (1024.0f - thickness) && mBallVel.x > 0.0f)
	{
		mBallVel.x *= -1.0f;
	}
	
	// ボールが上の壁と衝突したか
	if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
	{
		mBallVel.y *= -1;
	}
	// ボールが下の壁と衝突したか
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
	
	// バックバッファをクリアする
	SDL_RenderClear(mRenderer);

	// 背景の描画
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	
	// 上の壁の描画
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);
	
	// 下の壁の描画
	wall.y = 680 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);
	
	// 右の壁の描画
	//wall.x = 1024 - thickness;
	//wall.y = 0;
	//wall.w = thickness;
	//wall.h = 1024;
	//SDL_RenderFillRect(mRenderer, &wall);
	
	// パドルの描画
	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x),
		static_cast<int>(mPaddlePos.y - paddleH/2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle);
	//2Pパドルの描写
	paddle.x = static_cast<int>(mPaddlePos2.x);
	paddle.y = static_cast<int>(mPaddlePos2.y - paddleH / 2);
	SDL_RenderFillRect(mRenderer, &paddle);
	
	// ボールの描画
	SDL_Rect ball{	
		static_cast<int>(mBallPos.x - thickness/2),
		static_cast<int>(mBallPos.y - thickness/2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &ball);
	
	// フロントバッファとバックバッファを交換
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}
