#pragma once

class KdTrailPolygon 
{
public:
	enum class Trail_Pattern
	{
		eDefault,
		eBillboard,
		eVertices
	};

private:
	// １頂点の形式
	struct Vertex
	{
		Math::Vector3 Pos;
		Math::Vector2 UV;
		Math::Vector3 normal = Math::Vector3::Backward;
		unsigned int  color = 0xFFFFFFFF;
		Math::Vector3 tangent = Math::Vector3::Left;
	};

public:
	// 先頭のポイントを取得
	inline Math::Matrix* GetTopPoint()
	{
		if (m_pointList.size() == 0) { return nullptr; }
		return &m_pointList.front();
	}

	// ポイントを追加
	inline void AddPoint(const Math::Matrix& mat)
	{
		m_pointList.push_front(mat);
	}

	// 最後尾のポイントを削除
	inline void DelPoint_Back()
	{
		m_pointList.pop_back();
	}

	// リストの数を取得
	inline int GetNumPoints() const
	{
		return (int)m_pointList.size();
	}

	// 情報（画像・描画パターン・幅）のセット
	void SetInfo(const std::shared_ptr<KdTexture>& tex = nullptr, Trail_Pattern pattern = Trail_Pattern::eDefault);

	// 画像をセット
	inline void SetTexture(const std::shared_ptr<KdTexture>& tex)
	{
		if (!m_spMaterial) { m_spMaterial = std::make_shared<KdMaterial>(); }

		m_spMaterial->BaseColorTex = tex;
	}

	inline void SetPattern(Trail_Pattern pattern) { m_pattern = pattern; }

	void Draw() const;

	bool IsEnable() const { return (m_enable && m_spMaterial && m_pointList.size() >= (UINT)(2 * ((m_pattern == Trail_Pattern::eVertices) + 1))); }
	void SetEnable(bool enable){ m_enable = enable; }

	const std::shared_ptr<KdMaterial>& GetMaterial() { return m_spMaterial; }

private:

	// 描画用の頂点リスト作成
	void CreateVertices(std::vector<Vertex>& vertex) const;

	// 通常描画頂点リストの作成
	void CreateWithDefaultPattern(std::vector<Vertex>& vertex) const;

	// ビルボード描画
	void CreateWithBillboardPattern(std::vector<Vertex>& vertex) const;

	// 頂点情報をそのまま繋げてポリゴンを作成
	void CreateWithVerticesPattern(std::vector<Vertex>& vertex) const;

	// 軌跡の位置などを記憶する行列配列
	std::deque<Math::Matrix>	m_pointList;

	// マテリアル
	std::shared_ptr<KdMaterial> m_spMaterial;

	Trail_Pattern m_pattern = Trail_Pattern::eDefault;

	bool m_enable = true;
};