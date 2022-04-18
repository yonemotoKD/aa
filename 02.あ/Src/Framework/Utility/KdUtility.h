﻿#pragma once

class KdTexture;

//===========================================
//
// 便利機能
//
//===========================================

// 算術系短縮名
namespace Math = DirectX::SimpleMath;

// 角度変換
constexpr float KdToRadians = (3.141592654f / 180.0f);
constexpr float KdToDegrees = (180.0f / 3.141592654f);

// 安全にReleaseするための関数
template<class T>
void KdSafeRelease(T*& p)
{
	if (p)
	{
		p->Release();
		p = nullptr;
	}
}

// 安全にDeleteするための関数
template<class T>
void KdSafeDelete(T*& p)
{
	if (p)
	{
		delete p;
		p = nullptr;
	}
}

template<class T>
void DebugOutputNumber(T num)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << num;
	std::string str = stream.str();

	OutputDebugStringA(str.c_str());
}


//===========================================
//
// 色定数
//
//===========================================
static const Math::Color	kWhiteColor		= Math::Color(1.0f, 1.0f, 1.0f, 1.0f);
static const Math::Color	kBlackColor		= Math::Color(0.0f, 0.0f, 0.0f, 1.0f);
static const Math::Color	kRedColor		= Math::Color(1.0f, 0.0f, 0.0f, 1.0f);
static const Math::Color	kGreenColor		= Math::Color(0.0f, 1.0f, 0.0f, 1.0f);
static const Math::Color	kBlueColor		= Math::Color(0.0f, 0.0f, 1.0f, 1.0f);
static const Math::Color	kNormalColor	= Math::Color(0.5f, 0.5f, 1.0f, 1.0f);	// 垂直に伸びる法線情報


//===========================================
//
// ファイル
//
//===========================================

// ファイルパスから、親ディレクトリまでのパスを取得
inline std::string KdGetDirFromPath(const std::string &path)
{
	const std::string::size_type pos = std::max<signed>(path.find_last_of('/'), path.find_last_of('\\'));
	return (pos == std::string::npos) ? std::string() : path.substr(0, pos + 1);
}

//===========================================
//
// 文字列関係
//
//===========================================

// std::string版 sprintf
template <typename ... Args>
std::string KdFormat(const std::string& fmt, Args ... args)
{
	size_t len = std::snprintf(nullptr, 0, fmt.c_str(), args ...);
	std::vector<char> buf(len + 1);
	std::snprintf(&buf[0], len + 1, fmt.c_str(), args ...);
	return std::string(&buf[0], &buf[0] + len);
}

void KdGetTextureInfo(ID3D11View* view, D3D11_TEXTURE2D_DESC& outDesc);


//===========================================
//
// 変換関係
//
//===========================================
bool ConvertRectToUV(const KdTexture* srcTex, const Math::Rectangle& src, Math::Vector2& uvMin, Math::Vector2& uvMax);

//===========================================
//
// レンダーターゲット変更
//
//===========================================

struct RenderTargetPack
{
	RenderTargetPack() {}

	void CreateRenderTarget(int width, int height, bool needDSV = false, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM,  D3D11_VIEWPORT* pVP = nullptr);
	void SetRenderTarget(std::shared_ptr<KdTexture> RTT, std::shared_ptr<KdTexture> DST = nullptr, D3D11_VIEWPORT* pVP = nullptr);

	void SetViewPort(D3D11_VIEWPORT* pVP);

	void ClearTexture(const Math::Color& fillColor = kBlueColor);

	std::shared_ptr<KdTexture> m_RTTexture = nullptr;
	std::shared_ptr<KdTexture> m_ZBuffer = nullptr;
	D3D11_VIEWPORT m_viewPort = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
};

struct RenderTargetChanger
{
	~RenderTargetChanger() { Release(); }

	ID3D11RenderTargetView* m_saveRTV = nullptr;
	ID3D11DepthStencilView* m_saveDSV = nullptr;
	D3D11_VIEWPORT			m_saveVP = {};
	UINT					m_numVP = 1;
	bool					m_changeVP = false;

	bool Validate(ID3D11RenderTargetView* pRTV);

	bool ChangeRenderTarget(ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV = nullptr, D3D11_VIEWPORT* pVP = nullptr);
	bool ChangeRenderTarget(std::shared_ptr<KdTexture> RTT, std::shared_ptr<KdTexture> DST = nullptr, D3D11_VIEWPORT* pVP = nullptr);
	bool ChangeRenderTarget(RenderTargetPack& RTPack);

	void UndoRenderTarget();

	void Release();
};