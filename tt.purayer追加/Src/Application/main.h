#pragma once
class KdgameSystem;
struct FPSController
{
	// FPS制御
	int		m_nowfps = 0;		// 現在のFPS値
	int		m_maxFps = 60;		// 最大FPS

	void Init();

	void UpdateStartTime();

	void Update();

private:

	void Control();

	void Monitoring();

	DWORD		m_frameStartTime = 0;		// フレームの開始時間

	int			m_fpsCnt = 0;				// FPS計測用カウント
	DWORD		m_fpsMonitorBeginTime = 0;	// FPS計測開始時間

	const int	kSecond = 1000;				// １秒のミリ秒
};

//============================================================
// アプリケーションクラス
//	APP.～ でどこからでもアクセス可能
//============================================================
class Application
{
// メンバ
public:

	// アプリケーション実行
	void Execute();

	// アプリケーション終了
	void End() { m_endFlag = true; }

	//=====================================================
	// グローバルデータ
	//=====================================================

	// ゲームウィンドウクラス
	KdWindow		m_window;
	FPSController	m_fpsController;

	KdCamera* Applicat = nullptr;
private:

	// アプリケーション初期化
	bool Init(int w, int h);

	// アプリケーション解放
	void Release();

	bool		m_endFlag = false;		// ゲーム終了フラグ trueで終了
	KdgameSystem* game = nullptr;
//=====================================================
// シングルトンパターン
//=====================================================
private:
	// 
	Application() {}

public:
	static Application &Instance()
	{
		static Application Instance;
		return Instance;
	}
};