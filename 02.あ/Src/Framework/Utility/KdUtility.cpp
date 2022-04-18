#include "Framework/KdFramework.h"

#include "KdUtility.h"

// viewから画像情報を取得する
void KdGetTextureInfo(ID3D11View* view, D3D11_TEXTURE2D_DESC& outDesc)
{
	outDesc = {};

	ID3D11Resource* res;
	view->GetResource(&res);

	ID3D11Texture2D* tex2D;
	if (SUCCEEDED(res->QueryInterface<ID3D11Texture2D>(&tex2D)))
	{
		tex2D->GetDesc(&outDesc);
		tex2D->Release();
	}
	res->Release();
}

bool ConvertRectToUV(const KdTexture* srcTex, const Math::Rectangle& src, Math::Vector2& uvMin, Math::Vector2& uvMax)
{
	if (!srcTex) { return false; }

	uvMin.x = src.x / (float)srcTex->GetInfo().Width;
	uvMin.y = src.y / (float)srcTex->GetInfo().Height;

	uvMax.x = ( src.width  / (float)srcTex->GetInfo().Width) + uvMin.x;
	uvMax.y = ( src.height / (float)srcTex->GetInfo().Height) + uvMin.y;

	return true;
}


//===========================================
//
// レンダーターゲットのパッケージ　RTView-DSView-ViewPort
//
//===========================================

void RenderTargetPack::CreateRenderTarget(int width, int height, bool needDSV, DXGI_FORMAT format, D3D11_VIEWPORT* pVP)
{
	m_RTTexture = std::make_shared<KdTexture>();
	m_RTTexture->CreateRenderTarget(width, height, format);

	if (needDSV)
	{
		m_ZBuffer = std::make_shared<KdTexture>();
		m_ZBuffer->CreateDepthStencil(width, height);
	}

	SetViewPort(pVP);
}

void RenderTargetPack::SetRenderTarget(std::shared_ptr<KdTexture> RTT, std::shared_ptr<KdTexture> DST, D3D11_VIEWPORT* pVP)
{
	m_RTTexture = RTT;
	m_ZBuffer = DST;

	SetViewPort(pVP);
}

void RenderTargetPack::SetViewPort(D3D11_VIEWPORT* pVP)
{
	if (pVP)
	{
		m_viewPort = *pVP;
	}
	else
	{
		if (!m_RTTexture) { return; }

		// レンダーターゲットテクスチャから生成
		m_viewPort =
		{
			0.0f,
			0.0f,
			static_cast<float>(m_RTTexture->GetWidth()),
			static_cast<float>(m_RTTexture->GetHeight()),
			0.0f,
			1.0f
		};
	}
}

void RenderTargetPack::ClearTexture(const Math::Color& fillColor)
{
	ID3D11DeviceContext* DevCon = D3D.WorkDevContext();

	if (m_RTTexture)
	{
		// テクスチャの描画クリア
		DevCon->ClearRenderTargetView(m_RTTexture->WorkRTView(), fillColor);
	}

	if (m_ZBuffer)
	{
		DevCon->ClearDepthStencilView(m_ZBuffer->WorkDSView(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}


//===========================================
//
// レンダーターゲット変更クラス
//
//===========================================

bool RenderTargetChanger::Validate(ID3D11RenderTargetView* pRTV)
{
	if (!pRTV)
	{
		assert(0 && "変更先のRenderTargetがありません");

		return false;
	}

	if (m_saveRTV)
	{
		assert(0 && "既にRenderTargetを変更済みです");

		return false;
	}

	return true;
}

bool RenderTargetChanger::ChangeRenderTarget(ID3D11RenderTargetView* pRTV, 
	ID3D11DepthStencilView* pDSV, D3D11_VIEWPORT* pVP)
{
	if (!Validate(pRTV)) { return false; }

	ID3D11DeviceContext* DevCon = D3D.WorkDevContext();

	if (!m_saveRTV)
	{
		// 情報保存
		DevCon->OMGetRenderTargets(1, &m_saveRTV, &m_saveDSV);
	}

	// レンダーターゲット切替 ----- ----- ----- ----- -----
	DevCon->OMSetRenderTargets(1, &pRTV, pDSV);

	if (pVP)
	{
		// 情報保存
		DevCon->RSGetViewports(&m_numVP, &m_saveVP);

		DevCon->RSSetViewports(1, pVP);

		m_changeVP = true;
	}

	return true;
}

bool RenderTargetChanger::ChangeRenderTarget(std::shared_ptr<KdTexture> RTT, 
	std::shared_ptr<KdTexture> DST, D3D11_VIEWPORT* pVP)
{
	if (!RTT) { return false; }

	ID3D11DepthStencilView* pDSV = nullptr;

	if (DST)
	{
		pDSV = DST->WorkDSView();
	}

	return ChangeRenderTarget(RTT->WorkRTView(), pDSV, pVP);
}

bool RenderTargetChanger::ChangeRenderTarget(RenderTargetPack& RTPack)
{
	return ChangeRenderTarget(RTPack.m_RTTexture, RTPack.m_ZBuffer, &RTPack.m_viewPort);
}

void RenderTargetChanger::UndoRenderTarget()
{
	// 復帰すべきレンダーターゲットが存在しない
	if (!m_saveRTV) { return; }

	D3D.WorkDevContext()->OMSetRenderTargets(1, &m_saveRTV, m_saveDSV);

	if (m_changeVP)
	{
		D3D.WorkDevContext()->RSSetViewports(1, &m_saveVP);
	}

	KdSafeRelease(m_saveRTV);
	KdSafeRelease(m_saveDSV);

	m_changeVP = false;
}

void RenderTargetChanger::Release()
{
	KdSafeRelease(m_saveRTV);
	KdSafeRelease(m_saveDSV);
}
