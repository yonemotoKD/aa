﻿#include "main.h"

//===================================================================
// メイン
//===================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszArgs, int nWinMode)
{
	// メモリリークを知らせる
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// COM初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	// mbstowcs_s関数で日本語対応にするために呼ぶ
	setlocale(LC_ALL, "japanese");

	//===================================================================
	// 実行
	//===================================================================
	Application::Instance().Execute();

	// COM解放
	CoUninitialize();

	return 0;
}


// アプリケーション初期設定
bool Application::Init(int w, int h)
{
	//===================================================================
	// ウィンドウ作成
	//===================================================================
	if (m_window.Create(w, h, "3D GameProgramming", "Window") == false) {
		MessageBoxA(nullptr, "ウィンドウ作成に失敗", "エラー", MB_OK);
		return false;
	}

	//===================================================================
	// フルスクリーン確認
	//===================================================================
	bool bFullScreen = false;
	if (MessageBoxA(m_window.GetWndHandle(), "フルスクリーンにしますか？", "確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) {
		bFullScreen = true;
	}

	//===================================================================
	// Direct3D初期化
	//===================================================================

	// デバイスのデバッグモードを有効にする
	bool deviceDebugMode = false;
	#ifdef _DEBUG
	deviceDebugMode = true;
	#endif

	// Direct3D初期化
	std::string errorMsg;
	if (D3D.Init(m_window.GetWndHandle(), w, h, deviceDebugMode, errorMsg) == false) {
		MessageBoxA(m_window.GetWndHandle(), errorMsg.c_str(), "Direct3D初期化失敗", MB_OK | MB_ICONSTOP);
		return false;
	}

	// フルスクリーン設定
	if (bFullScreen) {
		D3D.WorkSwapChain()->SetFullscreenState(TRUE, 0);
	}

	//===================================================================
	// オーディオ初期化
	//===================================================================

	return true;
}

// アプリケーション終了
void Application::Release()
{
	// Direct3D解放
	D3D.Release();

	// ウィンドウ削除
	m_window.Release();
}

// アプリケーション実行
void Application::Execute()
{
	//===================================================================
	// 初期設定(ウィンドウ作成、Direct3D初期化など)
	//===================================================================
	if (!Application::Instance().Init(1280, 720))
	{
		return;
	}

	//===================================================================
	// ゲームループ
	//===================================================================

	// 時間
	

	// 初期化

	// ループ
	while (1)
	{
		// 処理開始時間Get
		//フレーム制御
		m_fpsController.UpdateStartTime();

		// ゲーム終了指定があるときはループ終了
		if (m_endFlag)
		{ 
			break;
		}

		//=========================================
		//
		// ウィンドウ関係の処理
		//
		//=========================================

		// ウィンドウのメッセージを処理する
		m_window.ProcessMessage();

		// ウィンドウが破棄されてるならループ終了
		if (m_window.IsCreated() == false)
		{
			break;
		}

		//=========================================
		//
		// ゲーム処理
		//
		//=========================================

		// ゲーム更新

		// ゲーム描画
		Math::Color col(0.5f,0.0f,1.0f,1.0f);

		D3D.WorkDevContext()->ClearRenderTargetView(D3D.GetBackBuffer()->WorkRTView(), col);
		
		D3D.WorkDevContext()->ClearDepthStencilView(D3D.WorkZBuffer()->WorkDSView(),D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,1.0f,0);
		// BackBuffer -> 画面表示
		D3D.WorkSwapChain()->Present(0, 0);

		//=========================================
		//
		// フレームレート制御・計測
		//
		//=========================================
		m_fpsController.Update();
	}

	//===================================================================
	// アプリケーション解放
	//===================================================================
	Release();
}

void FPSController::Init()
{
	m_fpsMonitorBeginTime = timeGetTime();
}

void FPSController::UpdateStartTime()
{
	m_frameStartTime = timeGetTime();
}

void FPSController::Update()
{
	Control();

	Monitoring();
}

// FPS制御
void FPSController::Control()
{
	// 処理終了時間Get
	DWORD frameProcessEndTime = timeGetTime();

	// 1フレームで経過すべき時間
	DWORD timePerFrame = kSecond / m_maxFps;

	if (frameProcessEndTime - m_frameStartTime < timePerFrame)
	{
		// 1秒間にMaxFPS回数以上処理が回らないように待機する
		Sleep(timePerFrame - (frameProcessEndTime - m_frameStartTime));
	}
}

// 現在のFPS計測
void FPSController::Monitoring()
{
	constexpr float kFpsRefreshFrame = 500;		// FPS計測のタイミング　0.5秒おき

	m_fpsCnt++;

	// 0.5秒おきに FPS計測
	if (m_frameStartTime - m_fpsMonitorBeginTime >= kFpsRefreshFrame)
	{
		// 現在のFPS算出
		m_nowfps = (m_fpsCnt * kSecond) / (m_frameStartTime - m_fpsMonitorBeginTime);

		m_fpsMonitorBeginTime = m_frameStartTime;
		
		m_fpsCnt = 0;
	}
}
