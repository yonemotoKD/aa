#pragma once
//============================================================
//
// 基本シェーダ
//
//============================================================
class KdHD2DShader
{
	struct cbObject;
	struct cbMesh;
	struct cbMaterial;
public:

	//================================================
	// 設定・取得
	//================================================

	// UVタイリング設定
	void SetUVTiling(const Math::Vector2& tiling)
	{
		m_cb0_Obj.Work().UVTiling = tiling;
	}

	// UVオフセット設定
	void SetUVOffset(const Math::Vector2& offset)
	{
		m_cb0_Obj.Work().UVOffset = offset;
	}

	// ライト有効/無効
	void SetLightEnable(bool enable)
	{
		m_cb0_Obj.Work().LightEnable = enable;
	}

	// フォグ有効/無効
	void SetFogEnable(bool enable)
	{
		m_cb0_Obj.Work().FogEnable = enable;
	}

	// ワールド行列設定
	void SetWorldMatrix(const Math::Matrix& mWorld)
	{
		m_cb1_Mesh.Work().mW = mWorld;
	}

	// マテリアルのセット
	void SetMaterial(const KdMaterial& material, const Math::Vector4& colRate);

	// Object用定数バッファをそのまま返す
	KdConstantBuffer<cbObject>& ObjectCB()
	{
		return m_cb0_Obj;
	}

	// Mesh用定数バッファをそのまま返す
	KdConstantBuffer<cbMesh>& MeshCB()
	{
		return m_cb1_Mesh;
	}

	// material用定数バッファをそのまま返す
	KdConstantBuffer<cbMaterial>& MaterialCB()
	{
		return m_cb2_Material;
	}

	//================================================
	// 描画
	//================================================

	void BeginGenerateDepthMapFromLight();
	void EndGenerateDepthMapFromLight();

	void BeginLighting();
	void EndLighting();

	void BeginNoLighting();
	void EndNoLighting();

	// このシェーダをデバイスへセット
	void SetToDevice();

	// メッシュ描画
	// ・mesh			… 描画するメッシュ
	// ・materials		… 使用する材質配列
	void DrawMesh(const KdMesh* mesh, const Math::Matrix& mWorld, const std::vector<KdMaterial>& materials, const Math::Vector4& col);

	// モデルデータ描画
	// ・rModel			… 描画するモデル
	// ・mWorld			… 使用するワールド行列
	void DrawModel(const KdModelData& rModel, const Math::Matrix& mWorld = Math::Matrix::Identity, const Math::Color& col = kWhiteColor);

	// モデルワーク描画
	// ・rModel			… 描画するモデル
	// ・mWorld			… 使用するワールド行列
	void DrawModel(const KdModelWork& rModel, const Math::Matrix& mWorld = Math::Matrix::Identity, const Math::Color& col = kWhiteColor);

	// 四角ポリゴン描画
	void DrawSquarePolygon(KdSquarePolygon& rSquarePolygon,
		const Math::Matrix& mWorld = Math::Matrix::Identity, const Math::Color& col = kWhiteColor);

	// 四角ポリゴン描画
	void DrawTrailPolygon(KdTrailPolygon& rTrailPolygon,
		const Math::Matrix& mWorld = Math::Matrix::Identity, const Math::Color& col = kWhiteColor);

	//================================================
	// 初期化・解放
	//================================================

	// 初期化
	bool Init();
	// 解放
	void Release();
	// 
	~KdHD2DShader()
	{
		Release();
	}

	std::shared_ptr<KdTexture>& GetDepthTex() { return m_depthMapFromLightRTPack.m_RTTexture; }

private:

	// 定数バッファ(オブジェクト単位更新)
	struct cbObject
	{
		// UV操作
		Math::Vector2	UVOffset = { 0,0 };
		Math::Vector2	UVTiling = { 1,1 };

		// フォグ有効
		int				LightEnable = 1;
		int				FogEnable = 1;
		float			DissolveThreshold = 0.0f;
		float			_blank = 0.0f;
	};

	// 定数バッファ(メッシュ単位更新)
	struct cbMesh
	{
		Math::Matrix		mW;
	};

	// 定数バッファ(マテリアル単位更新)
	struct cbMaterial {
		Math::Vector4	BaseColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		Math::Vector3	Emissive = { 1.0f, 1.0f, 1.0f };
		float			Metallic = 0.0f;

		float			Roughness = 1.0f;
		float			_blank[3] = { 0.0f, 0.0f ,0.0f };;
	};

	// 3Dモデル用シェーダ
	ID3D11VertexShader* m_VS = nullptr;						// 頂点シェーダー：陰影あり
	ID3D11VertexShader* m_VS_GenDepthFromLight = nullptr;	// 頂点シェーダー：光からの深度
	ID3D11VertexShader* m_VS_NoLighting = nullptr;			// 頂点シェーダー：陰影なし

	ID3D11InputLayout* m_inputLayout = nullptr;				// 頂点入力レイアウト
	
	ID3D11PixelShader* m_PS = nullptr;						// ピクセルシェーダー：陰影あり
	ID3D11PixelShader* m_PS_GenDepthFromLight = nullptr;	// ピクセルシェーダー：影生成
	ID3D11PixelShader* m_PS_NoLighting = nullptr;			// ピクセルシェーダー：陰影なし

	KdConstantBuffer<cbObject>		m_cb0_Obj;
	KdConstantBuffer<cbMesh>		m_cb1_Mesh;
	KdConstantBuffer<cbMaterial>	m_cb2_Material;

	RenderTargetPack	m_depthMapFromLightRTPack;
	RenderTargetChanger m_depthMapFromLightRTChanger;
};