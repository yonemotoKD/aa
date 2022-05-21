#include "KdPostProcessShader.h"

bool KdPostProcessShader::Init()
{
	// VS と InputLayout作成
	{
#include "KdPostProcessShader_VS.inc"

		if (FAILED(D3D.WorkDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) {
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}

		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		if (FAILED(D3D.WorkDev()->CreateInputLayout(
			&layout[0], layout.size(), compiledBuffer,
			sizeof(compiledBuffer), &m_inputLayout)) ) 
		{
			assert(0 && "CreateInputLayout失敗");
			Release();
			return false;
		}
	}

	// PS 作成
	{
#include "KdPostProcessShader_PS_Blur.inc"

		if (FAILED(D3D.WorkDev()->CreatePixelShader(
			compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS_Blur))) 
		{
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			
			return false;
		}

	}

	{
#include "KdPostProcessShader_PS_DoF.inc"

		if (FAILED(D3D.WorkDev()->CreatePixelShader(
			compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS_DoF))) 
		{
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();

			return false;
		}
	}

	{
#include "KdPostProcessShader_PS_Bright.inc"

		if (FAILED(D3D.WorkDev()->CreatePixelShader(
			compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS_Bright))) 
		{
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();

			return false;
		}
	}

	m_cb0_BlurInfo.Create();

	m_cb0_DoFInfo.Create();

	m_cb0_BrightInfo.Create();

	const std::shared_ptr<KdTexture>& backBuffer = D3D.GetBackBuffer();
	
	// ポストプロセス用のシーンの全描画用画像
	m_postEffectRTPack.CreateRenderTarget(backBuffer->GetWidth(), backBuffer->GetHeight(), true);

	// ぼかし画像
	m_blurRTPack.CreateRenderTarget(backBuffer->GetWidth(), backBuffer->GetHeight());
	m_strongBlurRTPack.CreateRenderTarget(backBuffer->GetWidth() / 2, backBuffer->GetHeight() / 2);

	// 被写界深度画像
	m_depthOfFieldRTPack.CreateRenderTarget(backBuffer->GetWidth(), backBuffer->GetHeight());
	
	m_brightEffectRTPack.CreateRenderTarget(backBuffer->GetWidth(), backBuffer->GetHeight());

	int lightBloomWidth = m_brightEffectRTPack.m_RTTexture->GetWidth();
	int lightBloomHeight = m_brightEffectRTPack.m_RTTexture->GetHeight();

	// 光源ぼかし画像
	for (int i = 0; i < kLightBloomNum; ++i)
	{
		m_lightBloomRTPack[i].CreateRenderTarget(lightBloomWidth, lightBloomHeight);

		lightBloomWidth /= 2;
		lightBloomHeight /= 2;
	}

	// 画面全体に書き込む用の頂点情報
	m_screenVert[0] = { {-1,-1,0}, {0, 1} };
	m_screenVert[1] = { {-1, 1,0}, {0, 0} };
	m_screenVert[2] = { { 1,-1,0}, {1, 1} };
	m_screenVert[3] = { { 1, 1,0}, {1, 0} };

	SetBrightThreshold( 0.6f );

	return true;
}

void KdPostProcessShader::Release()
{
	KdSafeRelease(m_VS);

	KdSafeRelease(m_inputLayout);

	KdSafeRelease(m_PS_Blur);
	KdSafeRelease(m_PS_DoF);
	KdSafeRelease(m_PS_Bright);

	m_cb0_BlurInfo.Release();
	m_cb0_DoFInfo.Release();
	m_cb0_BrightInfo.Release();
}

void KdPostProcessShader::BeginDrawPostProcess()
{
	// ポストエフェクトテクスチャの描画クリア
	m_postEffectRTPack.ClearTexture();

	// レンダーターゲット変更
	if (!m_postEffectRTChanger.ChangeRenderTarget(m_postEffectRTPack))
	{
		// 失敗したらUndo
		m_postEffectRTChanger.UndoRenderTarget();
	}
}

void KdPostProcessShader::EndPostProcess()
{

	m_postEffectRTChanger.UndoRenderTarget();

	LightBloomProcess();
	BlurProcess();
	DepthOfFieldProcess();

	D3D.WorkShaderManager().m_spriteShader.DrawTex(m_depthOfFieldRTPack.m_RTTexture.get(), 0, 0);
}

void KdPostProcessShader::BeginDrawBrightEffect()
{
	// 光源描画テクスチャの描画クリア
	m_brightEffectRTPack.ClearTexture(kBlackColor);

	if (!m_brightRTChanger.ChangeRenderTarget(m_brightEffectRTPack.m_RTTexture, m_postEffectRTPack.m_ZBuffer, &m_brightEffectRTPack.m_viewPort))
	{
		m_brightRTChanger.UndoRenderTarget();
	}

	D3D.WorkShaderManager().ChangeBlendState(D3D.WorkShaderManager().m_bs_Add);

	D3D.WorkShaderManager().ChangeDepthStencilState(D3D.WorkShaderManager().m_ds_ZEnable_ZWriteDisable);
}

void KdPostProcessShader::EndDrawBrightEffect()
{
	D3D.WorkShaderManager().UndoDepthStencilState();

	D3D.WorkShaderManager().UndoBlendState();

	m_brightRTChanger.UndoRenderTarget();
}

void KdPostProcessShader::LightBloomProcess()
{
	SetBrightToDevice();

	D3D.WorkShaderManager().ChangeBlendState(D3D.WorkShaderManager().m_bs_Add);

	// 高輝度抽出
	DrawTexture(&m_postEffectRTPack.m_RTTexture, 1, m_brightEffectRTPack.m_RTTexture, m_brightEffectRTPack.m_viewPort);

	D3D.WorkShaderManager().UndoBlendState();

	// LightBloom画像の作成
	SetBlurToDevice();

	std::shared_ptr<KdTexture> srcRTTex = m_brightEffectRTPack.m_RTTexture;

	for (int i = 0; i < kLightBloomNum; ++i)
	{
		GenerateBlurTexture(srcRTTex, m_lightBloomRTPack[i].m_RTTexture, m_lightBloomRTPack[i].m_viewPort, kBlurSamplingRadius);
			
		srcRTTex = m_lightBloomRTPack[i].m_RTTexture;
	}

	RenderTargetChanger RTChanger;
	RTChanger.ChangeRenderTarget(m_postEffectRTPack);

	D3D.WorkShaderManager().ChangeBlendState(D3D.WorkShaderManager().m_bs_Add);

	// 光源ぼかし画像の合成
	for (int i = 0; i < kLightBloomNum; ++i)
	{
		D3D.WorkShaderManager().m_spriteShader.DrawTex(m_lightBloomRTPack[i].m_RTTexture.get(), 0, 0, m_postEffectRTPack.m_RTTexture->GetWidth(), m_postEffectRTPack.m_RTTexture->GetHeight());
	}

	RTChanger.UndoRenderTarget();

	D3D.WorkShaderManager().UndoBlendState();
}

void KdPostProcessShader::BlurProcess()
{
	SetBlurToDevice();

	GenerateBlurTexture(m_postEffectRTPack.m_RTTexture, m_blurRTPack.m_RTTexture, m_blurRTPack.m_viewPort, kBlurSamplingRadius);

	GenerateBlurTexture(m_blurRTPack.m_RTTexture, m_strongBlurRTPack.m_RTTexture, m_strongBlurRTPack.m_viewPort, kBlurSamplingRadius);
}

void KdPostProcessShader::DepthOfFieldProcess()
{
	SetDoFToDevice();

	ID3D11DeviceContext* pDevCon = D3D.WorkDevContext();

	std::shared_ptr<KdTexture> srcTexList[4] =
	{
		m_postEffectRTPack.m_RTTexture,
		m_blurRTPack.m_RTTexture,
		m_strongBlurRTPack.m_RTTexture,
		m_postEffectRTPack.m_ZBuffer
	};

	DrawTexture(srcTexList, 4, m_depthOfFieldRTPack.m_RTTexture, m_depthOfFieldRTPack.m_viewPort);
}

void KdPostProcessShader::CreateBlurOffsetList(std::vector<Math::Vector3>& dstInfo, const std::shared_ptr<KdTexture>& spSrcTex, int samplingRadius, const Math::Vector2& dir)
{
	Math::Vector2 blurDir = dir;
	blurDir.Normalize();

	// 両サイドのサンプリング回数 ＋ サンプル開始中央のピクセル
	int totalSamplingNum = samplingRadius * 2 + 1;

	// サンプリングするテクセルのオフセット値
	Math::Vector2 texelSize;
	texelSize.x = 1.0f / spSrcTex->GetWidth();
	texelSize.y = 1.0f / spSrcTex->GetHeight();

	dstInfo.resize(totalSamplingNum);

	float totalWeight = 0;
	for (int i = 0; i < totalSamplingNum; ++i)
	{
		int samplingOffset = i - samplingRadius;
		dstInfo[i].x = blurDir.x * (samplingOffset * texelSize.x);
		dstInfo[i].y = blurDir.y * (samplingOffset * texelSize.y);

		// 中心のピクセルのウェイトが大きくなる計算
		float weight = exp(-(samplingOffset * samplingOffset) / 18.0f);

		// サンプリングする各ピクセルに重みをつける
		dstInfo[i].z = weight;
		totalWeight += weight;
	}

	// ウェイトを全体のウェイトから割り算し、各ピクセルのウェイトの意味を割合に置き換える
	// 全部足して1になるように数値を調整する
	for (int i = 0; i < totalSamplingNum; ++i)
	{
		dstInfo[i].z /= totalWeight;
	}
}

void KdPostProcessShader::GenerateBlurTexture(std::shared_ptr<KdTexture>& spSrcTex, std::shared_ptr<KdTexture>& spDstTex, D3D11_VIEWPORT& VP, int blurRadius)
{
	D3D.WorkShaderManager().ChangeSamplerState(0, D3D.WorkShaderManager().m_ss_Linear_Clamp);

	RenderTargetPack tmpBlurRTPack;
	tmpBlurRTPack.CreateRenderTarget(spDstTex->GetWidth(), spDstTex->GetHeight());

	// 横にぼかす
	std::vector<Math::Vector3> horizontalBlurInfo;
	CreateBlurOffsetList(horizontalBlurInfo, spDstTex, blurRadius, { 1.0f, 0 });
	SetBlurInfo(horizontalBlurInfo);

	DrawTexture(&spSrcTex, 1, tmpBlurRTPack.m_RTTexture, tmpBlurRTPack.m_viewPort);

	// 横にぼかした画像を更に縦にぼかす
	std::vector<Math::Vector3> verticalBlurInfo;
	CreateBlurOffsetList(verticalBlurInfo, spDstTex, blurRadius, { 0, 1.0f });
	SetBlurInfo(verticalBlurInfo);

	DrawTexture(&tmpBlurRTPack.m_RTTexture, 1, spDstTex, VP);

	D3D.WorkShaderManager().UndoSamplerState();
}

void KdPostProcessShader::DrawTexture(std::shared_ptr<KdTexture>* spSrcTex, int srcTexSize, std::shared_ptr<KdTexture>& spDstTex, D3D11_VIEWPORT& VP)
{
	if (!spSrcTex) { return; }

	RenderTargetChanger RTChanger;

	RTChanger.ChangeRenderTarget(spDstTex, nullptr, &VP);

	ID3D11DeviceContext* pDevCon = D3D.WorkDevContext();

	// SRVのセット
	for (int i = 0; i < srcTexSize; ++i)
	{
		pDevCon->PSSetShaderResources(i, 1, spSrcTex[i]->WorkSRViewAddress());
	}

	// テクスチャーの描画
	D3D.DrawVertices(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, &m_screenVert[0], sizeof(Vertex));

	// SRVの解放
	ID3D11ShaderResourceView* nullSRV = nullptr;

	for (int i = 0; i < srcTexSize; ++i)
	{
		pDevCon->PSSetShaderResources(i, 1, &nullSRV);
	}

	RTChanger.UndoRenderTarget();
}

void KdPostProcessShader::SetBlurInfo(const std::shared_ptr<KdTexture>& spSrcTex, int samplingRadius, const Math::Vector2& dir)
{
	std::vector<Math::Vector3> blurOffsetList;

	CreateBlurOffsetList(blurOffsetList, spSrcTex, samplingRadius, dir);

	SetBlurInfo(blurOffsetList);
}

void KdPostProcessShader::SetBlurInfo(const std::vector<Math::Vector3>& srcInfo)
{
	KdPostProcessShader::cbBlur& blurInfo = m_cb0_BlurInfo.Work();

	blurInfo.SamplingNum = srcInfo.size();

	if (blurInfo.SamplingNum > kMaxSampling)
	{
		assert(0 && "サンプリング指定回数が上限を超えています。");

		blurInfo.SamplingNum = 0;

		return;
	}

	for (int i = 0; i < blurInfo.SamplingNum; ++i)
	{
		blurInfo.Info[i].x = srcInfo[i].x;
		blurInfo.Info[i].y = srcInfo[i].y;
		blurInfo.Info[i].z = srcInfo[i].z;
	}

	m_cb0_BlurInfo.Write();
}

void KdPostProcessShader::SetBlurToDevice()
{
	ID3D11DeviceContext* DevCon = D3D.WorkDevContext();
	if (!DevCon) { return; }

	m_cb0_BlurInfo.Write();

	D3D.WorkDevContext()->PSSetConstantBuffers(0, 1, m_cb0_BlurInfo.GetAddress());

	KdShaderManager& shaderMgr = D3D.WorkShaderManager();

	if (shaderMgr.SetVertexShader(m_VS))
	{
		DevCon->IASetInputLayout(m_inputLayout);
	}

	shaderMgr.SetPixelShader(m_PS_Blur);
}

void KdPostProcessShader::SetDoFToDevice()
{
	ID3D11DeviceContext* DevCon = D3D.WorkDevContext();
	if (!DevCon) { return; }

	m_cb0_DoFInfo.Write();

	D3D.WorkDevContext()->PSSetConstantBuffers(0, 1, m_cb0_DoFInfo.GetAddress());

	KdShaderManager& shaderMgr = D3D.WorkShaderManager();

	if (shaderMgr.SetVertexShader(m_VS))
	{
		DevCon->IASetInputLayout(m_inputLayout);
	}

	shaderMgr.SetPixelShader(m_PS_DoF);
}

void KdPostProcessShader::SetBrightToDevice()
{
	ID3D11DeviceContext* DevCon = D3D.WorkDevContext();
	if (!DevCon) { return; }

	m_cb0_BrightInfo.Write();

	D3D.WorkDevContext()->PSSetConstantBuffers(0, 1, m_cb0_BrightInfo.GetAddress());

	KdShaderManager& shaderMgr = D3D.WorkShaderManager();

	if (shaderMgr.SetVertexShader(m_VS))
	{
		DevCon->IASetInputLayout(m_inputLayout);
	}

	shaderMgr.SetPixelShader(m_PS_Bright);
}
