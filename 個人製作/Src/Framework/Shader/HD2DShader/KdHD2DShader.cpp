#include "Framework/KdFramework.h"

#include "KdHD2DShader.h"

void KdHD2DShader::SetMaterial(const KdMaterial& material, const Math::Vector4& colRate)
{
	//-----------------------
	// マテリアル情報を定数バッファへ書き込む
	//-----------------------
	m_cb2_Material.Work().BaseColor = material.BaseColor * colRate;
	m_cb2_Material.Work().Emissive = material.Emissive;
	m_cb2_Material.Work().Metallic = material.Metallic;
	m_cb2_Material.Work().Roughness = material.Roughness;
	m_cb2_Material.Write();

	//-----------------------
	// テクスチャセット
	//-----------------------
	ID3D11ShaderResourceView* srvs[4] =
	{
		material.BaseColorTex->WorkSRView(),			// BaseColor
		material.NormalTex->WorkSRView(),				// Normal
		material.MetallicRoughnessTex->WorkSRView(),	// Metallic Roughness
		material.EmissiveTex->WorkSRView()				// Emissive
	};

	// セット
	D3D.WorkDevContext()->PSSetShaderResources(0, _countof(srvs), srvs);
}

void KdHD2DShader::BeginGenerateDepthMapFromLight()
{
	if (D3D.WorkShaderManager().SetVertexShader(m_VS_GenDepthFromLight))
	{
		D3D.WorkShaderManager().SetInputLayout(m_inputLayout);

		D3D.WorkShaderManager().SetVSConstantBuffer(0, m_cb0_Obj.GetAddress());
		D3D.WorkShaderManager().SetVSConstantBuffer(1, m_cb1_Mesh.GetAddress());
	}

	if (D3D.WorkShaderManager().SetPixelShader(m_PS_GenDepthFromLight))
	{
		D3D.WorkShaderManager().SetPSConstantBuffer(0, m_cb0_Obj.GetAddress());
	}

	m_depthMapFromLightRTPack.ClearTexture(kRedColor);
	m_depthMapFromLightRTChanger.ChangeRenderTarget(m_depthMapFromLightRTPack);
}

void KdHD2DShader::EndGenerateDepthMapFromLight()
{
	m_depthMapFromLightRTChanger.UndoRenderTarget();
}

// 不透明描画の開始
void KdHD2DShader::BeginLighting()
{
	// 描画デバイスへのセット
	if (D3D.WorkShaderManager().SetVertexShader(m_VS))
	{
		D3D.WorkShaderManager().SetInputLayout(m_inputLayout);

		D3D.WorkShaderManager().SetVSConstantBuffer(0, m_cb0_Obj.GetAddress());
		D3D.WorkShaderManager().SetVSConstantBuffer(1, m_cb1_Mesh.GetAddress());
	}

	if (D3D.WorkShaderManager().SetPixelShader(m_PS))
	{
		D3D.WorkShaderManager().SetPSConstantBuffer(0, m_cb0_Obj.GetAddress());
		D3D.WorkShaderManager().SetPSConstantBuffer(2, m_cb2_Material.GetAddress());
	}

	// シャドウマップのSRVセット
	D3D.WorkDevContext()->PSSetShaderResources(10, 1, m_depthMapFromLightRTPack.m_RTTexture->WorkSRViewAddress());

	// 影ぼかし用の比較機能付きサンプラー
	D3D.WorkDevContext()->PSSetSamplers(1, 1, &D3D.GetShaderManager().m_ss_Linear_Clamp_Cmp);
}

// 不透明描画の終了
void KdHD2DShader::EndLighting()
{
	D3D.WorkShaderManager().UndoSamplerState();

	// シャドウマップのSRV解除
	ID3D11ShaderResourceView* pNullSRV = nullptr;
	D3D.WorkDevContext()->PSSetShaderResources(10, 1, &pNullSRV);
}

// 半透明描画の開始
void KdHD2DShader::BeginNoLighting()
{
	if (D3D.WorkShaderManager().SetVertexShader(m_VS_NoLighting))
	{
		D3D.WorkShaderManager().SetInputLayout(m_inputLayout);

		D3D.WorkShaderManager().SetVSConstantBuffer(0, m_cb0_Obj.GetAddress());
		D3D.WorkShaderManager().SetVSConstantBuffer(1, m_cb1_Mesh.GetAddress());
	}

	if (D3D.WorkShaderManager().SetPixelShader(m_PS_NoLighting))
	{
		D3D.WorkShaderManager().SetPSConstantBuffer(0, m_cb0_Obj.GetAddress());
		D3D.WorkShaderManager().SetPSConstantBuffer(2, m_cb2_Material.GetAddress());
	}
}

// 半透明描画の終了
void KdHD2DShader::EndNoLighting()
{
}

void KdHD2DShader::SetToDevice()
{
	if (D3D.WorkShaderManager().SetVertexShader(m_VS))
	{
		D3D.WorkShaderManager().SetInputLayout(m_inputLayout);

		D3D.WorkShaderManager().SetVSConstantBuffer(0, m_cb0_Obj.GetAddress());
		D3D.WorkShaderManager().SetVSConstantBuffer(1, m_cb1_Mesh.GetAddress());
	}

	if (D3D.WorkShaderManager().SetPixelShader(m_PS))
	{
		D3D.WorkDevContext()->PSSetConstantBuffers(2, 1, m_cb2_Material.GetAddress());
	}
}

void KdHD2DShader::DrawMesh(const KdMesh* mesh, const Math::Matrix& mWorld, const std::vector<KdMaterial>& materials, const Math::Vector4& col)
{
	if (mesh == nullptr)return;

	// 定数バッファ書き込み

	// メッシュ情報をセット
	mesh->SetToDevice();

	// 行列セット
	SetWorldMatrix(mWorld);

	m_cb1_Mesh.Write();

	// 全サブセット
	for (UINT subi = 0; subi < mesh->GetSubsets().size(); subi++)
	{
		// 面が１枚も無い場合はスキップ
		if (mesh->GetSubsets()[subi].FaceCount == 0)continue;

		// マテリアルセット
		const KdMaterial& material = materials[mesh->GetSubsets()[subi].MaterialNo];

		SetMaterial(material, col);

		//-----------------------
		// サブセット描画
		//-----------------------
		mesh->DrawSubset(subi);
	}
}

void KdHD2DShader::DrawModel(const KdModelData& rModel, const Math::Matrix& mWorld, const Math::Color& col)
{
	auto& dataNodes = rModel.GetOriginalNodes();

	// 全メッシュノードを描画
	for (auto& nodeIdx : rModel.GetMeshNodeIndices())
	{
		auto& rDataNode = dataNodes[nodeIdx];

		// 描画
		DrawMesh(rDataNode.m_spMesh.get(), rDataNode.m_worldTransform * mWorld, rModel.GetMaterials(), col);
	}
}

void KdHD2DShader::DrawModel(const KdModelWork& rModel, const Math::Matrix& mWorld, const Math::Color& col)
{
	// 有効じゃないときはスキップ
	if (!rModel.IsEnable()) { return; }

	const std::shared_ptr<KdModelData>& data = rModel.GetData();

	auto& workNodes = rModel.GetNodes();

	// モデルがないときはスキップ
	if (data == nullptr) { return; }

	auto& dataNodes = data->GetOriginalNodes();

	// 全メッシュノードを描画
	for (auto& nodeIdx : rModel.GetDrawMeshNodeIndices())
	{
		auto& rDataNode = dataNodes[nodeIdx];
		auto& rWorkNode = workNodes[nodeIdx];

		const std::shared_ptr<KdMesh>& spMesh = rDataNode.m_spMesh;

		// 行列セット
		SetWorldMatrix(rWorkNode.m_worldTransform * mWorld);

		m_cb1_Mesh.Write();

		// 描画
		DrawMesh(spMesh.get(), rWorkNode.m_worldTransform * mWorld, data->GetMaterials(), col);
	}
}

void KdHD2DShader::DrawSquarePolygon(KdSquarePolygon& rSquarePolygon, const Math::Matrix& mWorld, const Math::Color& col)
{
	if (!rSquarePolygon.IsEnable()) { return; }

	// 行列セット
	SetWorldMatrix(mWorld);
	m_cb1_Mesh.Write();

	if (rSquarePolygon.GetMaterial())
	{
		SetMaterial(*rSquarePolygon.GetMaterial(), col);
	}


	// 四角形ポリゴンの描画
	rSquarePolygon.Draw(mWorld);
}

void KdHD2DShader::DrawTrailPolygon(KdTrailPolygon& rTrailPolygon, const Math::Matrix& mWorld, const Math::Color& col)
{
	if (!rTrailPolygon.IsEnable()) { return; }

	// 行列セット
	SetWorldMatrix(mWorld);
	m_cb1_Mesh.Write();


	if (rTrailPolygon.GetMaterial())
	{
		SetMaterial(*rTrailPolygon.GetMaterial(), col);
	}

	// 四角形ポリゴンの描画
	rTrailPolygon.Draw();
}

bool KdHD2DShader::Init()
{
	//-------------------------------------
	// 頂点シェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
#include "KdHD2DShader_VS.inc"

		// 頂点シェーダー作成
		if (FAILED(D3D.WorkDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) {
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}

		// １頂点の詳細な情報
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 頂点入力レイアウト作成
		if (FAILED(D3D.WorkDev()->CreateInputLayout(
			&layout[0],				// 入力エレメント先頭アドレス
			layout.size(),			// 入力エレメント数
			&compiledBuffer[0],		// 頂点バッファのバイナリデータ
			sizeof(compiledBuffer),	// 上記のバッファサイズ
			&m_inputLayout))
			) {
			assert(0 && "CreateInputLayout失敗");
			Release();
			return false;
		}
	}

	{
#include "KdHD2DShader_VS_GenDepthMapFromLight.inc"

		// 頂点シェーダー作成
		if (FAILED(D3D.WorkDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS_GenDepthFromLight))) {
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}
	}

	{
#include "KdHD2DShader_VS_NoLighting.inc"

		// 頂点シェーダー作成
		if (FAILED(D3D.WorkDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS_NoLighting))) {
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// ピクセルシェーダ
	//-------------------------------------
	{
#include "KdHD2DShader_PS.inc"

		if (FAILED(D3D.WorkDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS))) {
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return false;
		}
	}

	{
#include "KdHD2DShader_PS_GenDepthMapFromLight.inc"

		if (FAILED(D3D.WorkDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS_GenDepthFromLight))) {
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return false;
		}
	} 
	
	{
#include "KdHD2DShader_PS_NoLighting.inc"

		if (FAILED(D3D.WorkDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS_NoLighting))) {
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return false;
		}
	}
	//-------------------------------------
	// 定数バッファ作成
	//-------------------------------------
	m_cb0_Obj.Create();
	m_cb1_Mesh.Create();
	m_cb2_Material.Create();

	std::shared_ptr<KdTexture> ds = std::make_shared<KdTexture>();
	ds->CreateDepthStencil(1024, 1024);
	D3D11_VIEWPORT vp = {
		0.0f, 0.0f,
		static_cast<float>(ds->GetWidth()),
		static_cast<float>(ds->GetHeight()),
		0.0f, 1.0f };

	m_depthMapFromLightRTPack.CreateRenderTarget(1024, 1024, true, DXGI_FORMAT_R32_FLOAT);
	m_depthMapFromLightRTPack.ClearTexture(kRedColor);

	return true;
}

void KdHD2DShader::Release()
{
	KdSafeRelease(m_VS);
	KdSafeRelease(m_VS_GenDepthFromLight);
	KdSafeRelease(m_VS_NoLighting);

	KdSafeRelease(m_inputLayout);
	
	KdSafeRelease(m_PS);
	KdSafeRelease(m_PS_GenDepthFromLight);
	KdSafeRelease(m_PS_NoLighting);

	m_cb0_Obj.Release();
	m_cb1_Mesh.Release();
	m_cb2_Material.Release();
}