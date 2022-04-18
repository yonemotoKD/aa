#pragma once

// 四角形ポリゴンクラス
class KdSquarePolygon
{
public:

	enum class PivotType
	{
		Right_Top,
		Right_Middle,
		Right_Bottom,
		Center_Top = 10,
		Center_Middle,
		Center_Bottom,
		Left_Top = 20,
		Left_Middle,
		Left_Bottom,
	};

	KdSquarePolygon(const Math::Vector2* _pScale = nullptr, const Math::Color* _pColor = nullptr, const Math::Rectangle* _pRect = nullptr);

	// 描画の幅と高さの設定
	void SetScale(const Math::Vector2& _scale, PivotType pivot = PivotType::Center_Middle);
	void SetPivot(PivotType pivot);

	// テクスチャの描画合成色の設定
	void SetColor(const Math::Color& _color);

	// テクスチャ内の描画エリアの設定
	void SetUVRect(int FrameCnt);
	void SetUVRect(const Math::Rectangle& _rect);
	void SetUVRect(const Math::Vector2& _minUV, const Math::Vector2& _maxUV);

	void SetInfo(const Math::Vector2* pScale, const Math::Color* pColor = nullptr, const Math::Rectangle* pRect = nullptr);

	// マテリアルをセット
	inline void SetTexture(const std::shared_ptr<KdTexture>& tex)
	{
		if (!m_spMaterial) { m_spMaterial = std::make_shared<KdMaterial>(); }

		m_spMaterial->BaseColorTex = tex;
	}

	void SetMaterial(const std::string_view filename);

	// テクスチャの分割数を設定
	inline void SetSplit(UINT splitX, UINT splitY)
	{
		m_splitX = splitX;
		m_splitY = splitY;
	}

	// 描画
	void Draw(const Math::Matrix& mWorld);

	bool IsEnable() const { return (m_enable && m_spMaterial); }
	void SetEnable(bool flag) { m_enable = flag; }

	UINT GetSplitX() { return m_splitX; }
	UINT GetSplitY() { return m_splitY; }

	const std::shared_ptr<KdMaterial>& GetMaterial() { return m_spMaterial; }

protected:

	void InitScale();
	void InitRect();

	// １頂点の形式
	struct Vertex
	{
		Math::Vector3 pos;
		Math::Vector2 UV;
		Math::Vector3 normal = Math::Vector3::Backward;
		unsigned int  color = 0xFFFFFFFF;
		Math::Vector3 tangent = Math::Vector3::Left;
	};

	// 頂点配列
	Vertex m_vertex[4];

	UINT m_splitX = 1;
	UINT m_splitY = 1;

	std::shared_ptr<KdMaterial> m_spMaterial;

	bool m_enable = true;
};